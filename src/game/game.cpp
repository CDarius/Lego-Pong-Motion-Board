#include "game/game.hpp"

bool playerIsAI(GamePlayer player, GameMode mode) {
    if (mode == GameMode::PLAYER_VS_PLAYER)
        return false;
    if (mode == GameMode::AI_VS_AI)
        return true;
    if (mode == GameMode::PLAYER_VS_AI)
        return (player == GamePlayer::R);

    return false;
}

pbio_error_t Game::run(GamePlayer startPlayer, GameMode mode, CancelToken& cancelToken, bool logGame) {
    // Init logger
    if (logGame)
        _logger.start(GAME_LOG_MAX_ENTRIES);
    else
        _logger.deleteLog();

    // Init game
    _scoreL = 0;
    _scoreR = 0;
    _ioBoard.showScore(0, 0);
    GamePlayer servingPlayer = startPlayer;

    _lPlayerIsAI = playerIsAI(GamePlayer::L, mode);
    _rPlayerIsAI = playerIsAI(GamePlayer::R, mode);
    _AIPlayerMaxMoveStep = _settings.aiPlayer.paddleMaxSpeed * ((float)GAME_LOOP_PERIOD_MS / 1000.0f);

    // Get axes software limits
    _xSwLimitM = _xMotor.getSwLimitMinus();
    _xSwLimitP = _xMotor.getSwLimitPlus();
    _ySwLimitM = _yMotor.getSwLimitMinus();
    _ySwLimitP = _yMotor.getSwLimitPlus();
    _lSwLimitM = _lMotor.getSwLimitMinus();
    _lSwLimitP = _lMotor.getSwLimitPlus();
    _rSwLimitM = _rMotor.getSwLimitMinus();
    _rSwLimitP = _rMotor.getSwLimitPlus();
    
    // Game loop
    while (true) {
        // Move the ball in front of the serving player paddle
        PBIO_RETURN_ON_ERROR(moveBallToPaddle(servingPlayer, cancelToken));

        bool servingPlayerIsAI = playerIsAI(servingPlayer, mode);

        if (!servingPlayerIsAI) {
            // Ensure that serving player is not pushing the encoder
            EncoderMultiJog* servingEncoder = (servingPlayer == GamePlayer::L) ? &_lEncoderJog : &_rEncoderJog;
            bool buttonPressed = true;
            servingEncoder->getEncoder()->isButtonPressed(buttonPressed);
            while (buttonPressed)
            {
                IF_CANCELLED(cancelToken, {
                    return PBIO_ERROR_CANCELED;
                });

                servingEncoder->getEncoder()->isButtonPressed(buttonPressed);

                delay(100);
            }
        }
        
        // Stop the jog to force to reload all jog parameter at the next start
        _lEncoderJog.stop();
        _rEncoderJog.stop();

        // Enable jog for non AI players
        if (!_lPlayerIsAI)
            _lEncoderJog.start(Axes::L);
        if (!_rPlayerIsAI)
            _rEncoderJog.start(Axes::R);

        // Wait for the serving player to throw the ball
        if (servingPlayerIsAI) {
            PBIO_RETURN_ON_ERROR(aiPlayerServeBall(servingPlayer, cancelToken));
        } else {
            PBIO_RETURN_ON_ERROR(humanPlayerServeBall(servingPlayer, cancelToken));
        }

        // Throw the ball
        throwBall(servingPlayer);

        // Bounce the ball until a player scores
        _lastBallUpdateTime = millis() - GAME_LOOP_PERIOD_MS;
        while (true) {
            IF_CANCELLED(cancelToken, {
                _lEncoderJog.stop();
                _rEncoderJog.stop();
                _xMotor.stop();
                _yMotor.stop();
                _lMotor.stop();
                _rMotor.stop();
                return PBIO_ERROR_CANCELED;
            });

            // Update axes positions
            _ballX = _xMotor.angle();
            _ballY = _yMotor.angle();
            _paddleL = _lMotor.angle();
            _paddleR = _rMotor.angle();

            GAME_LOG_NEW_CYCLE(_ballX, _ballY, _paddleL, _paddleR);

            // Calculate new ball position
            unsigned long now = millis();
            _deltaTimeS = ((float)(now - _lastBallUpdateTime)) / 1000.0f;
            _lastBallUpdateTime = now;
            _targetBallX += _speedX * _deltaTimeS;
            _targetBallY += _speedY * _deltaTimeS;

            GAME_LOG_SUB_CYCLE;

            // Jog the paddles for non AI players
            if (!_lPlayerIsAI)
                _lEncoderJog.update();
            if (!_rPlayerIsAI)
                _rEncoderJog.update();

            // Update AI players
            if (_lPlayerIsAI)
                aiPlayer(GamePlayer::L);
            if (_rPlayerIsAI)
                aiPlayer(GamePlayer::R);

            GAME_LOG_SUB_CYCLE;

            // Bounce the ball on top or bottom border
            bounceBallTopBottom();

            GAME_LOG_SUB_CYCLE;

            // Bounce the ball on paddles
            bool isBallAtPaddleBounceLimitL = isBallAtPaddleBounceLimit(GamePlayer::L);
            bool isBallAtPaddleBounceLimitR = isBallAtPaddleBounceLimit(GamePlayer::R);
            if (isBallAtPaddleBounceLimitL) {
                bounceOnPaddle(GamePlayer::L);
            } else if (isBallAtPaddleBounceLimitR) {
                bounceOnPaddle(GamePlayer::R);
            }

            GAME_LOG_SUB_CYCLE;
            
            // Test for player miss and score
            if (_speedX > 0) {
                float scroreX = _xSwLimitP - _overshootX;
                if (_ballX >= scroreX) {
                    // Player R missed the ball
                    _rEncoderJog.stop(); // Block R player encoder
                    _scoreL++;
                    servingPlayer = GamePlayer::R;
                    // Exit the bounce loop
                    break;
                }
            } else {
                float scroreX = _xSwLimitM + _overshootX;
                if (_ballX <= scroreX) {
                    // Player L missed the ball
                    _lEncoderJog.stop(); // Block L player encoder
                    _scoreR++;
                    servingPlayer = GamePlayer::L;
                    // Exit the bounce loop
                    break;
                }
            }

            // Limit _targetBallX and _targetBallY to max 2 studs difference from _ballX and _ballY
            float maxTargetBallX = _ballX + 2.0f;
            float minTargetBallX = _ballX - 2.0f;
            float maxTargetBallY = _ballY + 2.0f;
            float minTargetBallY = _ballY - 2.0f;
            if (_targetBallX > maxTargetBallX) _targetBallX = maxTargetBallX;
            if (_targetBallX < minTargetBallX) _targetBallX = minTargetBallX;
            if (_targetBallY > maxTargetBallY) _targetBallY = maxTargetBallY;
            if (_targetBallY < minTargetBallY) _targetBallY = minTargetBallY;

            GAME_LOG_SUB_CYCLE;

            // Final limit to avid ball and paddle clash
            if (isBallAtPaddleBounceLimitL)
                limitPaddleOrBallToAvoidCollision(GamePlayer::L);
            if (isBallAtPaddleBounceLimitR)
                limitPaddleOrBallToAvoidCollision(GamePlayer::R);

            GAME_LOG_SUB_CYCLE;

            // Limit _targetBall x and y to software limits
            if (_targetBallX > _xSwLimitP) 
                _targetBallX = _xSwLimitP;
            if (_targetBallX < _xSwLimitM)
                _targetBallX = _xSwLimitM;
            if (_targetBallY > _ySwLimitP)
                _targetBallY = _ySwLimitP;
            if (_targetBallY < _ySwLimitM)
                _targetBallY = _ySwLimitM;

            GAME_LOG_SUB_CYCLE;

            // Move the ball to the target position
            _xMotor.track_target(_targetBallX);
            _yMotor.track_target(_targetBallY);

            delay(GAME_LOOP_PERIOD_MS);
        }

        // Test for game end
        if (_scoreL >= GAME_WIN_SCORE || _scoreR >= GAME_WIN_SCORE)
            break;

        // Game continue, display the scrore
        _ioBoard.playSound(IO_BOARD_SOUND_SCORE);
        _ioBoard.showScore(_scoreL, _scoreR, SCROLLING_SCORE_ANIM_DELAY);

    }

    // Game end, one player has won
    _ioBoard.showScore(_scoreL, _scoreR, SCROLLING_SCORE_ANIM_DELAY, SCRORE_BLINK_DELAY);
    for (int i = 0; i < 3; i++) {
        _ioBoard.playSound(IO_BOARD_SOUND_SCORE);
        delay(600);
    }
    _ioBoard.showScore(_scoreL, _scoreR);
    
    return PBIO_SUCCESS;
}

void Game::resetDisplay() {
    _ioBoard.showScore(0, 0);
}

pbio_error_t Game::moveBallToPaddle(GamePlayer paddle, CancelToken& cancelToken) {
    float ballX, ballY;
    if (paddle == GamePlayer::L) {
        ballY = _lMotor.angle() + GAME_PADDLE_H / 2.0 - GAME_BALL_L / 2.0;
        ballX = _xSwLimitM + GAME_PADDLE_W + _settings.xAxis.ballPaddleDistance;
    } else {
        ballY = _rMotor.angle() + GAME_PADDLE_H / 2.0 - GAME_BALL_L / 2.0;
        ballX = _xSwLimitP - GAME_BALL_L - _settings.xAxis.ballPaddleDistance;
    }

    if (isBallOnThePaddleColumn(paddle)) {
        // Make the C movement to return the ball in front of the player's paddle
        float x1;
        if (paddle == GamePlayer::L) {
            x1 = ballX + GAME_PADDLE_BALL_X_DIST_COLUMN;
        } else {
            x1 = ballX - GAME_PADDLE_BALL_X_DIST_COLUMN;
        }

        // Move the ball on X past the paddle
        PBIO_RETURN_ON_ERROR(_xMotor.run_target(
            _xMotor.get_speed_limit(),
            x1,
            PBIO_ACTUATION_HOLD,
            true,
            &cancelToken
        ));

        // Center the ball Y on the paddle
        PBIO_RETURN_ON_ERROR(_yMotor.run_target(
            _yMotor.get_speed_limit(),
            ballY,
            PBIO_ACTUATION_HOLD,
            true,
            &cancelToken
        ));

        // Move the ball X next on the paddle
        PBIO_RETURN_ON_ERROR(_xMotor.run_target(
            _xMotor.get_speed_limit(),
            ballX,
            PBIO_ACTUATION_HOLD,
            true,
            &cancelToken
        ));
    } else {
        GamePlayer otherPlayer = OTHER_GAME_PLAYER(paddle);
        if (isBallOnThePaddleColumn(otherPlayer)) {
            // Move the ball outside the other paddle column
            float x;
            if (otherPlayer == GamePlayer::L) {
                x = _xSwLimitM + GAME_PADDLE_W + GAME_PADDLE_BALL_X_DIST_COLUMN / 2.0f;
            } else {
                x = _xSwLimitP - GAME_PADDLE_W - GAME_PADDLE_BALL_X_DIST_COLUMN / 2.0f;
            }

            PBIO_RETURN_ON_ERROR(_xMotor.run_target(
                _xMotor.get_speed_limit(),
                x,
                PBIO_ACTUATION_HOLD,
                true,
                &cancelToken
            ));
        }

        // Move the ball in front of the paddle
        PBIO_RETURN_ON_ERROR(moveBallCloseLoop(ballX, ballY, cancelToken));
    }

    return PBIO_SUCCESS;
}

void Game::ballTrackPaddle(IMotorHoming& axis) {
    float ballY = axis.angle() + GAME_PADDLE_H / 2.0 - GAME_BALL_L / 2.0;
    _yMotor.track_target(ballY);
}

bool Game::isBallOnThePaddleColumn(GamePlayer paddle) const {
    if (paddle == GamePlayer::L) {
        return _xMotor.angle() < (_xSwLimitM + GAME_PADDLE_W + GAME_PADDLE_BALL_X_DIST_COLUMN);
    } else {
        return _xMotor.angle() > (_xSwLimitP - GAME_PADDLE_W - GAME_PADDLE_BALL_X_DIST_COLUMN);
    }
}

pbio_error_t Game::moveBallCloseLoop(float x, float y, CancelToken& cancelToken, float speedX, float speedY) {
    float deltaX = x - _xMotor.angle();
    float deltaY = y - _yMotor.angle();
    float absDeltaX = fabs(deltaX);
    float absDeltaY = fabs(deltaY);

    float maxSpeedX = isnan(speedX) ? _xMotor.get_speed_limit() : speedX;
    float maxSpeedY = isnan(speedY) ? _yMotor.get_speed_limit() : speedY;

    float deltaTimeX = absDeltaX / maxSpeedX;
    float deltaTimeY = absDeltaY / maxSpeedY;

    IMotorHoming *masterMotor, *slaveMotor;
    float startMaster, startSlave;
    float destMaster, destSlave;
    float deltaMaster, deltaSlave;

    if (deltaTimeX > deltaTimeY) {
        // X is the longest movement, Y follows
        masterMotor = &(_xMotor);
        slaveMotor = &(_yMotor);
        startMaster = x;
        startSlave = y;
        destMaster = x;
        destSlave = y;
        deltaMaster = deltaX;
        deltaSlave = deltaY;
    } else {
        // Y is the longest movement, X follows
        masterMotor = &(_yMotor);
        slaveMotor = &(_xMotor);
        startMaster = y;
        startSlave = x;
        destMaster = y;
        destSlave = x;
        deltaMaster = deltaY;
        deltaSlave = deltaX;
    }

    // Start the master motor to run to its target position
    pbio_error_t err = masterMotor->run_target(masterMotor->get_speed_limit(), destMaster, PBIO_ACTUATION_HOLD, false, &cancelToken);
    if (err != PBIO_SUCCESS) {
        return err;
    }

    // Make the slave motor follow the master motor
    while (!masterMotor->is_completion()) {
        IF_CANCELLED(cancelToken, {
            masterMotor->stop();
            slaveMotor->stop();
            return PBIO_ERROR_CANCELED;
        });

        float masterPos = masterMotor->angle();
        float progress = (masterPos - startMaster) / deltaMaster;
        float targetSlavePos = progress * deltaSlave + startSlave;

        slaveMotor->track_target(targetSlavePos);

        delay(2 * PBIO_CONFIG_SERVO_PERIOD_MS);
    }

    // Ensure that slave motor is arrived at the target position too
    err = slaveMotor->run_target(slaveMotor->get_speed_limit(), destSlave, PBIO_ACTUATION_HOLD, true, &cancelToken);

    return err;
}

pbio_error_t Game::humanPlayerServeBall(GamePlayer player, CancelToken& cancelToken) {
    // Wait for the serving player to throw the ball    
    Button encoderButton;
    EncoderMultiJog* servingEncoder = (player == GamePlayer::L) ? &_lEncoderJog : &_rEncoderJog;

    while (!encoderButton.wasPressed())
    {
        IF_CANCELLED(cancelToken, {
            return PBIO_ERROR_CANCELED;
        });

        // Both players move the paddles with their encoders if they are not AI
        if (!_lPlayerIsAI)
            _lEncoderJog.update();
        if (!_rPlayerIsAI)
            _rEncoderJog.update();

        // Ball track the serving player paddle
        ballTrackPaddle(*(servingEncoder->getMotor()));

        bool rawButtonPressed;
        servingEncoder->getEncoder()->isButtonPressed(rawButtonPressed);
        encoderButton.setRawState(millis(), rawButtonPressed);

        delay(GAME_LOOP_PERIOD_MS);
    }

    return PBIO_SUCCESS;
}

pbio_error_t Game::aiPlayerServeBall(GamePlayer player, CancelToken& cancelToken) {
    IMotorHoming* servingMotor = (player == GamePlayer::L) ? &(_lMotor) : &(_rMotor);

    // Little delay before moving the paddle
    unsigned long startTime = millis();
    while ((millis() - startTime) < 1000) {
        IF_CANCELLED(cancelToken, {
            return PBIO_ERROR_CANCELED;
        });

        // Both players move the paddles with their encoders if they are not AI
        if (!_lPlayerIsAI)
            _lEncoderJog.update();
        if (!_rPlayerIsAI)
            _rEncoderJog.update();

        delay(GAME_LOOP_PERIOD_MS);
    }

    // Move the paddle to a random position
    float paddlePos = servingMotor->getSwLimitMinus() + (float)random(0, 10001) / 10000.0f * (servingMotor->getSwLimitPlus() - servingMotor->getSwLimitMinus() - GAME_PADDLE_H);
    PBIO_RETURN_ON_ERROR(servingMotor->run_target(
        servingMotor->get_speed_limit() * 0.5f,
        paddlePos,
        PBIO_ACTUATION_HOLD,
        false,
        &cancelToken
    ));

    // Ball track the moving paddle
    while (!servingMotor->is_completion()) {
        IF_CANCELLED(cancelToken, {
            servingMotor->stop();
            return PBIO_ERROR_CANCELED;
        });

        // Move the other player paddle with its encoder if it is not AI
        if (!_lPlayerIsAI)
            _lEncoderJog.update();
        if (!_rPlayerIsAI)
            _rEncoderJog.update();

        // Ball track the serving player paddle
        ballTrackPaddle(*servingMotor);

        delay(PBIO_CONFIG_SERVO_PERIOD_MS);
    }

    if (player == GamePlayer::L) {
        _lAIPlayerActualYSetpoint = paddlePos;
        _lAIPlayerTargetY = paddlePos;
    } else {
        _rAIPlayerActualYSetpoint = paddlePos;
        _rAIPlayerTargetY = paddlePos;
    }

    return PBIO_SUCCESS;
}

void Game::throwBall(GamePlayer player) {
    _speedX = _settings.xAxis.startBallGameSpeed * ((player == GamePlayer::L) ? 1.0f : -1.0f);
    _speedY = ((float)random(-10000, 10001) / 10000.0f) * _settings.yAxis.ballServeSpeedMax;
    Serial.println(String("Throw ball speedX: ") + String(_speedX) + String(" speedY: ") + String(_speedY));

    _overshootX = getXInversionOvershoot(_speedX);
    _overshootY = getYInversionOvershoot(_speedY);
    _ioBoard.playSound(IO_BOARD_SOUND_PADDLE);
}

void Game::bounceBallTopBottom() {
    bool invert;
    if (_speedY > 0.0f) {
        // Going down
        float border = _ySwLimitP - _overshootY;

        if (_targetBallY > border) {
            invert = true;
            _targetBallY = border - (_targetBallY - border);
        }
    } else {
        // Going up
        float border = _ySwLimitM + _overshootY;

        if (_targetBallY < border) {
            invert = true;
            _targetBallY = border + (border - _targetBallY);
        }
    }

    if (invert) {
        _speedY = -(_speedY);
        _ioBoard.playSound(IO_BOARD_SOUND_WALL);
    }
}

bool Game::isBallAtPaddleBounceLimit(GamePlayer paddle) const {
    if (paddle == GamePlayer::R) {
        // Right paddle
        float border = _xSwLimitP - _settings.xAxis.paddleCollisionTolerance - GAME_PADDLE_W;

        return (_ballX + _overshootX) > border;
    } else {
        // Left paddle
        float border = _xSwLimitM + _settings.xAxis.paddleCollisionTolerance + GAME_PADDLE_W;

        return (_ballX - _overshootX) < border;
    }
}

void Game::bounceOnPaddle(GamePlayer paddle) {
    float paddleY = (paddle == GamePlayer::R) ? _paddleR : _paddleL;
    
    float yMin = paddleY - _settings.yAxis.paddleCollisionTolerance - GAME_BALL_L;
    float yMax = paddleY + _settings.yAxis.paddleCollisionTolerance + GAME_PADDLE_H;
    
    bool inRange = (_ballY >= yMin && _ballY <= yMax);
    if (inRange) {
        // The ball is in front of the paddle, bounce
        bool towardPaddleX = paddle == GamePlayer::R ? (_speedX > 0.0f) : (_speedX < 0.0f);
        if (towardPaddleX) {
            // The ball is going toward the paddle, bounce inverting X-Axis speed

            // Invert and increment X-Axis speed
            if (_speedX > 0.0f) {
                _speedX += _settings.xAxis.ballBounceSpeedIncrement;
                if (_speedX > _settings.xAxis.ballBounceMaxSpeed)
                    _speedX = _settings.xAxis.ballBounceMaxSpeed;
                _speedX = -_speedX;
            } else {
                _speedX = (-_speedX) + _settings.xAxis.ballBounceSpeedIncrement;
                if (_speedX > _settings.xAxis.ballBounceMaxSpeed)
                    _speedX = _settings.xAxis.ballBounceMaxSpeed;
            }
            _overshootX = getXInversionOvershoot(_speedX);

            // Calculate the new Y-Axis speed based on the position relative to paddle
            float relativePosition = 2.0f * (_ballY - yMin) / (yMax - yMin) - 1.0f;
            _speedY = relativePosition * _settings.yAxis.ballBounceSpeedMax;
            _overshootY = getYInversionOvershoot(_speedY);

            _ioBoard.playSound(IO_BOARD_SOUND_PADDLE);

            // Update the target ball positon
            _targetBallX = _ballX + _speedX * _deltaTimeS;
            _targetBallY = _ballY + _speedY * _deltaTimeS;

            // When the X-Axis speed is at maximum, also increment the L/R-Axes jog speed
            if (fabs(_speedX) == _settings.xAxis.ballBounceMaxSpeed) {
                float jogMultiplier = _lEncoderJog.getEncoderMultiplier();
                if (jogMultiplier > _settings.lrAxis.minJogEncoderMultiplier) {
                    jogMultiplier -= _settings.lrAxis.jogMultiplierDecrement;
                    if (jogMultiplier < _settings.lrAxis.minJogEncoderMultiplier) {
                        jogMultiplier = _settings.lrAxis.minJogEncoderMultiplier;
                    }
                    _lEncoderJog.setEncoderMultiplier(jogMultiplier);
                    _rEncoderJog.setEncoderMultiplier(jogMultiplier);
                }
            }
        } else {
            // The ball isn’t going toward the paddle
            // Test if the ball is on the shoulder of the paddle
            bool onShoulder = paddle == GamePlayer::R ? 
                (_ballX >= (_xSwLimitP - GAME_PADDLE_W)) 
                : (_ballX <= (_xSwLimitM + GAME_PADDLE_W));

            if (onShoulder) {
                // Test is goinge toward the paddle
                bool ballIsBelowPaddle = _ballY > (paddleY + GAME_PADDLE_H / 2.0f - GAME_BALL_L / 2.0f);
                bool towardPaddle = _speedY > 0.0f ? !ballIsBelowPaddle : ballIsBelowPaddle;
                if (towardPaddle) {
                    // The ball is going toward the paddle, bounce inverting Y-Axis speed
                    _speedY = -(_speedY);
                    _targetBallY = _ballY + _speedY * _deltaTimeS;
                }
            }
        }     
    }
}

void Game::limitPaddleOrBallToAvoidCollision(GamePlayer paddle) {
    float paddleY = (paddle == GamePlayer::L) ? _paddleL : _paddleR;
    bool ballIsBelowPaddle = _ballY > (paddleY + GAME_PADDLE_H / 2.0f - GAME_BALL_L / 2.0f);
    if (ballIsBelowPaddle) {
        if (_speedY > 0.0f) {
            // Ball is below the paddle and going down, limit the paddle max position
            float maxPaddleY = _ballY - GAME_PADDLE_H;
            float paddleTargetY = getPaddleTargetY(paddle);
            if (paddleTargetY > maxPaddleY) {
                // Limit the paddle position
                movePlayerPaddleToY(paddle, maxPaddleY);
            }
        } else {
            // Ball is below the paddle and going up, limit the ball min position
            float minBallY = paddleY + GAME_PADDLE_H;
            if (_targetBallY < minBallY) {
                _targetBallY = minBallY;
            }
        }

        // Last test to avoid the ball pushed in the corner by the paddle
        if (_targetBallY > _ySwLimitP) {
            _targetBallY = _ySwLimitP;
            float maxPaddleY = _targetBallY - GAME_PADDLE_H;
            float paddleTargetY = getPaddleTargetY(paddle);
            if (paddleTargetY > maxPaddleY) {
                // Limit the paddle position
                movePlayerPaddleToY(paddle, maxPaddleY);
            }            
        }
    } else {
        if (_speedY < 0.0f) {
            // Ball is above the paddle and going up, limit the paddle min position
            float minPaddleY = _ballY + GAME_BALL_L;
            float paddleTargetY = getPaddleTargetY(paddle);
            if (paddleTargetY < minPaddleY) {
                // Limit the paddle position
                movePlayerPaddleToY(paddle, minPaddleY);
            }
        } else {
            // Ball is above the paddle and going down, limit the ball max position
            float maxBallY = paddleY - GAME_BALL_L;
            if (_targetBallY > maxBallY) {
                _targetBallY = maxBallY;
            }
        }

        // Last test to avoid the ball pushed in the corner by the paddle
        if (_targetBallY < _ySwLimitM) {
            _targetBallY = _ySwLimitM;
            float minPaddleY = _targetBallY + GAME_BALL_L;
            float paddleTargetY = getPaddleTargetY(paddle);
            if (paddleTargetY < minPaddleY) {
                // Limit the paddle position
                movePlayerPaddleToY(paddle, minPaddleY);
            }            
        }
    }
}

float Game::getPaddleTargetY(GamePlayer player) const {
    if (player == GamePlayer::L) {
        if (_lPlayerIsAI) {
            return _lAIPlayerTargetY;
        } else {
            return _lEncoderJog.getPosSetpoint();
        }
    } else {
        if (_rPlayerIsAI) {
            return _rAIPlayerTargetY;
        } else {
            return _rEncoderJog.getPosSetpoint();
        }
    }
}

void Game::movePlayerPaddleToY(GamePlayer player, float y) {
    if (player == GamePlayer::L) {
        if (_lPlayerIsAI) {
            _lAIPlayerTargetY = y;
            _lMotor.track_target(y);
        } else {
            _lEncoderJog.overridePosSetpoint(y);
        }
    } else {
        if (_rPlayerIsAI) {
            _rAIPlayerTargetY = y;
            _rMotor.track_target(y);
        } else {
            _rEncoderJog.overridePosSetpoint(y);
        }
    }
}

void Game::aiPlayer(GamePlayer player) {
    unsigned long now = millis();
    uint16_t deltaTime = now - _lastAIUpdateTime;
    bool calcNewPosition = deltaTime < _settings.aiPlayer.playerUpdateTimeMs;
    _lastAIUpdateTime = now;

    if (calcNewPosition) {
        // Ideal tracking position
        float paddleTargetPos = _ballY + GAME_BALL_L / 2.0f - GAME_PADDLE_H / 2.0f;
        // Add some random error
        paddleTargetPos += ((float)random(-10000, 10001) / 10000.0f) * _settings.aiPlayer.paddleMaxError;

        // Limit target pos on software limits
        if (paddleTargetPos < _rSwLimitM) {
            paddleTargetPos = _rSwLimitM;
        } else if (paddleTargetPos > _rSwLimitP) {
            paddleTargetPos = _rSwLimitP;
        }

        // Set the new target position
        if (player == GamePlayer::L) {
            _lAIPlayerTargetY = paddleTargetPos;
        } else {
            _rAIPlayerTargetY = paddleTargetPos;
        }
    }

    // Get paddle target position and actual setpoint
    float targetY, yActualSetpoint;
    IMotorHoming* playerMotor;
    if (player == GamePlayer::L) {
        targetY = _lAIPlayerTargetY;
        yActualSetpoint = _lAIPlayerActualYSetpoint;
        playerMotor = &(_lMotor);
    } else {
        targetY = _rAIPlayerTargetY;
        yActualSetpoint = _rAIPlayerActualYSetpoint;
        playerMotor = &(_rMotor);
    }

    if (yActualSetpoint == targetY) {
        // Already at target
        return;
    }

    // Move the paddle
    if (targetY > yActualSetpoint) {
        // Need to go down
        yActualSetpoint += _AIPlayerMaxMoveStep;
        if (yActualSetpoint > targetY) {
            yActualSetpoint = targetY;
        }
    } else {
        // Need to go up
        yActualSetpoint -= _AIPlayerMaxMoveStep;
        if (yActualSetpoint < targetY) {
            yActualSetpoint = targetY;
        }
    }

    playerMotor->track_target(yActualSetpoint);

    // Save new AI player status
    if (player == GamePlayer::L) {
        _lAIPlayerActualYSetpoint = yActualSetpoint;
    } else {
        _rAIPlayerActualYSetpoint = yActualSetpoint;
    }
}

float Game::getYInversionOvershoot(float speed) const {
    /*
    float absSpeed = fabs(speed);
    float result;

    float thresholds[] = Y_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    for (int i = 0; i < Y_AXIS_BOUNCE_INVERSIONS_COUNT; i++) {
        if (absSpeed < thresholds[i]) {
            result = _settings.yAxis.bounceInversionOvershootAtSpeed[i];
        }
    }

    result = _settings.yAxis.bounceInversionOvershootAtSpeed[Y_AXIS_BOUNCE_INVERSIONS_COUNT - 1];
    return result * 0.75f;
    */
   return 0.5f;
}

float Game::getXInversionOvershoot(float speed) const {
    /*
    float absSpeed = fabs(speed);

    float thresholds[] = X_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    for (int i = 0; i < X_AXIS_BOUNCE_INVERSIONS_COUNT; i++) {
        if (absSpeed < thresholds[i]) {
            return _settings.xAxis.bounceInversionOvershootAtSpeed[i];
        }
    }

    return _settings.xAxis.bounceInversionOvershootAtSpeed[X_AXIS_BOUNCE_INVERSIONS_COUNT - 1];
    */
    return 0.0f;
}

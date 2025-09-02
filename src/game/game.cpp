#include "game/game.hpp"

pbio_error_t Game::run(GamePlayer startPlayer) {
    CancelToken cancelToken;

    // Init game
    _scoreL = 0;
    _scoreR = 0;
    _ioBoard.showScore(0, 0);
    GamePlayer servingPlayer = startPlayer;

    // Game loop
    while (true) {
        // Move the ball in front of the serving player paddle
        PBIO_RETURN_ON_ERROR(moveBallToPaddle(servingPlayer, cancelToken));

        // Ensure that serving player is not pushing the encoder
        EncoderMultiJog* servingEncoder = (servingPlayer == GamePlayer::L) ? &_lEncoderJog : &_rEncoderJog;
        while (servingEncoder->getEncoder()->isButtonPressed())
        {
            IF_CANCELLED(cancelToken, {
                return PBIO_ERROR_CANCELED;
            });
            delay(100);
        }
        
        Button encoderButton;

        // Wait for the serving player to throw the ball
        _lEncoderJog.start(Axes::L);
        _rEncoderJog.start(Axes::R);
        while (!encoderButton.wasPressed())
        {
            IF_CANCELLED(cancelToken, {
                return PBIO_ERROR_CANCELED;
            });

            // Both players move the paddles with their encoders
            _lEncoderJog.update();
            _rEncoderJog.update();

            // Ball track the serving player paddle
            ballTrackPaddle(*(servingEncoder->getMotor()));

            delay(PBIO_CONFIG_SERVO_PERIOD_MS);
        }

        // Throw the ball
        throwBall(servingPlayer);

        // Bounce the ball until a player scores
        while (true) {
            IF_CANCELLED(cancelToken, {
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

            // Jog the paddles
            _lEncoderJog.update();
            _rEncoderJog.update();

            // Bounce the ball on top or bottom border
            bounceBallTopBottom();

            // Bounce the ball on paddle or score
            if (isBallAtPaddleBounceLimit()) {
                // The ball has reached the paddle on X-axis. Now test if it is within the paddle's Y-axis range or not
                // If it is, the ball bounce, otherwise the player misses (other player score)
                if (!bounceOnPaddle()) {
                    // The ball wasn't in front of the paddle, player misses
                    GamePlayer scoringPlayer = _speedX > 0 ? GamePlayer::L : GamePlayer::R;
                    if (scoringPlayer == GamePlayer::L) {
                        servingPlayer = GamePlayer::R;
                        // Block right paddle
                        _rEncoderJog.stop();
                        _rMotor.track_target(_paddleR);
                        // Move ball past the right paddle
                        moveBallCloseLoop(_xMotor.getSwLimitPlus(), _ballY, cancelToken);
                        // Increment the score
                        _scoreL++;
                    } else {
                        servingPlayer = GamePlayer::L;
                        // Block left paddle
                        _lEncoderJog.stop();
                        _lMotor.track_target(_paddleL);
                        // Move ball past the left paddle
                        moveBallCloseLoop(_xMotor.getSwLimitMinus(), _ballY, cancelToken);
                        // Increment the score
                        _scoreR++;
                    }

                    // Exit the bounce loop
                    break;
                }                
            }

            delay(PBIO_CONFIG_SERVO_PERIOD_MS);
        }

        // Test for game end
        if (_scoreL > GAME_WIN_SCORE || _scoreR > GAME_WIN_SCORE)
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

pbio_error_t Game::moveBallToPaddle(GamePlayer player, CancelToken& cancelToken) {
    float ballX, ballY;
    if (player == GamePlayer::L) {
        float xSwLimitM = _xMotor.getSwLimitMinus();
        ballX = _lMotor.angle() + GAME_PADDLE_H / 2.0 - GAME_BALL_L / 2.0;
        ballY = xSwLimitM + GAME_PADDLE_W + _settings.xAxis.ballPaddleDistance;
    } else {
        float xSwLimitP = _xMotor.getSwLimitPlus();
        ballX = _rMotor.angle() + GAME_PADDLE_H / 2.0 - GAME_BALL_L / 2.0;
        ballY = xSwLimitP - GAME_PADDLE_W - GAME_BALL_L - _settings.xAxis.ballPaddleDistance;
    }

    if (isBallOnThePaddleColumn(player)) {
        // Make the C movement to return the ball in front of the player's paddle
        float x1;
        if (player == GamePlayer::L) {
            float xSwLimitM = _xMotor.getSwLimitMinus();
            x1 = xSwLimitM + GAME_PADDLE_W + GAME_PADDLE_BALL_X_DIST_COLUMN;
        } else {
            float xSwLimitP = _xMotor.getSwLimitPlus();
            x1 = xSwLimitP - - GAME_PADDLE_W - GAME_BALL_L - GAME_PADDLE_BALL_X_DIST_COLUMN;
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
        GamePlayer otherPlayer = OTHER_GAME_PLAYER(player);
        if (isBallOnThePaddleColumn(otherPlayer)) {
            // Move the ball outside the other paddle column
            float x;
            if (otherPlayer == GamePlayer::L) {
                x = _xMotor.getSwLimitMinus() + GAME_PADDLE_W + GAME_PADDLE_BALL_X_DIST_COLUMN;
            } else {
                x = _xMotor.getSwLimitPlus() - GAME_PADDLE_W - GAME_BALL_L - GAME_PADDLE_BALL_X_DIST_COLUMN;
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

bool Game::isBallOnThePaddleColumn(GamePlayer player) const {
    if (player == GamePlayer::L) {
        return _xMotor.angle() < (_xMotor.getSwLimitMinus() + GAME_PADDLE_W + GAME_PADDLE_BALL_X_DIST_COLUMN);
    } else {
        return _xMotor.angle() > (_xMotor.getSwLimitPlus() - GAME_PADDLE_W - GAME_BALL_L - GAME_PADDLE_BALL_X_DIST_COLUMN);
    }
}

pbio_error_t Game::moveBallCloseLoop(float x, float y, CancelToken& cancelToken) {
    float deltaX = x - _xMotor.angle();
    float deltaY = y - _yMotor.angle();
    float absDeltaX = fabs(deltaX);
    float absDeltaY = fabs(deltaY);

    float maxSpeedX = _xMotor.get_speed_limit();
    float maxSpeedY = _yMotor.get_speed_limit();

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

void Game::throwBall(GamePlayer player) {
    _speedX = ((float)_settings.xAxis.startBallGameSpeed) * ((player == GamePlayer::L) ? 1.0f : -1.0f);
    _speedY = ((float)random(-10000, 10001) / 10000.0f) * (_settings.yAxis.bounceSpeedMax - _settings.yAxis.bounceSpeedMin);
    _speedY += (float)(_speedY > 0.0f ? _settings.yAxis.bounceSpeedMin : -_settings.yAxis.bounceSpeedMin);
    _overshootX = getXInversionOvershoot(_speedX);
    _overshootY = getYInversionOvershoot(_speedY);
    _ioBoard.playSound(IO_BOARD_SOUND_PADDLE);
}

void Game::bounceBallTopBottom() {
    bool invert;
    if (_speedY > 0.0f) {
        // Going down
        float border = _yMotor.getSwLimitPlus();

        invert = (_ballY + _overshootY) > border;
    } else {
        // Going up
        float border = _yMotor.getSwLimitMinus();

        invert = (_ballY - _overshootY) < border;
    }

    if (invert) {
        _speedY = -(_speedY);
        _yMotor.dc(_speedY);
        _ioBoard.playSound(IO_BOARD_SOUND_WALL);
    }
}

bool Game::isBallAtPaddleBounceLimit() const {
    if (_speedX > 0.0f) {
        // Going right
        float border = _xMotor.getSwLimitPlus() - _settings.xAxis.paddleCollisionTolerance - GAME_PADDLE_W;

        return (_ballX + _overshootX) > border;
    } else {
        // Going left
        float border = _xMotor.getSwLimitMinus() - _settings.xAxis.paddleCollisionTolerance + GAME_PADDLE_W;

        return (_ballX - _overshootX) < border;
    }
}

bool Game::bounceOnPaddle() {
    float paddleY = (_speedX > 0.0f) ? _paddleR : _paddleL;

    float yMin = paddleY - _settings.yAxis.paddleCollisionTolerance - GAME_BALL_L;
    float yMax = paddleY + _settings.yAxis.paddleCollisionTolerance + GAME_PADDLE_H;
        
    bool inRange = (_ballY >= yMin && _ballY <= yMax);
    if (inRange) {
        // The ball is in front of the paddle, bounce
        _speedX = -(_speedX + _settings.xAxis.ballBounceSpeedIncrement * (_speedX > 0.0f ? 1.0f : -1.0f));
        if (_speedX > 100.0f) _speedX = 100.0f;
        if (_speedX < -100.0f) _speedX = -100.0f;
        _xMotor.dc(_speedX);
        _overshootX = getXInversionOvershoot(_speedX);

        float relativePosition = 2.0f * (_ballY - yMin) / (yMax - yMin) - 1.0f;        
        _speedY = relativePosition * (_settings.yAxis.bounceSpeedMax - _settings.yAxis.bounceSpeedMin);
        _speedY += (float)(_speedY > 0.0f ? _settings.yAxis.bounceSpeedMin : -_settings.yAxis.bounceSpeedMin);
        _yMotor.dc(_speedY);
        _overshootY = getYInversionOvershoot(_speedY);

        _ioBoard.playSound(IO_BOARD_SOUND_PADDLE);
    }

    return inRange;
}

float Game::getYInversionOvershoot(float speed) const {
    float absSpeed = fabs(speed);

    float thresholds[] = Y_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    for (int i = 0; i < Y_AXIS_BOUNCE_INVERSIONS_COUNT; i++) {
        if (absSpeed < thresholds[i]) {
            return _settings.yAxis.bounceInversionOvershootAtSpeed[i];
        }
    }

    return _settings.yAxis.bounceInversionOvershootAtSpeed[Y_AXIS_BOUNCE_INVERSIONS_COUNT - 1];
}

float Game::getXInversionOvershoot(float speed) const {
    float absSpeed = fabs(speed);

    float thresholds[] = X_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    for (int i = 0; i < X_AXIS_BOUNCE_INVERSIONS_COUNT; i++) {
        if (absSpeed < thresholds[i]) {
            return _settings.xAxis.bounceInversionOvershootAtSpeed[i];
        }
    }

    return _settings.xAxis.bounceInversionOvershootAtSpeed[X_AXIS_BOUNCE_INVERSIONS_COUNT - 1];
}
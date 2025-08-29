#include "game/game.hpp"

pbio_error_t Game::run(GamePlayer startPlayer) {
    CancelToken cancelToken;

    // Init game
    _scoreL = 0;
    _scoreR = 0;
    _ioBoard.showScore(0, 0);
    GamePlayer servingPlayer = startPlayer;

    // Game loop
    while (_scoreL < 3 && _scoreR < 3) {
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

    }

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
        PBIO_RETURN_ON_ERROR(moveBall(ballX, ballY, cancelToken));
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

pbio_error_t Game::moveBall(float x, float y, CancelToken& cancelToken) {
    return PBIO_ERROR_NOT_IMPLEMENTED;
}
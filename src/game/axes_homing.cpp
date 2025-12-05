#include "game/axes_homing.hpp"
#include "devices/button.hpp"

pbio_error_t homeAllAxes(
    IMotorHoming& xMotor,
    IMotorHoming& yMotor,
    IMotorHoming& lMotor,
    IMotorHoming& rMotor,
    IOBoard& ioBoard,
    EncoderMultiJog& lEncoderJog,
    EncoderMultiJog& rEncoderJog,
    CancelToken& cancelToken) {

    Button startButton;

    IMotorHoming* axesSequence[] = { &yMotor, &lMotor, &rMotor, &xMotor };
    uint16_t axesSequenceCount = sizeof(axesSequence) / sizeof(axesSequence[0]);

    bool done = false;

    while (!done) {
        ioBoard.showScrollingText("Press Start to perform axes homing", SCROLLING_TEXT_ANIM_DELAY, true);
        lEncoderJog.start(Axes::X);
        rEncoderJog.start(Axes::Y);

        // Wait for the user to press the start button
        // and meanwhile allow the user to jog the axes
        while (!startButton.wasClicked() || STOP_BUTTON_PRESSED) {
            startButton.setRawState(millis(), START_BUTTON_PRESSED);

            IF_CANCELLED(cancelToken, {
                // If the homing is canceled, stop the motor
                Logger::instance().logI("Home all canceled by the user");

                lEncoderJog.stop();
                rEncoderJog.stop();
                return PBIO_ERROR_CANCELED;
            });

            // Update the encoder joggers to allow user to move the axes
            lEncoderJog.update();
            rEncoderJog.update();

            delay(3 * PBIO_CONFIG_SERVO_PERIOD_MS);
        }

        lEncoderJog.stop();
        rEncoderJog.stop();

        // Start axis homing one a the time
        for (uint16_t i = 0; i < axesSequenceCount; i++) {
            IMotorHoming* axis = axesSequence[i];
            if (!axis->referenced()) {
                ioBoard.showScrollingText("Homing " + String(axis->name()) + "-axis", SCROLLING_TEXT_ANIM_DELAY, false);
                pbio_error_t err = axis->run_axis_homing(cancelToken);
                if (err != PBIO_SUCCESS) {
                    if (err == PBIO_ERROR_CANCELED) {
                        // User has stopped the homing process. Prepare to restart
                        break;
                    }

                    // Axis homing failed, abort the procedure
                    Logger::instance().logE(String(axis->name()) + " axis homing failed. Reasong: " + pbio_error_str(err));
                    return err;
                }
            }
        }

        // Set done only if all axes are referenced
        done = true;
        for (uint16_t i = 0; i < axesSequenceCount; i++) {
            if (!axesSequence[i]->referenced()) {
                done = false;
                break;
            }
        }
    }

    return PBIO_SUCCESS;
}
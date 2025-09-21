#pragma once

#define START_BUTTON_PIN        43 // Grove port 1
#define STOP_BUTTON_PIN         44 // Grove port 1

#define START_BUTTON_PRESSED    (digitalRead(START_BUTTON_PIN) == LOW)
#define STOP_BUTTON_PRESSED     (digitalRead(STOP_BUTTON_PIN) == LOW)
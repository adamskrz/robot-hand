#include <Arduino.h>

const int leftButtonPin = 11; // Trigger Pin of Ultrasonic Sensor
const int rightButtonPin = 12; // Echo Pin of Ultrasonic Sensor

void servo_setup() {
    pinMode(leftButtonPin, INPUT_PULLUP);
    pinMode(rightButtonPin, INPUT_PULLUP);
}

void getButtonStatus(char text[64]) {
    if (digitalRead(leftButtonPin) == LOW) {
        strcpy(text, "Left Button Pressed");
    } else if (digitalRead(rightButtonPin) == LOW) {
        strcpy(text, "Right Button Pressed");
    } else {
        strcpy(text, "No Button Pressed");
    }
}
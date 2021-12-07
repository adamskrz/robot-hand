#include <Arduino.h>
#include <Servo.h>

const int leftButtonPin = 11; // Trigger Pin of Ultrasonic Sensor
const int rightButtonPin = 12; // Echo Pin of Ultrasonic Sensor
const int servoPin = 5; // Servo Pin
Servo servo; // Servo Object
int pos = 90;

void servo_setup() {
    pinMode(leftButtonPin, INPUT_PULLUP);
    pinMode(rightButtonPin, INPUT_PULLUP);
    servo.attach(servoPin);
    servo.write(pos);
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

void rotateServo(){
    // Check the button status, update status and rotate servo accordingly
    if (digitalRead(leftButtonPin) == LOW) {
        // Serial.println("Left Button Pressed");
        pos += 1;
        if (pos > 180) {
            pos = 180;
        }
    } else if (digitalRead(rightButtonPin) == LOW) {
        // Serial.println("Right Button Pressed");
        pos -= 1;
        if (pos < 0) {
            pos = 0;
        }
    }
    servo.write(pos);
}
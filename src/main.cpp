#include "display_loop.h"
#include "distance_loop.h"
#include "gripper_motion.h"
#include <Arduino.h>
#include <Ultrasonic.h>

const int pingPin = 3; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 2; // Echo Pin of Ultrasonic Sensor
unsigned long position_countdown_start = 0;
unsigned long countdown_val;
unsigned long gripCloseStart;
unsigned long calibratedClosingTime;
unsigned long currentClosingTime;
bool gripper_open = true;
bool gripper_closing = false;
double gripWidth;
int distance_cm;
char displayBuffer[64];
char floatBuffer[5];

// Initializing Ultrasonic Sensor library with pins and maximum 89cm distance timeout
Ultrasonic ultrasonic(pingPin, echoPin, 5000UL);

void setup()
{
    Serial.begin(9600); // Starting Serial Terminal
    gripper_setup();
    calibratedClosingTime = calibrate_gripper();
    Serial.print("closing time: ");
    Serial.println(calibratedClosingTime);
}

void loop()
{
    // Serial.println("Start of loop");
    if (gripper_open) {
        distance_cm = measureDistance(&ultrasonic, 100);

        if (distance_cm > 70) {
            // if over 70, display "Object out of range - please move closer"
            position_countdown_start = 0;
            snprintf(displayBuffer, sizeof(displayBuffer), "Object out of range - please move closer");
            displayText(displayBuffer, 200);
        } else if (distance_cm > 20) {
            // if over 20, display "xcm - place object in gripper"
            position_countdown_start = 0;

            snprintf(displayBuffer, sizeof(displayBuffer), "%dcm - place object in gripper", distance_cm);
            displayText(displayBuffer, 200);
        } else {

            // start timer
            countdown_val = millis() - position_countdown_start;
            // snprintf(displayBuffer, sizeof(displayBuffer), "Current timer - %lu", countdown_val);
            // displayText(displayBuffer, 200);

            if (position_countdown_start == 0) {
                position_countdown_start = millis();
            } else if (countdown_val < 1500) {
                // display "Object in gripper, please hold still - xs
                float displayTime = (1500 - countdown_val) / 1000.0;
                strcpy(displayBuffer, "Object in gripper, please hold still - ");
                dtostrf(displayTime, 2, 1, floatBuffer);
                strcat(displayBuffer, floatBuffer);
                strcat(displayBuffer, " sec");
                displayText(displayBuffer, 100);

            } else {
                // display "Gripping"
                snprintf(displayBuffer, sizeof(displayBuffer), "Gripping");
                displayText(displayBuffer, 200);

                gripper_open = false;
                gripCloseStart = millis();
                closeGripper();
                gripper_closing = true;
            }
        }
    } else if (gripper_closing) {
        currentClosingTime = millis() - gripCloseStart;
        gripWidth = gripper_width(currentClosingTime, calibratedClosingTime);
        dtostrf(gripWidth, 4, 2, floatBuffer);

        if (!gripperHolding() && currentClosingTime < (calibratedClosingTime + 500)) {
            
            strcpy(displayBuffer, "Closing grip - ");
            strcat(displayBuffer, floatBuffer);
            strcat(displayBuffer, "mm");

            displayText(displayBuffer, 200);

        } else {
            // Display "Gripper closed"
            // print grip width
            stopGripper();
            gripper_closing = false;
            strcpy(displayBuffer, "Gripper holding - ");
            strcat(displayBuffer, floatBuffer);
            strcat(displayBuffer, "mm");
        }
    } else {
        // Display "Gripper holding"
        displayText(displayBuffer, 200);
    }
}
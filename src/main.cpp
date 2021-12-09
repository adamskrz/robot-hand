
#include "display_loop.h"
#include "distance_loop.h"
#include "gripper_motion.h"
#include "servo_helper.h"
#include <Arduino.h>
#include <Ultrasonic.h>
#include <InputDebounce.h>

#define BUTTON_DEBOUNCE_DELAY 20 // [ms]

const int trigPin = 3; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 4; // Echo Pin of Ultrasonic Sensor
const int disablePin = 2; // Disable Pin for gripper function

static InputDebounce button;

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
bool gripper_disabled = false;

const int display_delay = 200;

// Initializing Ultrasonic Sensor library with pins and maximum 89cm distance timeout
Ultrasonic ultrasonic(trigPin, echoPin, 5000UL);

void voidfunction(uint8_t pinIn) { }

void switchDisable(uint8_t pinIn)
{
    Serial.println("Interrupt called");
    gripper_disabled = !gripper_disabled;
    position_countdown_start = 0;
}

void setup()
{
    Serial.begin(9600); // Starting Serial Terminal
    pinMode(disablePin, INPUT_PULLUP);
    button.registerCallbacks(switchDisable, voidfunction);
    button.setup(disablePin, BUTTON_DEBOUNCE_DELAY, InputDebounce::PIM_INT_PULL_UP_RES);
    // display_setup();
    servo_setup();
    gripper_setup();
    Serial.print("calibrating: ");
    calibratedClosingTime = calibrate_gripper();
    Serial.print("closing time: ");
    Serial.println(calibratedClosingTime);
    
}

void loop()
{
    // Serial.println("Start of loop");
    // getButtonStatus(displayBuffer);
    // displayText(displayBuffer, 200);

    // Check if disable button is pressed
    button.process(millis());
    

    if (gripper_disabled) {

        // Deal with interrupt - open gripper if closed/closing
        snprintf(displayBuffer, sizeof(displayBuffer), "GRIPPER DISABLED");
        displayText(displayBuffer, display_delay);

        if (!gripper_open) {
            openGripper();
            gripper_open = true;
            gripper_closing = false;
        }

    } else if (gripper_open) {

        // If not gripping, check the distance from object
        distance_cm = measureDistance(&ultrasonic, 100);

        if (distance_cm > 20) {

            // if over 20, reset countdown
            position_countdown_start = 0;
            
            rotateServo();

            if (distance_cm > 70) {
                // if over 70, print "object out of range"
                snprintf(displayBuffer, sizeof(displayBuffer), "Object out of range - please move closer");
            } else {
                // if under 70, print distance
                snprintf(displayBuffer, sizeof(displayBuffer), "%dcm - place object in gripper", distance_cm);
            }

            displayText(displayBuffer, display_delay);
            
        } else {

            // If object in gripping area, start countdown
            // start timer
            countdown_val = millis() - position_countdown_start;

            snprintf(displayBuffer, sizeof(displayBuffer), "Current timer - %lu", countdown_val);
            displayText(displayBuffer, 200);

            if (position_countdown_start == 0) {
                position_countdown_start = millis();
            } else if (countdown_val < 1500) {
                // display "Object in gripper, please hold still - xs
                float displayTime = (1500 - countdown_val) / 1000.0;
                strcpy(displayBuffer, "Object in gripper, please hold still - ");
                dtostrf(displayTime, 2, 1, floatBuffer);
                strcat(displayBuffer, floatBuffer);
                strcat(displayBuffer, " sec");
                displayText(displayBuffer, display_delay);

            } else {
                // if countdown is over, start closing
                // display "Gripping"
                snprintf(displayBuffer, sizeof(displayBuffer), "Gripping");
                displayText(displayBuffer, display_delay);

                gripper_open = false;
                gripCloseStart = millis();
                closeGripper();
                gripper_closing = true;
            }
        }
    } else if (gripper_closing) {
        // check closing status
        currentClosingTime = millis() - gripCloseStart;
        gripWidth = gripper_width(currentClosingTime, calibratedClosingTime);
        dtostrf(gripWidth, 4, 2, floatBuffer);

        if (!gripperHolding() && currentClosingTime < (calibratedClosingTime + 500)) {

            strcpy(displayBuffer, "Closing grip - ");
            strcat(displayBuffer, floatBuffer);
            strcat(displayBuffer, "mm");

            displayText(displayBuffer, display_delay);

        } else {
            // if closed, display "Gripper closed"
            // print grip width
            stopGripper();
            gripper_closing = false;
            strcpy(displayBuffer, "Gripper holding - ");
            strcat(displayBuffer, floatBuffer);
            strcat(displayBuffer, "mm");
        }
    } else {
        // Display "Gripper holding"
        // strcpy(displayBuffer, "Not disabled, open, or closing - probably holding");
        displayText(displayBuffer, display_delay);
    }

    delay(1);
}
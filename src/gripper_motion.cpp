#include "gripper_motion.h"
#include <Arduino.h>
#include <math.h>
#include "display_loop.h"

#define OPEN false
#define CLOSE true

const int pwm = 6; // digital pin 9 will be used for PWM (Pulse Width Modulation) output
const int dir = 7; // digital pin 8 will be used for high/low output
const int forcePin = A0; // analogue pin A0 will be used to read the voltage from the force sensor
const int ledPin = 13; // digital pin 13 is also wired to a built-in LED
int forceValue = 0; // value read from force pin
uint8_t closingSpeed = 50; // 8 bit unsigned integer (0-255) defining motor speed
bool grip_direction = OPEN; // boolean tracking direction
// unsigned long closingTime; // time it takes to close the gripper
char gripperDisplayBuffer[64];

// Setup function (runs when Arduino is powered on or reset)
void gripper_setup()
{
    pinMode(pwm, OUTPUT); //we have to set PWM pin as output
    pinMode(dir, OUTPUT); // Direction pin pins are also set as output
    pinMode(ledPin, OUTPUT); // LED pin is an output
    digitalWrite(dir, grip_direction);
    analogWrite(pwm, 0);
}

int getForce()
{
    return analogRead(forcePin);
}

bool gripperClosed()
{
    return getForce() > 855;
}

bool gripperHolding()
{
    return getForce() > 830;
}

void setGripperMotor(bool closing_dir, uint8_t speed = closingSpeed)
{
    if (closing_dir != grip_direction) {
        analogWrite(pwm, 0); // stop motor
        delay(50); // delay is in milliseconds
        digitalWrite(dir, closing_dir); // set direction
        grip_direction = closing_dir;
    }
    analogWrite(pwm, speed);
}

unsigned long calibrate_gripper()
{
    setGripperMotor(OPEN, 255);
    delay(1000);
    unsigned long startTime = millis();
    setGripperMotor(CLOSE, closingSpeed);
    while (!gripperClosed()) {
        delay(1);
        forceValue = getForce();
        if (forceValue < 25) {
            forceValue=0;
        }
        snprintf(gripperDisplayBuffer, sizeof(gripperDisplayBuffer), "Waiting for gripper to close - current force: %d", forceValue);
        displayText(gripperDisplayBuffer, 20);
    }
    unsigned long closingTime = millis() - startTime;
    setGripperMotor(OPEN, 255);
    delay(1000);
    setGripperMotor(CLOSE, 0);
    return closingTime;
}

void closeGripper()
{
    setGripperMotor(CLOSE, closingSpeed);
}

void openGripper()
{
    setGripperMotor(OPEN, 255);
    delay(1000);
    setGripperMotor(CLOSE, 0);
}

void stopGripper()
{
    setGripperMotor(CLOSE, 0);
}

double gripper_width(unsigned long currentTime, unsigned long fullTime)
{
    if (currentTime > fullTime) {
        return 0;
    }

    double amountClosed = currentTime / (float)fullTime;

    return -14.926 * pow(amountClosed, 2) - 50.014 * amountClosed + 64.224;
}
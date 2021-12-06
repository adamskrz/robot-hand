#include <Arduino.h>
#include <Ultrasonic.h>
#include "distance_loop.h"

unsigned long distanceStartMillis = millis();
unsigned long distanceCurrentMillis;
unsigned int current_distance_cm;

int measureDistance(Ultrasonic* ultrasonic, int delay)
{
    distanceCurrentMillis = millis();
    if (distanceCurrentMillis - distanceStartMillis >= delay) //test whether the period has elapsed
    {
        current_distance_cm = ultrasonic->read();
        distanceStartMillis = millis();
    }

    return current_distance_cm;
}
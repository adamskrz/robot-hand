#include "display_loop.h"
#include <Arduino.h>

unsigned long displayStartMillis = millis();
unsigned long currentMillis;
char prevtext[64];

void displayText(char text[64], int delay)
{
    currentMillis = millis();
    if ((currentMillis - displayStartMillis) >= delay) //test whether the period has elapsed
    {
        if (strcmp(prevtext, text) != 0) {
            // Serial.println("\n\n\n\n\n\n\n\n\n");
            Serial.println(text);
            strcpy(prevtext, text);
            displayStartMillis = millis();
        }
    }
}
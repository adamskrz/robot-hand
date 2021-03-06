#include "display_loop.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long displayStartMillis = millis();
unsigned long currentMillis;
char prevtext[64];

void display_setup()
{
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    display.display();
    delay(2000); // Pause for 2 seconds
    display.clearDisplay();
}

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
            
            // display.clearDisplay();
            // display.setTextSize(1); // Normal 1:1 pixel scale
            // display.setTextColor(SSD1306_WHITE); // Draw white text
            // display.setCursor(0, 0); // Start at top-left corner
            // display.println(text);
        }
    }
}
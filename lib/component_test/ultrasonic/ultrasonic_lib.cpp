#include <Arduino.h>
#include <Ultrasonic.h>

const int pingPin = 3; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 2; // Echo Pin of Ultrasonic Sensor

Ultrasonic ultrasonic(pingPin, echoPin, 40UL); // Initializing Ultrasonic Sensor

void setup() {
   Serial.begin(9600); // Starting Serial Terminal
   
}

void loop() {
   int distance_cm = ultrasonic.read(); // Reading Distance
   Serial.print(distance_cm);
   Serial.print("cm");
   Serial.println();
   delay(100); 
}
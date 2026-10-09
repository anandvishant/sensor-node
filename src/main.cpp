#include <Arduino.h>

const int LED_PIN = 2;      // built-in LED
const int BUTTON_PIN = 4;   // button between GPIO4 and GND

void setup() {
  Serial.begin(115200);               // start UART at 115200 baud
  pinMode(LED_PIN, OUTPUT);           // pin drives the LEDD
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // pin reads the button
  Serial.println("Booted!");
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {   // pressed = connected to GND
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Button pressed");
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  delay(50);
}
#include <Arduino.h>

const int LED_PIN = 2;
const int BUTTON_PIN = 4;
const uint32_t DEBOUNCE_MS = 50;
const uint32_t LOOP_DELAY_MS = 0;   // start at 1000, later change to 0

volatile uint32_t pressCount = 0;      // shared with the ISR
volatile uint32_t lastPressMs = 0;     // time of last accepted press
volatile bool pressedFlag = false;     // ISR sets, loop() clears
volatile uint32_t rawPresses = 0;

bool ledOn = false;

// ISR: runs the instant the button pin goes HIGH -> LOW
void IRAM_ATTR onButtonPress() {
  rawPresses++;                          // counts EVERY edge, even bounces
  uint32_t now = millis();
  if (now - lastPressMs > DEBOUNCE_MS) {
    pressCount++;                        // counts only accepted presses
    pressedFlag = true;
    lastPressMs = now;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonPress, FALLING);
  Serial.println("Booted! Interrupt armed.");
}
void loop() {
  static uint32_t lastPrint = 0;         // keeps its value between calls

  if (pressedFlag) {
    pressedFlag = false;
    ledOn = !ledOn;
    digitalWrite(LED_PIN, ledOn);
  }

  if (millis() - lastPrint >= 2000) {
    lastPrint = millis();
    Serial.print("accepted: ");
    Serial.print(pressCount);
    Serial.print("   raw edges: ");
    Serial.println(rawPresses);
  }
} 
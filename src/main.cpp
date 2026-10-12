#include <Arduino.h>
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);          // SDA = 21, SCL = 22
  Serial.println("I2C scanner");
}

void loop() {
  int found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);          // knock on this address
    if (Wire.endTransmission() == 0) {     // 0 means someone answered
      Serial.print("Found device at 0x");
      Serial.println(addr, HEX);
      found++;
    }
  }
  if (found == 0) Serial.println("No devices found");
  delay(3000);
}
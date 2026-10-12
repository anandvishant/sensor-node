#include <Arduino.h>
#include <Wire.h>

const uint8_t ADDR   = 0x76; // This is the addr where we 
const uint8_t REG_ID = 0xD0;

uint8_t readReg(uint8_t reg) {
  Wire.beginTransmission(ADDR);
  Wire.write(reg);                      // "I want register 0xD0"
  Wire.endTransmission(false);
  Wire.requestFrom(ADDR, (uint8_t)1);   // "send me 1 byte"
  return Wire.read();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
}

void loop() {
  Serial.print("Chip ID: 0x");
  Serial.println(readReg(REG_ID), HEX);
  delay(60000);
}
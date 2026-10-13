#include <Arduino.h>
#include <Wire.h>

const uint8_t ADDR   = 0x76;   // BME280 I2C address (SDO low)
const uint8_t REG_ID = 0xD0;   // chip ID register

// Returns true on success and stores the byte in `out`
bool readRegRetry(uint8_t reg, uint8_t &out, int tries = 3) {
  for (int i = 0; i < tries; i++) {
    Wire.beginTransmission(ADDR);
    Wire.write(reg);
    if (Wire.endTransmission(false) == 0 &&
        Wire.requestFrom(ADDR, (uint8_t)1) == 1) {
      out = Wire.read();
      return true;
    }
    delay(10);                 // brief pause, then try again
  }
  return false;                // gave up after `tries` attempts
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
}

void loop() {
  uint8_t id;
  if (readRegRetry(REG_ID, id)) {
    Serial.print("Chip ID: 0x");
    Serial.println(id, HEX);
  } else {
    Serial.println("Read failed after retries");
  }
  delay(1000);                 // keep the failing gap on purpose to test the fix
}
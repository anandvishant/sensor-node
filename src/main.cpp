#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

// One reading = three numbers packed together
struct Reading {
  float temperature;
  float humidity;
  float pressure;
};

QueueHandle_t readingQueue;   // the mailbox

// TASK 1: reads the sensor and puts the result in the mailbox
void sensorTask(void *param) {
  while (true) {
    Reading r;
    r.temperature = bme.readTemperature();
    r.humidity    = bme.readHumidity();
    r.pressure    = bme.readPressure() / 100.0F;

    xQueueSend(readingQueue, &r, 0);     // drop it in the mailbox

    vTaskDelay(pdMS_TO_TICKS(1000));     // sleep 1 s, other tasks can run
  }
}

// TASK 2: waits for mail, then prints it
void printTask(void *param) {
  while (true) {
    Reading r;
    if (xQueueReceive(readingQueue, &r, portMAX_DELAY)) {   // sleeps until mail arrives
      Serial.print("Temp: ");
      Serial.print(r.temperature);
      Serial.print(" C   Humidity: ");
      Serial.print(r.humidity);
      Serial.print(" %   Pressure: ");
      Serial.print(r.pressure);
      Serial.println(" hPa");
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  if (!bme.begin(0x76)) {
    Serial.println("BME280 not found");
    while (true) delay(1000);
  }

  readingQueue = xQueueCreate(5, sizeof(Reading));   // mailbox holds up to 5 readings

  xTaskCreate(sensorTask, "sensor", 4096, NULL, 1, NULL);
  xTaskCreate(printTask,  "print",  4096, NULL, 1, NULL);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));   // loop() is not used any more
}
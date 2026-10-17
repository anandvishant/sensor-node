#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

// One reading = three numbers packed together
struct Reading
{
  float temperature;
  float humidity;
  float pressure;
};

QueueHandle_t readingQueue; // the mailbox
enum State
{
  RUNNING,
  PAUSED
};
volatile State state = RUNNING;      // current state
volatile uint32_t intervalMs = 1000; // how often the sensor reads

const int BUTTON_PIN = 4; // <-- change to YOUR button pin
const uint32_t DEBOUNCE_MS = 50;

QueueHandle_t buttonQueue; // button events travel through this

// ISR: keep it tiny. Debounce, then send one event to the queue.
void IRAM_ATTR onButtonPress()
{
  static uint32_t lastMs = 0;
  uint32_t now = millis();
  if (now - lastMs < DEBOUNCE_MS)
    return; // ignore bounce
  lastMs = now;

  uint8_t event = 1;
  BaseType_t woken = pdFALSE;
  xQueueSendFromISR(buttonQueue, &event, &woken); // the ISR-safe version
  if (woken)
    portYIELD_FROM_ISR();
}

void sensorTask(void *param)
{
  while (true)
  {
    if (state == RUNNING)
    { // only read when running
      Reading r;
      r.temperature = bme.readTemperature();
      r.humidity = bme.readHumidity();
      r.pressure = bme.readPressure() / 100.0F;
      xQueueSend(readingQueue, &r, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(intervalMs)); // sleep for the current interval
  }
}
// TASK 2: waits for mail, then prints it
void printTask(void *param)
{
  while (true)
  {
    Reading r;
    if (xQueueReceive(readingQueue, &r, portMAX_DELAY))
    { // sleeps until mail arrives
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

void handleCommand(String line)
{
  line.trim(); // remove stray spaces

  if (line == "pause")
  {
    state = PAUSED;
    Serial.println("OK paused");
  }
  else if (line == "resume")
  {
    state = RUNNING;
    Serial.println("OK running");
  }
  else if (line == "status")
  {
    Serial.print("State: ");
    Serial.print(state == RUNNING ? "RUNNING" : "PAUSED");
    Serial.print("  interval: ");
    Serial.print(intervalMs);
    Serial.println(" ms");
  }
  else if (line.startsWith("interval "))
  {
    long v = line.substring(9).toInt(); // the part after "interval "
    if (v >= 100 && v <= 60000)
    {
      intervalMs = v;
      Serial.print("OK interval ");
      Serial.println(v);
    }
    else
    {
      Serial.println("ERR interval must be a number from 100 to 60000");
    }
  }
  else
  {
    Serial.print("ERR unknown command: ");
    Serial.println(line);
  }
}
// TASK 3: listens for typed characters and builds them into lines
void commandTask(void *param)
{
  String line = "";
  while (true)
  {
    while (Serial.available() > 0)
    {
      char c = Serial.read();
      if (c == '\n' || c == '\r')
      {
        if (line.length() > 0)
        {
          handleCommand(line);
          line = "";
        }
      }
      else
      {
        line += c;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// TASK 4: waits for button events and toggles the state
void buttonTask(void *param)
{
  uint8_t event;
  while (true)
  {
    if (xQueueReceive(buttonQueue, &event, portMAX_DELAY))
    { // sleeps until a press
      state = (state == RUNNING) ? PAUSED : RUNNING;
      Serial.println(state == RUNNING ? "BUTTON: running" : "BUTTON: paused");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(21, 22);

  if (!bme.begin(0x76))
  {
    Serial.println("BME280 not found");
    while (true)
      delay(1000);
  }

  readingQueue = xQueueCreate(5, sizeof(Reading)); // mailbox holds up to 5 readings

  xTaskCreate(sensorTask, "sensor", 4096, NULL, 1, NULL);
  xTaskCreate(printTask, "print", 4096, NULL, 1, NULL);
  xTaskCreate(commandTask, "command", 4096, NULL, 1, NULL);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  buttonQueue = xQueueCreate(5, sizeof(uint8_t));
  xTaskCreate(buttonTask, "button", 4096, NULL, 1, NULL);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonPress, FALLING);
}

void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000)); // loop() is not used any more
}
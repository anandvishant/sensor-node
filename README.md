# sensor-node
# Sensor Node

A small embedded project I built to learn embedded development from scratch. An ESP32 reads a BME280 temperature, humidity and pressure sensor over I2C, runs its work as FreeRTOS tasks connected by queues, and accepts commands over UART. A Python script on the PC logs the data and tests the commands.

## Hardware
- ESP32 dev board (ESP32-WROOM-32)
- BME280 sensor module (I2C address 0x76)
- Push button, breadboard and jumper wires

## Wiring
| Part | Connection |
|---|---|
| BME280 SDA | GPIO 21 |
| BME280 SCL | GPIO 22 |
| BME280 VCC / GND | 3.3 V / GND |
| Button | GPIO 4 and GND (internal pull-up, no resistor) |

## How it works
```
 button --(interrupt)--> [button queue] --> button task --\
                                                           +--> state: RUNNING / PAUSED
 UART commands --> command task -------------------------/
                                                           |
 sensor task (reads BME280 every N ms, if RUNNING) -> [reading queue] -> print task -> UART
```
- **sensor task** reads the BME280 and puts a reading in a queue.
- **print task** waits for readings and prints them.
- **command task** reads typed lines and changes the state or the interval.
- **button task** toggles RUNNING and PAUSED when the button interrupt sends an event.

## Serial commands (115200 baud)
| Command | Effect |
|---|---|
| `status` | prints the state and the reading interval |
| `pause` / `resume` | stop and start sensor reading |
| `interval N` | set the read interval in ms (100 to 60000) |

The button also toggles pause and resume.

## Build and run
1. Install VS Code with the PlatformIO extension.
2. Open the project, connect the ESP32, then click **Upload**.
3. Open the Serial Monitor at 115200 baud.

## PC tools (Python)
Install `pyserial` (`pip install pyserial`). Close the Serial Monitor first, because only one program can use the port. Edit `PORT` in the scripts if your port is not `COM3`.
- `python tools/logger.py` logs readings to `readings.csv` (stop with Ctrl+C).
- `python tools/send_commands.py` sends a test sequence of commands and prints the replies.

## What I learned
- TODO: 3 bullets in your own words (for example: interrupts and debounce, I2C, tasks and queues, state machine).

## Bugs I hit
- **Frozen sensor readings:** TODO in your words (the cause was a loose connection on the unsoldered pins).
- **I2C `Error -1` with repeated start:** TODO in your words.
- **Queue full when the printer was slower than the sensor:** TODO in your words.

## Notes
Day-by-day notes are in `NOTES.md`, and a quick reference is in `Cheatsheet.md`.
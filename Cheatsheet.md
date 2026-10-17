# Cheat sheet (Days 0-4)

## Arduino basics (ESP32)
| Item | What it does |
|---|---|
| `setup()` | runs once at start |
| `loop()` | runs forever after setup |
| `Serial.begin(115200)` | start the serial port; monitor_speed in platformio.ini must match |
| `Serial.print / println` | print text or numbers to the Serial Monitor |
| `delay(ms)` | wait (on ESP32 it calls vTaskDelay underneath) |
| `millis()` | milliseconds since boot; used for timing without blocking |

## GPIO (Day 0-1)
| Item | What it does |
|---|---|
| `pinMode(pin, INPUT_PULLUP)` | input with an internal resistor to 3.3 V; unpressed = HIGH, pressed = LOW |
| `pinMode(pin, OUTPUT)` | pin drives an LED etc. |
| `digitalRead(pin)` | read HIGH or LOW |
| `digitalWrite(pin, level)` | set a pin HIGH or LOW |
| Polling | loop keeps asking "has it changed?" and wastes CPU |

## Interrupts and debounce (Day 2)
| Item | What it does |
|---|---|
| `attachInterrupt(pin, isr, FALLING)` | run isr when the pin goes HIGH to LOW |
| ISR | keep it very short; no Serial.println inside (can deadlock) |
| `IRAM_ATTR` | on ESP32, puts the ISR in fast memory |
| `volatile` | tells the compiler the variable can change behind its back (ISR) |
| Flag pattern | ISR sets a flag, loop() sees it and does the slow work |
| Bounce | a button makes several fast on/off edges when pressed |
| Debounce | ignore edges within a time window (here 50 ms), using millis() |

## I2C (Day 3)
| Item | What it does |
|---|---|
| SDA, SCL | data line and clock line; shared by all devices; master sets the clock |
| Address | each device has one (BME280 = 0x76 with SDO low) |
| `Wire.begin(21, 22)` | start I2C on SDA = GPIO21, SCL = GPIO22 |
| `Wire.beginTransmission(addr)` | start talking to a device |
| `Wire.write(byte)` | send a byte (like a register address) |
| `Wire.endTransmission()` | finish; returns 0 if the device ACKed. Use the stop version (no `false`) |
| `Wire.requestFrom(addr, n)` | ask for n bytes; check it returned n |
| Scanner | try addresses 1-126; endTransmission() == 0 means a device answered |
| BME280 chip ID | register 0xD0 returns 0x60 (a BMP280 returns 0x58) |
| UART vs I2C | UART has no clock, so both sides agree on a baud rate; I2C has a clock line |

## BME280 library (Day 3)
| Item | What it does |
|---|---|
| `lib_deps = adafruit/Adafruit BME280 Library` | PlatformIO downloads the library |
| `bme.begin(0x76)` | init; returns false if the sensor isn't found |
| `readTemperature()` | degrees C |
| `readHumidity()` | % relative humidity |
| `readPressure()` | Pa (divide by 100 for hPa) |

## FreeRTOS (Day 4)
| Item | What it does |
|---|---|
| Task | an independent function with its own loop, scheduled by FreeRTOS |
| `xTaskCreate(fn, "name", stack, NULL, priority, NULL)` | start a task |
| `vTaskDelay(pdMS_TO_TICKS(ms))` | task sleeps; others keep running |
| `xQueueCreate(n, sizeof(item))` | a mailbox holding n items |
| `xQueueSend(q, &item, 0)` | put an item in; 0 = don't wait; returns pdTRUE (1) if it went in |
| `xQueueReceive(q, &item, portMAX_DELAY)` | wait forever for an item |
| Blocked state | a task waiting on a queue uses no CPU |
| FIFO | first in, first out; oldest comes out first |
| Queue full | producer faster than consumer; new items are dropped or the sender waits |

## UART commands and state machine (Day 5)
| Item | What it does |
|---|---|
| `Serial.available()` | how many typed characters are waiting |
| `Serial.read()` | take one character from the input |
| Line buffer | add characters to a String until Enter arrives, then treat it as one message |
| `line.trim()` | remove spaces and stray characters at the ends |
| `line == "pause"` | compare the whole text |
| `line.startsWith("interval ")` | check how a command begins |
| `line.substring(9)` | the part of the text after position 9 |
| `.toInt()` | turn text into a number; text like "abc" gives 0 |
| `enum State { RUNNING, PAUSED }` | a named list of states |
| State machine | always in one state; events (commands) move it between states |
| Shared variable between tasks | mark it `volatile`; fine for a single value, proper locking comes later |
| Input validation | never trust serial input; check ranges and reject bad values with an ERR message |
| `monitor_filters = send_on_enter` | PlatformIO monitor sends the whole line on Enter |
| `monitor_echo = yes` | monitor shows what you type |
| `monitor_eol = LF` | Enter sends a plain newline |
| Git order | create the branch first (`git switch -c`), then commit |
| `git branch -f main origin/main` | move local main back to match GitHub |

## Python on the PC (Day 6)
| Item | What it does |
|---|---|
| `python --version`, `pip install pyserial` | check Python; install a library (import it as `serial`) |
| `serial.Serial(PORT, BAUD, timeout=1)` | open the COM port; `timeout` stops readline() waiting forever |
| `ser.readline()` | read one line of bytes |
| `.decode("utf-8", errors="replace")` | bytes to text; `.encode()` goes the other way |
| `ser.write(...)` | send bytes to the ESP32 |
| `ser.reset_input_buffer()` | throw away old unread data |
| `re.compile(...)`, `match.groups()` | find numbers inside a line of text |
| `float(x)` | text to a decimal number |
| `csv.writer(f).writerow([...])` | write one row to a CSV file |
| `open(file, "a")` | append; `"w"` erases the file first |
| `with ... as ...:` | open something and close it automatically |
| `try: ... except KeyboardInterrupt:` | stop cleanly on Ctrl+C |
| One COM port, one program | the logger and the Serial Monitor can't run together |
| Opening the port | resets the ESP32, so wait about 2 s |


## Git (all days)
| Command | What it does |
|---|---|
| `git switch -c name` | new branch |
| `git add file` / `git commit -m "msg"` | stage and save a snapshot |
| `git push -u origin name` | first push of a branch |
| `git switch main` / `git pull` | back to main and update it |
| `git fetch --prune` | update remote info, remove deleted remote branches |
| `git branch -d name` | delete a merged local branch |
| `git log --oneline` | short history |
| `git diff` / `git diff --staged` | what changed / what's staged |
| `git restore file` | throw away uncommitted changes in a file |
| PR flow | branch, commit, push, open PR, read diff, merge, delete branch, sync |
| Rule | never commit directly to main |

## Debugging method
- Change one thing at a time.
- Hands off the hardware while testing.
- Check the physical connection before blaming the part.
- Narrow it down: scanner, then chip ID, then raw registers, then the library.
- Write bugs and fixes in NOTES.md.
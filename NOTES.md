# Bug journal

## Day1:
i connected and set the project.

Experiment 1:
connected and everything.Intially when running the serial monitor and testing with delay 50. it showed that it registered the button press 4 times in a single press.
After changing delay(2000) from 50. A single button press had to be pressed and held a bit long than 50 scenario. And blue light also lighted a bit late than usual(i.e 50 sec ddelay)

Experiment 2:
const int BUTTON_PIN = 5;   // button between GPIO4 and GND 
changed this from 4(which is the actual wire thats connected to button and D4 on the esp 32)
and when pressed the button nothing happened. this is because the wiring is different. to have something happe. i would have tio change the pin to 5. But there is no D5 on the espe board. So set it to relevant in code that matchers on board. then only something will happen.

Experiment 3:
commented baud rate   //Serial.begin(115200);  
and this chnaged something. when tested by pressing the button. While the blue light lighted on board. it didnt print anyything on serial monitor terminal. Like "Button pressed" . i Think this is due to UART utilizes baud rate  for  communication and transffer of that data?


## Day2
Experiment 1:
in loop delay at 1000ms. it counts button press all the time using ISR but doesnt print because there is a 1 sec(1000ms) delay in calling the loop()

Experiment 2:
when changing the loop delay to 0ms it counts all an prints all. becuse that allows loop() to run continuosly 

Experiment3:
couldnt understand the debounce concept fully. this is more likely about the metal contacts inside the button bouncing up and down and this is to ignore that fake contacts as button press counts
Setting	What you did	Result
DEBOUNCE_MS = 50	20 presses	accepted 20, raw 26
DEBOUNCE_MS = 0	slam	accepted 38, raw 38 (no bounce)
DEBOUNCE_MS = 0	slow press	accepted 22, raw 28
DEBOUNCE_MS = 150	10 slow presses	accepted 12
DEBOUNCE_MS = 50	20 slow presses	accepted 20, raw 27, one press miscounted

1. Interrupt vs polling: correct. The interrupt pauses whatever the chip is doing and records the press right away, so a tap during delay() isn’t lost. One refinement: the polling code does see the button, just too rarely, so a short tap falls between checks. The interrupt doesn’t depend on when the code looks.

2. Serial.println in an ISR: this is the weak one. “ISR only handles interrupts” restates the rule without giving the reason. The reason is:

While the ISR runs, everything else on the chip is paused, so it has to finish in microseconds.
Serial.println is slow. It formats text and pushes bytes through a buffer that normal code may be using at that very moment.
That buffer is protected by locks, and waiting on a lock inside an ISR can freeze or crash the chip. On the ESP32 you’d often get a reboot with a “Guru Meditation Error”.

So the ISR just notes what happened (the counter and the flag), and loop() does the slow printing. Rewrite your answer with that in mind, for example: “Printing is slow and uses locks and buffers, and an ISR must be short, so it sets a flag and loop() prints.”

3. volatile: correct. A tighter version: it tells the compiler this variable can change outside the normal flow of the code, such as inside an ISR, so it must re-read the real value every time and not reuse an old copy.

4. Debouncing: correct. One accuracy fix: the 50 ms isn’t a delay the chip waits through. It’s a window. The chi


## Day 3: I2C sensor (BME280)

**I2C:** two shared wires, SDA (data) and SCL (clock). Each device has an
address, and the ESP32 talks to it by address.

**Scanner:** the for loop tries addresses 1 to 126. If a device answers,
endTransmission() returns 0, so it prints the address and adds 1 to found.
The loop does not stop at the first hit, so it lists every device. found is
declared inside loop(), so it resets to 0 on every scan. If found is still 0
after the loop, it prints "No devices found". Result: found 0x76.

**Chip ID:** register 0xD0 returned 0x60, so the sensor is a real BME280
(a BMP280 would return 0x58).

**Experiments**
| Test | What happened | Why |
|---|---|---|
| Pulled SDA | read returned 0xFF, Error -1 | nobody answered |
| ADDR = 0x77 | same 0xFF, Error -1 | sensor is at 0x76, nothing answers 0x77 |

Both gave the same error, so the error alone can't tell a wiring fault from a
wrong address. The scanner can: it still finds 0x76 if only the address in
the code was wrong.

**Bug: first read after a long gap fails**
- Gaps of 1000 to 5000 ms always worked. 5500, 6000 and 8000 ms failed on the
  first read (0xFF, Error -1).
- A retry 50 ms later worked every time.


**Final code: readings with the Adafruit library**
- Added `lib_deps = adafruit/Adafruit BME280 Library` to platformio.ini, so PlatformIO downloads the library.
- `bme.begin(0x76)` sets up the sensor and checks that it answers. If it doesn't, the code prints "BME280 not found" and stops.
- `bme.readTemperature()`, `bme.readHumidity()` and `bme.readPressure()` return real units (C, % and Pa). I divide the pressure by 100 to print hPa.
- The library does the register reads and the calibration math that I did by hand earlier. Doing it by hand first showed me what the library is doing.
- The frozen values (23.44 C, 77.96 %, 671.33 hPa) were the library converting a placeholder raw value (0x800000), meaning no measurement had happened.
this froze becayuse the conncetion awwsnt correct or something. earlier it was hanging rather than placed ibn breadboard. after they are postiionedd in bread board it startedd to ddisplay accurate measurments. like below
- Result: about 29.6 C, 41 % RH and 984 hPa, against a wall thermometer reading 27 C and 48 %. The difference is plausible, since the sensor warms slightly and the wall thermometer sits elsewhere.

What did the I2C scanner do, and what did it find?
The code was used to scan the aaddress the device is at. meaning the hex addres of that sensor between 1 to 127 devices in for loop
Why did the first read fail after a long wait, and what did you do about it?
For some reason if read is after 5s(5000ms) it seems to fail and in order to check that's really the case. added a delay(50) and checked againt it suceeded. Later I found endTransmission(false) gave Error -1 on the ESP32, and endTransmission() fixed it. I haven't re-tested the 5 s gap since, so I'm not sure the delay was the real cause.
What was actually wrong when the readings froze, and how did you fix it? probably due to unstable way the senor was placed physcially and switching to bread board fixed it

What does the Adafruit library do for you that the hand-written code didn’t?
its has bme sensor reallted tfunctions to check sensor related readings
What would you check first next time a sensor doesn’t answer? Conncetions 

## Day 4
What is a task, and how is it different from the single loop() of Day 3?
Task is operation. In freeRTOS, this OS helps in running tasks simulataneouly in a safe way and in terms of loop in day 3 we are just grabbing temp as its going on and not using tasks or queues to manage. But usign rtos helps and is good for large scale measurements and tasks managing. might not be big deal in this small project

What does the queue do, and what happened when the printer was slower than the sensor?
Queue stores the readings. this is a data structure, its a way to store the data in FIFO process. And when printer was slower than sensor. we were getting measurements with delay and that delay might not cause us to see latest reading rather it prints old reading in sequential way and this might result ot see the readings thats printed. but they are old 

What would you do about it in a real product?
in real product if there is delay i wouldd wnat to delete top of ques and print reading bottomn of que to represnet accurate info

Does a task waiting on the queue waste CPU?
No. A task waiting on the queue is in the Blocked state, so the FreeRTOS scheduler doesn't run it and it uses no CPU until a reading arrives. The scheduler decides which task gets the CPU and when.

vTaskDelay vs delay()?
On the ESP32, delay() already calls vTaskDelay(), so there's no difference on my board. vTaskDelay is the clearer, portable FreeRTOS choice, and it sleeps for a number of scheduler ticks. A real busy-wait would hog the CPU and starve other tasks.
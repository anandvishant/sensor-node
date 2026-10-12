# Bug journal

Day1:
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


Day2
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
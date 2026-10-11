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
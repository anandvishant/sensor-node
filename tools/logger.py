import serial
import time

PORT = "COM3"      # the port from your PlatformIO monitor
BAUD = 115200      # must match Serial.begin(115200) on the ESP32

with serial.Serial(PORT, BAUD, timeout=1) as ser:
    time.sleep(2)                      # the ESP32 restarts when the port opens; let it boot
    end = time.time() + 10             # run for 10 seconds
    while time.time() < end:
        raw = ser.readline()           # wait for one line, up to 1 second
        line = raw.decode("utf-8", errors="replace").strip()
        if line:                       # skip empty reads
            print(line)
import serial
import time

PORT = "COM3"
BAUD = 115200

def send(ser, command):
    ser.write((command + "\n").encode())          # text -> bytes, plus Enter
    deadline = time.time() + 2                    # wait up to 2 seconds for a reply
    while time.time() < deadline:
        line = ser.readline().decode("utf-8", errors="replace").strip()
        if line.startswith(("OK", "ERR", "State")):   # skip sensor lines, keep replies
            return line
    return None                                   # no reply arrived

with serial.Serial(PORT, BAUD, timeout=1) as ser:
    time.sleep(2)                                 # let the ESP32 finish booting
    ser.reset_input_buffer()                      # throw away boot messages and old lines

    commands = ["status", "pause", "status", "resume",
                "interval 500", "status", "interval abc", "foo"]
    for cmd in commands:
        print(f"> {cmd}")
        print("  ", send(ser, cmd))
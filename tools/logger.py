import csv
import os
import re
import serial
from datetime import datetime

PORT = "COM3"
BAUD = 115200
CSV_FILE = "readings.csv"

# Matches: Temp: 23.54 C   Humidity: 36.86 %   Pressure: 1000.81 hPa
PATTERN = re.compile(
    r"Temp:\s*([-\d.]+)\s*C\s+Humidity:\s*([-\d.]+)\s*%\s+Pressure:\s*([-\d.]+)\s*hPa"
)

new_file = not os.path.exists(CSV_FILE)

with serial.Serial(PORT, BAUD, timeout=1) as ser, open(CSV_FILE, "a", newline="") as f:
    writer = csv.writer(f)
    if new_file:
        writer.writerow(["time", "temp_c", "humidity_pct", "pressure_hpa"])

    print("Logging... press Ctrl+C to stop")
    try:
        while True:
            line = ser.readline().decode("utf-8", errors="replace").strip()
            match = PATTERN.match(line)
            if match:                                   # boot messages don't match, so they're skipped
                temp, hum, pres = (float(x) for x in match.groups())
                now = datetime.now().isoformat(timespec="seconds")
                writer.writerow([now, temp, hum, pres])
                f.flush()                               # write to disk right away
                print(now, temp, hum, pres)
    except KeyboardInterrupt:
        print("Stopped.")
import serial
import time

PORT = "COM12"      # your ESP32's port
BAUD = 115200
OUTFILE = "log.csv"

ser = serial.Serial(PORT, BAUD, timeout=2)
time.sleep(2)            # opening the port can restart the ESP32
ser.reset_input_buffer()

ser.write(b"d")          # send the dump command

capturing = False
lines = []

while True:
    line = ser.readline().decode(errors="ignore").strip()
    if "LOG START" in line:
        capturing = True
        continue
    if "LOG END" in line:
        break
    if capturing and line:
        lines.append(line)
    if not line and not capturing:
        break            # nothing came back, so stop

ser.close()

with open(OUTFILE, "w") as f:
    f.write("\n".join(lines) + "\n")

print(f"Saved {len(lines)} lines to {OUTFILE}")
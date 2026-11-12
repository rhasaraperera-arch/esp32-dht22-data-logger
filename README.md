# ESP32 Temperature & Humidity Data Logger

An ESP32-based logger that reads temperature and humidity, timestamps each reading over Wi-Fi (NTP), saves it to onboard flash, and exports it to CSV for analysis in Excel. Built as part of my self-training in embedded systems and automation.

## Features
- Reads temperature and humidity from an AM2302 (DHT22) sensor
- Real timestamps from an NTP server (UTC+5:30, Sri Lanka)
- Saves readings to the ESP32's flash with LittleFS, so data survives power-offs
- Serial commands: `d` dump log, `c` clear log, `s` show file size
- Python script that pulls the log from the board and saves it as `log.csv`

## Hardware
| Part | Connection |
|---|---|
| ESP32 dev board (CP2102, Type-C) | USB to PC |
| AM2302 / DHT22 sensor | VCC to 3V3, GND to GND, DATA to GPIO 4 |

## Software
- Arduino IDE 2.x with the ESP32 board package (Espressif)
- Library: DHT sensor library (Adafruit)
- Python 3 with `pyserial` (`pip install pyserial`)

## How to use
1. Open the sketch and enter your Wi-Fi name and password (2.4 GHz only).
2. Select board **ESP32 Dev Module** and upload.
3. Open the Serial Monitor at 115200 baud to watch live readings.
4. Close the Serial Monitor, then run `python get_log.py` to export the log to `log.csv`.

Change `LOG_INTERVAL` in the sketch to set how often readings are saved (default 10 s for testing, 60 s recommended for long runs).

## Problems I solved
- **Wi-Fi would not connect:** used a network scan to find that the real network name differed from the one in my code.
- **Compile error (variable scope):** a variable declared inside an `else` block was used outside it, so I moved it up.
- **Misplaced code broke `setup()`:** inserting a line between an `if` and its `{` caused an early return.
- **Python export returned 0 lines:** opening the serial port restarts the ESP32, so the script waits and retries.

## What I learned
Serial communication and baud rates, Wi-Fi and NTP time sync, flash file systems and wear, reading sensor data, and basic debugging with scans and status codes.

## Next steps
- Add a MAX6675 thermocouple (SPI) for high-temperature monitoring
- Web dashboard with wireless log download

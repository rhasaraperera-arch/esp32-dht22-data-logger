#include <DHT.h>
#include <WiFi.h>
#include <time.h>
#include <LittleFS.h>

#define DHTPIN 4
#define DHTTYPE DHT22


const char* ssid = "wifi name"; //WIFI_name
const char* password = "wifi password"; //pass

// Sri Lanka is UTC+5:30 = 19800 seconds
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;

const unsigned long LOG_INTERVAL = 60000; // ms between saves
unsigned long lastLog = 0;

DHT dht(DHTPIN, DHTTYPE);

void dumpLog() {
  File f = LittleFS.open("/log.csv", FILE_READ);
  if (!f) { Serial.println("No log file"); return; }
  Serial.println("--- LOG START ---");
  while (f.available()) Serial.write(f.read());
  Serial.println("--- LOG END ---");
  f.close();
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  if (!LittleFS.begin(true)) {   // true = format if first use
    Serial.println("LittleFS mount failed");
    return;
  }

  Serial.printf("Storage: %u used / %u total bytes\n",
                LittleFS.usedBytes(), LittleFS.totalBytes());
  if (!LittleFS.exists("/log.csv")) {
    File f = LittleFS.open("/log.csv", FILE_WRITE);
    f.println("time,temp_C,humidity_pct");
    f.close();
  }


   WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");

Serial.println("Commands: d = dump log, c = clear log, s = size");
}

void loop() {
  // Serial commands
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'd') dumpLog();
    if (cmd == 'c') {
      LittleFS.remove("/log.csv");
      File f = LittleFS.open("/log.csv", FILE_WRITE);
      f.println("time,temp_C,humidity_pct");
      f.close();
      Serial.println("Log cleared");
    }
    if (cmd == 's') {
      File f = LittleFS.open("/log.csv", FILE_READ);
      Serial.printf("Log size: %u bytes\n", f.size());
      f.close();
    }
  }

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Waiting for time...");
    delay(1000);
    return;
  }

  char timeStr[20];   // moved here so the whole loop can use it
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);

  if (isnan(h) || isnan(t)) {
    Serial.println("Sensor read failed");
  } else {
    Serial.printf("%s, %.1f, %.1f\n", timeStr, t, h);

    if (millis() - lastLog >= LOG_INTERVAL) {
      lastLog = millis();
      File f = LittleFS.open("/log.csv", FILE_APPEND);
      if (f) {
        f.printf("%s,%.1f,%.1f\n", timeStr, t, h);
        f.close();
        Serial.println("  -> saved");
      } else {
        Serial.println("  -> save FAILED");
      }
    }
  }
  delay(2000);
}
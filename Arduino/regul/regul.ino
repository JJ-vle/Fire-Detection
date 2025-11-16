//regul.ino
#include <ArduinoJson.h>
#include <WiFi.h>
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "FS.h"
#include <LittleFS.h>

#include "config.h"
#include "sensors.h"
#include "actuators.h"
#include "routes.h"
#include "wifi_utils.h"

#define USE_SERIAL Serial

// définition des variables globales
extern int fanDuty;
String last_temp = "N/A";
String last_light = "0";

float SEUIL_BAS = 26.0;
float SEUIL_HAUT = 28.0;
float SOUS_SEUIL_FAN = 27.0;

unsigned long lastPrint = 0;
const unsigned long LOOP_DELAY_MS = 1000;

AsyncWebServer server(80);

void handleSerialInput();
void sendStatusJson(float tempC, int lightVal, int lightAvg, bool fire, unsigned long now);
void updateSensorsAndActuators();

// initialiser
void setup() {
  USE_SERIAL.begin(115200);
  delay(100);

  pinMode(PIN_CLIM, OUTPUT);
  pinMode(PIN_HEAT, OUTPUT);
  pinMode(PIN_ONBOARD_LED, OUTPUT);

  initTemperature();
  initFan();
  initLight();
  initNeoPixel();

  SOUS_SEUIL_FAN = (SEUIL_HAUT + SEUIL_BAS) / 2.0;

  String hostname = "Mon_petit_objet_ESP32";
  wifi_connect_multi(hostname);

  if (!LittleFS.begin(true)) {
    USE_SERIAL.println("LittleFS mount error");
  }

  setup_http_routes(&server);
  server.begin();
}
void loop() {
  handleSerialInput();

  unsigned long now = millis();
  if (now - lastPrint >= LOOP_DELAY_MS) {
    lastPrint = now;
    updateSensorsAndActuators();
  }
}

// gestion des commandes reçues sur le port série
void handleSerialInput() {
  if (!Serial.available()) return;
  String input = Serial.readStringUntil('\n');
  input.trim();

  if (input.startsWith("MAX:")) {
    SEUIL_HAUT = input.substring(4).toFloat();
    SOUS_SEUIL_FAN = (SEUIL_HAUT + SEUIL_BAS) / 2.0;
  }
  else if (input.startsWith("MIN:")) {
    SEUIL_BAS = input.substring(4).toFloat();
    SOUS_SEUIL_FAN = (SEUIL_HAUT + SEUIL_BAS) / 2.0;
  }
}

void sendStatusJson(float tempC, int lightVal, int lightAvg, bool fire, unsigned long now) {
  DynamicJsonDocument doc(512);
  doc["timestamp_ms"] = now;
  if (isnan(tempC)) {
    doc["temperature_c"] = nullptr;
  } else {
    doc["temperature_c"] = tempC;
  }

  JsonObject actu = doc.createNestedObject("actuators");
  actu["fan_pwm"] = fanDuty;
  actu["clim"] = (bool)digitalRead(PIN_CLIM);
  actu["heat"] = (bool)digitalRead(PIN_HEAT);

  doc["light_raw"] = lightVal;
  doc["light_avg"] = lightAvg;
  doc["fire_detected"] = fire;

  JsonObject th = doc.createNestedObject("thresholds");
  th["low"] = SEUIL_BAS;
  th["high"] = SEUIL_HAUT;

  // === Infos WiFi ===
  JsonObject wifi = doc.createNestedObject("wifi");
  wifi["ssid"] = WiFi.SSID();
  wifi["mac"] = WiFi.macAddress();
  wifi["ip"] = WiFi.localIP().toString();
  wifi["rssi_dbm"] = WiFi.RSSI();
  wifi["rssi_percent"] = constrain(2 * (WiFi.RSSI() + 100), 0, 100);

  serializeJson(doc, Serial);
  Serial.println();
}

void updateSensorsAndActuators() {
  float tempC = readTemperature();
  int lightVal = readLight();
  pushLightValue(lightVal);

  int lightAvg = lightAverage();
  bool fire = detectFire(lightVal);

  if (!isnan(tempC)) {
    controlTemperature(tempC);
    updateFan(tempC, fire);
  }

  updateNeoPixelForTemp(tempC);
  digitalWrite(PIN_ONBOARD_LED, fire ? HIGH : LOW);

  last_temp = isnan(tempC) ? "N/A" : String(tempC, 2);
  last_light = String(lightVal);

  sendStatusJson(tempC, lightVal, lightAvg, fire, millis());
}

#include <ArduinoJson.h>

#include "config.h"
/*#include "./modules/sensors/light.h"
#include "./modules/sensors/temperature.h"
#include "./modules/actuators/fan.h"
#include "./modules/actuators/neopixel.h"*/

#include "light.h"
#include "temperature.h"
#include "fan.h"
#include "neopixel.h"

// Définition des variables globales
float SEUIL_BAS = 26.0;
float SEUIL_HAUT = 28.0;
float SOUS_SEUIL_FAN = 0;

unsigned long lastPrint = 0;
const unsigned long LOOP_DELAY_MS = 1000;

void handleSerialInput() {
  if (!Serial.available()) return;

  String input = Serial.readStringUntil('\n');
  input.trim();

  if (input.startsWith("MAX:")) {
    float newMax = input.substring(4).toFloat();
    SEUIL_HAUT = newMax;
    SOUS_SEUIL_FAN = (SEUIL_HAUT + SEUIL_BAS) / 2.0;
    Serial.println(F("{\"info\":\"Seuil haut modifié\"}"));
  } 
  else if (input.startsWith("MIN:")) {
    float newMin = input.substring(4).toFloat();
    SEUIL_BAS = newMin;
    SOUS_SEUIL_FAN = (SEUIL_HAUT + SEUIL_BAS) / 2.0;
    Serial.println(F("{\"info\":\"Seuil bas modifié\"}"));
  }
}


void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(PIN_CLIM, OUTPUT);
  pinMode(PIN_HEAT, OUTPUT);
  pinMode(PIN_ONBOARD_LED, OUTPUT);

  initTemperature();
  initFan();
  initLight();
  initNeoPixel();

  SOUS_SEUIL_FAN = (SEUIL_HAUT + SEUIL_BAS) / 2.0;
}

void loop() {

  handleSerialInput();

  unsigned long now = millis();
  if (now - lastPrint < LOOP_DELAY_MS) return;
  lastPrint = now;

  // Lecture
  float tempC = readTemperature();
  int lightVal = readLight();
  pushLightValue(lightVal);
  bool fire = detectFire(lightVal);

  // Actions
  if (!isnan(tempC)) {
    controlTemperature(tempC);
    updateFan(tempC);
  }
  updateNeoPixelForTemp(tempC);
  digitalWrite(PIN_ONBOARD_LED, fire ? HIGH : LOW);

  // JSON
  DynamicJsonDocument doc(512);
  doc["timestamp_ms"] = now;
  if (isnan(tempC)) {
    doc["temperature_c"] = nullptr;
  } else {
    doc["temperature_c"] = tempC;
  }
  doc["light_raw"] = lightVal;
  doc["fire_detected"] = fire;
  doc["fan_pwm"] = fanDuty;
  doc["seuil_low"] = SEUIL_BAS;
  doc["seuil_high"] = SEUIL_HAUT;

  serializeJson(doc, Serial);
  Serial.println();
}

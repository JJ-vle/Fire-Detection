#include "sensors.h"
#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// ======================= TEMPERATURE ===========================
OneWire oneWire(PIN_DS18B20);
DallasTemperature sensors(&oneWire);

void initTemperature() {
  sensors.begin();
}

float readTemperature() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  if (tempC == DEVICE_DISCONNECTED_C) return NAN;
  return tempC;
}

// ======================== LUMIÈRE ==============================
int lightBuffer[LIGHT_BUFFER_SIZE];
int lightIndex = 0;
bool lightBufferFilled = false;

void initLight() {
  analogReadResolution(12);
}

int readLight() {
  return analogRead(PIN_LIGHT);
}

void pushLightValue(int v) {
  lightBuffer[lightIndex++] = v;
  if (lightIndex >= LIGHT_BUFFER_SIZE) {
    lightIndex = 0;
    lightBufferFilled = true;
  }
}

int lightAverage() {
  long s = 0;
  int count = lightBufferFilled ? LIGHT_BUFFER_SIZE : lightIndex;
  if (count == 0) return 0;
  for (int i = 0; i < count; i++) s += lightBuffer[i];
  return (int)(s / count);
}

// ====================== DÉTECTION FEU ===========================
bool detectFire(int currentLight) {
  int avgLight = lightAverage();
  float tempC = readTemperature();
  if (isnan(tempC)) return false;

  bool lightHigh = (avgLight >= LIGHT_ABSOLUTE_THRESHOLD);
  bool tempHigh = (tempC >= SEUIL_HAUT);

  static unsigned long lastFireMillis = 0;
  bool detectedNow = lightHigh && tempHigh;

  if (detectedNow) {
    lastFireMillis = millis();
    return true;
  }

  if (millis() - lastFireMillis < FIRE_HOLD_TIME_MS)
    return true;

  return false;
}

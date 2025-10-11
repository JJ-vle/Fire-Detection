// temperature.cpp
#include "config.h"
#include "temperature.h"

#include <OneWire.h>
#include <DallasTemperature.h>

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

void controlTemperature(float tempC) {
  if (tempC > SEUIL_HAUT) {
    digitalWrite(PIN_CLIM, HIGH);
    digitalWrite(PIN_HEAT, LOW);
  } else if (tempC < SEUIL_BAS) {
    digitalWrite(PIN_HEAT, HIGH);
    digitalWrite(PIN_CLIM, LOW);
  } else {
    digitalWrite(PIN_CLIM, LOW);
    digitalWrite(PIN_HEAT, LOW);
  }
}

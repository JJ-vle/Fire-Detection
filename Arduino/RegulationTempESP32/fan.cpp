// fan.cpp
#include "config.h"
#include "fan.h"

int fanDuty = 0;

void initFan() {
  pinMode(PIN_FAN, OUTPUT);
}

void updateFan(float tempC) {
  if (tempC <= SOUS_SEUIL_FAN) fanDuty = FAN_MIN;
  else if (tempC >= SEUIL_HAUT) fanDuty = FAN_MAX;
  else {
    float frac = (tempC - SOUS_SEUIL_FAN) / (SEUIL_HAUT - SOUS_SEUIL_FAN);
    if (frac < 0) frac = 0;
    if (frac > 1) frac = 1;
    fanDuty = FAN_MIN + (int)(frac * (FAN_MAX - FAN_MIN));
  }
  analogWrite(PIN_FAN, fanDuty);
}

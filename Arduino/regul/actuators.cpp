//actuators.cpp
#include "actuators.h"
#include "config.h"
#include <Adafruit_NeoPixel.h>

// ====================== VENTILATEUR =============================
int fanDuty = 0;

void initFan() {
  pinMode(PIN_FAN, OUTPUT);
}

void updateFan(float tempC, bool fire) {
  if (tempC <= SOUS_SEUIL_FAN) fanDuty = FAN_MIN;
  else if (tempC >= SEUIL_HAUT) fanDuty = FAN_MAX;
  else if (fire) fanDuty = 0;
  else {
    float frac = (tempC - SOUS_SEUIL_FAN) / (SEUIL_HAUT - SOUS_SEUIL_FAN);
    frac = constrain(frac, 0.0, 1.0);
    fanDuty = FAN_MIN + (int)(frac * (FAN_MAX - FAN_MIN));
  }
  analogWrite(PIN_FAN, fanDuty);
}

// ===================== CHAUFFAGE / CLIM =========================
void controlTemperature(float tempC) {
  if (tempC > SEUIL_HAUT) {
    digitalWrite(PIN_CLIM, HIGH);
    digitalWrite(PIN_HEAT, LOW);
  }
  else if (tempC < SEUIL_BAS) {
    digitalWrite(PIN_HEAT, HIGH);
    digitalWrite(PIN_CLIM, LOW);
  }
  else {
    digitalWrite(PIN_CLIM, LOW);
    digitalWrite(PIN_HEAT, LOW);
  }
}

// ===================== NEOPIXELS ================================
Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void initNeoPixel() {
  strip.begin();
  strip.show();
  strip.setBrightness(80);
}

void setAllNeoPixel(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

void updateNeoPixelForTemp(float tempC) {
  if (tempC > SEUIL_HAUT) setAllNeoPixel(255, 0, 0);        // rouge
  else if (tempC >= SEUIL_BAS && tempC <= SEUIL_HAUT) setAllNeoPixel(255, 165, 0); // orange
  else setAllNeoPixel(0, 255, 0);                           // vert
}

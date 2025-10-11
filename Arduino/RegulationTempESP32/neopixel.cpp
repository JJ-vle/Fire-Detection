// neopixel.cpp
#include "config.h"
#include "neopixel.h"
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void initNeoPixel() {
  strip.begin();
  strip.show();
  strip.setBrightness(80);
}

void setAllNeoPixel(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NEOPIXEL_COUNT; i++) strip.setPixelColor(i, strip.Color(r, g, b));
  strip.show();
}

void updateNeoPixelForTemp(float tempC) {
  if (tempC > SEUIL_HAUT) setAllNeoPixel(255, 0, 0);
  else if (tempC < SEUIL_BAS) setAllNeoPixel(0, 0, 255);
  else setAllNeoPixel(0, 255, 0);
}

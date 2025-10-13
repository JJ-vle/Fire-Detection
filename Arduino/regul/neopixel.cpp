// neopixel.cpp
#include "config.h"
#include "neopixel.h"
#include <Adafruit_NeoPixel.h>

// créer un objet 'strip' représentant la bande LED avec nb LEDs, pin et protocole de communication
Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// initi&lisation
void initNeoPixel() {
  strip.begin();
  strip.show();
  strip.setBrightness(80);
}

// allume toutes les LEDs avec une couleur donnée
void setAllNeoPixel(uint8_t r, uint8_t g, uint8_t b) {
  // Boucle sur chaque LED pour lui assigner la couleur demandée (RGB)
  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

// change couleur LEDs selon temp
void updateNeoPixelForTemp(float tempC) {
  // si temp dépasse seuil haut -> rouge
  if (tempC > SEUIL_HAUT) {
    setAllNeoPixel(255, 0, 0);
  }
  // si temp est entre les deux seuils -> orange
  else if (tempC >= SEUIL_BAS && tempC <= SEUIL_HAUT) {
    setAllNeoPixel(255, 165, 0);
  }
  // si temp en dessous du seuil bas -> vert
  else {
    setAllNeoPixel(0, 255, 0); 
  }
}


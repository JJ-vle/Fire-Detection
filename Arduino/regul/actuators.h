//actuators.h
#pragma once
#include <Arduino.h>

// ---- Ventilateur ----
void initFan();
void updateFan(float tempC, bool fire);
extern int fanDuty;

// ---- Chauffage / Clim ----
void controlTemperature(float tempC);

// ---- LEDs NeoPixel ----
void initNeoPixel();
void setAllNeoPixel(uint8_t r, uint8_t g, uint8_t b);
void updateNeoPixelForTemp(float tempC);

// neopixel.h
#pragma once
#include <Arduino.h>

void initNeoPixel();
void setAllNeoPixel(uint8_t r, uint8_t g, uint8_t b);
void updateNeoPixelForTemp(float tempC);

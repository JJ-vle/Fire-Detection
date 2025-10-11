// light.cpp
#include "light.h"
#include "config.h"

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

bool detectFire(int currentLight) {
  int avg = lightAverage();
  if (currentLight >= LIGHT_ABSOLUTE_THRESHOLD) return true;
  if (lightBufferFilled && (currentLight - avg) >= LIGHT_DELTA_THRESHOLD) return true;
  return false;
}

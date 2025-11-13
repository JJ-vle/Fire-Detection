#pragma once
#include <Arduino.h>

// ---- Température ----
void initTemperature();
float readTemperature();

// ---- Lumière ----
void initLight();
int readLight();
void pushLightValue(int v);
int lightAverage();
bool detectFire(int currentLight);

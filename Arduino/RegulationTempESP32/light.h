// light.h
#pragma once
#include <Arduino.h>

void initLight();
int readLight();
bool detectFire(int currentLight);
int lightAverage();
void pushLightValue(int v);

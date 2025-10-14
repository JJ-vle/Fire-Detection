// fan.h
#pragma once
#include <Arduino.h>

void initFan();
void updateFan(float tempC, bool fire);
extern int fanDuty;

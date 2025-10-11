// fan.h
#pragma once
#include <Arduino.h>

void initFan();
void updateFan(float tempC);
extern int fanDuty;

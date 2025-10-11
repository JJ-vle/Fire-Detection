// temperature.h
#pragma once
#include <Arduino.h>
#include <DallasTemperature.h>

void initTemperature();
float readTemperature();
void controlTemperature(float tempC);

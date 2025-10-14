// config.h
#pragma once

// Seuils de consigne
extern float SEUIL_BAS;
extern float SEUIL_HAUT;
extern float SOUS_SEUIL_FAN;

// Ventilateur
const int FAN_MIN = 0;
const int FAN_MAX = 255;

// NeoPixel
#define NEOPIXEL_PIN 13
#define NEOPIXEL_COUNT 5

// Pins
#define PIN_DS18B20 23
#define PIN_CLIM 19
#define PIN_HEAT 21
#define PIN_FAN  27
#define PIN_LIGHT 33
#define PIN_ONBOARD_LED 2

// Lumière
const int LIGHT_BUFFER_SIZE = 10;
const int LIGHT_DELTA_THRESHOLD = 800;
const int LIGHT_ABSOLUTE_THRESHOLD = 1000;
// Lumière - Détection de feu
const unsigned long FIRE_HOLD_TIME_MS = 3000; // garder l'état feu 3s minimum

// PWM
const int PWM_FREQ = 5000;
const int PWM_RESOLUTION = 8;
const int FAN_CHANNEL = 3;

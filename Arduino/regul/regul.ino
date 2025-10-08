/*
  Regulation temperature ambiante - ESP32 DEV KIT V1 (36 pins)
  Brochages obligatoires :
    - DS18B20 (OneWire) on GPIO 23
    - Climatisation (LED verte) on GPIO 19
    - Radiateur (LED rouge) on GPIO 21
    - Ventilateur PWM on GPIO 27 (progressif)
    - Bande NeoPixel on GPIO 13
    - Capteur lumière ADC1 CH5 -> GPIO 33
    - LED onboard (alerte incendie) on GPIO 2

  Dépendances :
    - OneWire
    - DallasTemperature
    - Adafruit_NeoPixel
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_NeoPixel.h>
#include "esp32-hal-ledc.h"



// ------------------- Paramètres utilisateur -------------------
// Seuils de consigne (modifiable facilement)
float SEUIL_BAS  = 26.0; // SB en °C  (chauffage en dessous)
float SEUIL_HAUT = 28.0; // SH en °C  (climatisation au dessus)

// Fan PWM behaviour
const float FAN_MAX_DELTA = 10.0; // °C au dessus de SH pour atteindre 100% de la vitesse
int chan;
// NeoPixel
#define NEOPIXEL_PIN 13
#define NEOPIXEL_COUNT 5

// Pins
#define PIN_DS18B20 23
#define PIN_CLIM 19    // LED verte
#define PIN_HEAT 21    // LED rouge
#define PIN_FAN  27    // PWM
#define PIN_LIGHT 33   // ADC1 channel 5 (A5)
#define PIN_ONBOARD_LED 2

// ADC / light detection
const int LIGHT_BUFFER_SIZE = 10;
const int LIGHT_DELTA_THRESHOLD = 800;   // delta au-dessus de la moyenne => possible incendie
const int LIGHT_ABSOLUTE_THRESHOLD = 3500; // valeur brute très élevée => incendie

// PWM (ESP32 ledc)
// on utilisera 8 bits de résolution (0-255)
const int PWM_FREQ = 5000;
const int PWM_RESOLUTION = 8;
const int FAN_CHANNEL = 3; // un channel libre

// ------------------- Objets / variables -------------------
OneWire oneWire(PIN_DS18B20);
DallasTemperature sensors(&oneWire);

Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Buffer circulaire pour moyenne lumière
int lightBuffer[LIGHT_BUFFER_SIZE];
int lightIndex = 0;
bool lightBufferFilled = false;

// ------------------- Fonctions utilitaires -------------------
void setAllNeoPixel(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NEOPIXEL_COUNT; i++) strip.setPixelColor(i, strip.Color(r, g, b));
  strip.show();
}

int readLight() {
  // Lire sur GPIO33 (ADC1). Arduino core pour ESP32 mappe analogRead directement.
  // Résolution native 12 bits => 0..4095
  return analogRead(PIN_LIGHT);
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
  // Condition de détection :
  //  - valeur actuelle très élevée (absolue), OU
  //  - augmentation notable par rapport à la moyenne
  if (currentLight >= LIGHT_ABSOLUTE_THRESHOLD) return true;
  if (lightBufferFilled) {
    if ((currentLight - avg) >= LIGHT_DELTA_THRESHOLD) return true;
  } else {
    // si pas encore rempli, être un peu moins strict
    if ((currentLight - avg) >= (LIGHT_DELTA_THRESHOLD * 1.2)) return true;
  }
  return false;
}

// Met à jour le buffer de lumière
void pushLightValue(int v) {
  lightBuffer[lightIndex++] = v;
  if (lightIndex >= LIGHT_BUFFER_SIZE) {
    lightIndex = 0;
    lightBufferFilled = true;
  }
}

// Met à jour sorties chauffage/clim/fan selon temp
void controlTemperature(float tempC) {
  // Cas 1 : temp > SEUIL_HAUT => climatisation ON, radiateur OFF.
  //           Ventilateur démarre et augmente avec delta
  // Cas 2 : temp < SEUIL_BAS => chauffage ON, clim OFF, fan OFF
  // Cas 3 : entre SB et SH => tout OFF
  if (tempC > SEUIL_HAUT) {
    digitalWrite(PIN_CLIM, HIGH);
    digitalWrite(PIN_HEAT, LOW);

    float over = tempC - SEUIL_HAUT;
    if (over < 0) over = 0;
    float frac = over / FAN_MAX_DELTA;
    if (frac > 1.0) frac = 1.0;
    int duty = (int)(frac * 255.0); // 0..255
    ledcWrite(FAN_CHANNEL, duty); // PWM
  }
  else if (tempC < SEUIL_BAS) {
    digitalWrite(PIN_HEAT, HIGH);
    digitalWrite(PIN_CLIM, LOW);
    ledcWrite(FAN_CHANNEL, 0);
  }
  else {
    // entre seuils : tout éteint
    digitalWrite(PIN_CLIM, LOW);
    digitalWrite(PIN_HEAT, LOW);
    ledcWrite(FAN_CHANNEL, 0);
  }
}

// Mise à jour état NeoPixel selon température
void updateNeoPixelForTemp(float tempC) {
  if (tempC > SEUIL_HAUT) {
    // ROUGE
    setAllNeoPixel(255, 0, 0);
  } else if (tempC < SEUIL_BAS) {
    // BLEU
    setAllNeoPixel(0, 0, 255);
  } else {
    // ENTRE : VERT
    setAllNeoPixel(0, 255, 0);
  }
}

// ------------------- Setup -------------------
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("=== Regulation Temperature ESP32 ===");

  // Init capteurs
  sensors.begin();

  // Pins digital outputs
  pinMode(PIN_CLIM, OUTPUT);
  pinMode(PIN_HEAT, OUTPUT);
  pinMode(PIN_ONBOARD_LED, OUTPUT);

  digitalWrite(PIN_CLIM, LOW);
  digitalWrite(PIN_HEAT, LOW);
  digitalWrite(PIN_ONBOARD_LED, LOW);

  // PWM fan setup
  // VENTILATEUR NON FONCTIONNEL
  //ledcSetup(FAN_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  //ledcAttachPin(PIN_FAN, FAN_CHANNEL);
  //ledcWrite(FAN_CHANNEL, 0);

  // NeoPixel init
  strip.begin();
  strip.show(); // off
  strip.setBrightness(80);

  // ADC config (optionnel) : préciser attenuation si besoin (par défaut)
  // analogSetPinAttenuation(PIN_LIGHT, ADC_11db); // si nécessaire pour plage plus large
  analogReadResolution(12); // 0..4095

  Serial.printf("Seuil bas (SB): %.2f C\n", SEUIL_BAS);
  Serial.printf("Seuil haut (SH): %.2f C\n", SEUIL_HAUT);
  Serial.println("Demarrage boucle de lecture...");
}

// ------------------- Loop -------------------
unsigned long lastPrint = 0;
const unsigned long LOOP_DELAY_MS = 1000;

void loop() {
  unsigned long now = millis();

  // On fait une lecture toutes les secondes
  if (now - lastPrint >= LOOP_DELAY_MS) {
    lastPrint = now;

    // 1) Lecture température
    sensors.requestTemperatures(); // lance conversion (bloquant court)
    float tempC = sensors.getTempCByIndex(0); // capteur index 0
    if (tempC == DEVICE_DISCONNECTED_C) {
      Serial.println("Erreur : capteur DS18B20 non trouve !");
      tempC = NAN;
    }

    // 2) Lecture lumière
    int lightVal = readLight();
    pushLightValue(lightVal);
    int lightAvg = lightAverage();

    // 3) Détection incendie
    bool fire = detectFire(lightVal);

    // 4) Contrôle clim/heat/fan
    if (!isnan(tempC)) controlTemperature(tempC);

    // 5) Mise à jour NeoPixel et onboard LED si incendie
    updateNeoPixelForTemp(tempC);
    if (fire) {
      digitalWrite(PIN_ONBOARD_LED, HIGH);
    } else {
      digitalWrite(PIN_ONBOARD_LED, LOW);
    }

    // 6) Affichage monitoring
    Serial.print("Temp: ");
    if (isnan(tempC)) Serial.print("NaN");
    else Serial.print(tempC, 2);
    Serial.print(" C  | Light: ");
    Serial.print(lightVal);
    Serial.print(" (avg ");
    Serial.print(lightAvg);
    Serial.print(")");

    // Afficher état actuateurs
    Serial.print(" | Clim:");
    Serial.print(digitalRead(PIN_CLIM) ? "ON" : "OFF");
    Serial.print(" | Heat:");
    Serial.print(digitalRead(PIN_HEAT) ? "ON" : "OFF");
    int fanDuty = ledcRead(FAN_CHANNEL);
    Serial.print(" | Fan PWM:");
    Serial.print(fanDuty);
    Serial.print("/255");

    if (fire) {
      Serial.print("  *** FIRE DETECTED! ***");
    }

    Serial.println();
  }

  // nothing else blocking
}

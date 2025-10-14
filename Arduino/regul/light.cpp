// light.cpp
#include "light.h"
#include "config.h"
#include "temperature.h"

int lightBuffer[LIGHT_BUFFER_SIZE]; // buffer circulaire pour stocker les dernières mesures de luminosité
int lightIndex = 0;
bool lightBufferFilled = false;

// initialisation
void initLight() {
  analogReadResolution(12);
}

// lecture instantannée de la val ducapteur
int readLight() {
  return analogRead(PIN_LIGHT);
}

// enregistre une nouvelle mesure dans le buffer
void pushLightValue(int v) {
  // stocke new val
  lightBuffer[lightIndex++] = v;

  // si on atteint la fin du buffer, on repart au début (buffer circulaire)
  if (lightIndex >= LIGHT_BUFFER_SIZE) {
    lightIndex = 0;
    lightBufferFilled = true; // buffer plein
  }
}

// calcule moyenne des val stockées dans le buffer
int lightAverage() {
  long s = 0; // somme des valeurs
  // si buffer pas encore rempli, on prend en compte que les vals enregistrées
  int count = lightBufferFilled ? LIGHT_BUFFER_SIZE : lightIndex;
  if (count == 0) return 0; // éviter une division par zéro
  for (int i = 0; i < count; i++) s += lightBuffer[i];
  return (int)(s / count); // retourne la moyenne
}


// détection d’un feu potentiel basé sur la luminosité et la température
// -> on cherche à détecter une situation où la température est élevée
//   ET où la luminosité moyenne est forte (lumière persistante, pas un flash soudain)
bool detectFire(int currentLight) {
  
  int avgLight = lightAverage();  // moyenne des dernières valeurs de lumière

  float tempC = readTemperature();
  if (isnan(tempC)) return false; // si capteur débranché, on ne détecte rien

  // detections
  bool lightHigh = (avgLight >= LIGHT_ABSOLUTE_THRESHOLD);
  bool tempHigh = (tempC >= SEUIL_HAUT);

  // mémorisation de l'état de feu
  static unsigned long lastFireMillis = 0;

  // feu détecté si lumière forte ET température haute
  bool detectedNow = lightHigh && tempHigh;

  if (detectedNow) {
    lastFireMillis = millis(); // on enregistre le moment de détection
    return true;
  }

  // si un feu a été détecté récemment, on garde l'état actif un court moment
  if (millis() - lastFireMillis < FIRE_HOLD_TIME_MS)
    return true;

  // sinon, pas de feu
  return false;
}


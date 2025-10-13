// light.cpp
#include "light.h"
#include "config.h"

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


// détection d’un feu potentiel basé sur la luminosité
bool detectFire(int currentLight) {
  int avg = lightAverage();

  // si luminosité dépasse seuil absolu -> feu détecté
  if (currentLight >= LIGHT_ABSOLUTE_THRESHOLD) return true;
  // si buffer plein et que luminosité actuelle est nettement supérieure à la moyenne -> feu détecté
  if (lightBufferFilled && (currentLight - avg) >= LIGHT_DELTA_THRESHOLD) return true;

  // sinon -> pas de feu détecté
  return false;
}

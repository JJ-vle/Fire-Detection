// temperature.cpp
#include "config.h"
#include "temperature.h"

#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(PIN_DS18B20); // bus OneWire connecté au capteur DS18B20
DallasTemperature sensors(&oneWire); // gestionnaire pour le capteur de température

// initialisation
void initTemperature() {
  sensors.begin();
}

// lecture température actuelle en degrés Celsius
float readTemperature() {
  sensors.requestTemperatures();              // envoie une requête de mesure au capteur
  float tempC = sensors.getTempCByIndex(0);   // récupère température du premier capteur sur le bus

  // si capteur déconnecté -> renvoie NaN (valeur non valide)
  if (tempC == DEVICE_DISCONNECTED_C) return NAN;

  return tempC;
}


// contrôle automatique chauffage et climaitsation
void controlTemperature(float tempC) {
  // si température dépasse seuil haut -> activer la clim, désactiver le chauffage
  if (tempC > SEUIL_HAUT) {
    digitalWrite(PIN_CLIM, HIGH);
    digitalWrite(PIN_HEAT, LOW);
  }

  // si température en dessous seuil bas -> activer le chauffage, désactiver la clim
  else if (tempC < SEUIL_BAS) {
    digitalWrite(PIN_HEAT, HIGH);
    digitalWrite(PIN_CLIM, LOW);
  }

  // si température dans plage normale -> tout désactivé
  else {
    digitalWrite(PIN_CLIM, LOW);
    digitalWrite(PIN_HEAT, LOW);
  }
}

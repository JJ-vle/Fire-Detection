// fan.cpp
#include "config.h"
#include "fan.h"

int fanDuty = 0;

// initialise le ventilateur (pin en sortie)
void initFan() {
  pinMode(PIN_FAN, OUTPUT);
}

// met à jour la puissance du ventilateur en fonction de la température
void updateFan(float tempC) {
  // si la température est en dessous du seuil intermédiaire, ventilateur au minimum (éteint)
  if (tempC <= SOUS_SEUIL_FAN) fanDuty = FAN_MIN;
  // si la température dépasse le seuil haut, ventilateur au maximum
  else if (tempC >= SEUIL_HAUT) fanDuty = FAN_MAX;

  // sinon, ventilateur proportionnel à la température
  else {
    float frac = (tempC - SOUS_SEUIL_FAN) / (SEUIL_HAUT - SOUS_SEUIL_FAN); // calcul de la fraction relative
    if (frac < 0) frac = 0;   // clamp fraction min à 0
    if (frac > 1) frac = 1;   // clamp fraction max à 1

    // interpolation linéaire entre FAN_MIN et FAN_MAX
    fanDuty = FAN_MIN + (int)(frac * (FAN_MAX - FAN_MIN));
  }

  // applique la valeur PWM au ventilateur
  analogWrite(PIN_FAN, fanDuty);
}

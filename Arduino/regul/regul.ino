//regul.ino
#include <ArduinoJson.h>
#include "config.h"
#include "sensors.h"
#include "actuators.h"
#include "wifi_utils.h"
#include "mqtt_client.h"
#include "piscine_utils.h"

// définition des variables globales
float SEUIL_BAS = 26.0;
float SEUIL_HAUT = 28.0;
float SOUS_SEUIL_FAN = 0;

unsigned long lastPrint = 0;
const unsigned long LOOP_DELAY_MS = 1000;

String lastStatusJson;
String hostname = "Mon petit objet ESP32";

String my_ident = "P_dt210745"; // VOTRE ID ICI
float my_lat = 43.7032; float my_lon = 7.2660; //MANDELIEU LA NAPOULE
float my_current_temp;

// gestion des commandes reçues sur le port série
void handleSerialInput() {
  if (!Serial.available()) return;

  String input = Serial.readStringUntil('\n');
  input.trim();

  if (input.startsWith("MAX:")) { // changer seuil haut
    float newMax = input.substring(4).toFloat();
    SEUIL_HAUT = newMax;
    SOUS_SEUIL_FAN = (SEUIL_HAUT + SEUIL_BAS) / 2.0;
  } 
  else if (input.startsWith("MIN:")) {  // changer seuil bas
    float newMin = input.substring(4).toFloat();
    SEUIL_BAS = newMin;
    SOUS_SEUIL_FAN = (SEUIL_HAUT + SEUIL_BAS) / 2.0;
  }
}

void sendStatusJson(float tempC, int lightVal, int lightAvg, bool fire, unsigned long now) {
  DynamicJsonDocument doc(512);
  doc["timestamp_ms"] = now;
  if (isnan(tempC)) {
    doc["temperature_c"] = nullptr;
  } else {
    doc["temperature_c"] = tempC;
    my_current_temp = tempC;
  }

  /********** RENDU PISCINE ************/
  doc["name"] = "JJT";
  doc["lat"] = my_lat; doc["lon"] = my_lon; 

  JsonObject info = doc.createNestedObject("info");
  info["ident"] = my_ident;

  JsonObject piscine = doc.createNestedObject("piscine");
  piscine["occuped"] = (lightVal > 700);
  piscine["hotspot"] = get_hotspot_status();

  JsonObject actuators = doc.createNestedObject("actuators");
  actuators["fan_pwm"] = fanDuty;
  actuators["clim"] = (bool)digitalRead(PIN_CLIM);
  actuators["heat"] = (bool)digitalRead(PIN_HEAT);

  doc["light_raw"] = lightVal;
  doc["light_avg"] = lightAvg;
  doc["fire_detected"] = fire;

  JsonObject thresholds = doc.createNestedObject("thresholds");
  thresholds["low"] = SEUIL_BAS;
  thresholds["high"] = SEUIL_HAUT;

  // ---- MAJ de la variable globale ----
  lastStatusJson.clear();
  serializeJson(doc, lastStatusJson);
}

void updateSensorsAndActuators() {
  unsigned long now = millis();
  float tempC = readTemperature();
  int lightVal = readLight();
  pushLightValue(lightVal);
  int lightAvg = lightAverage();
  bool fire = detectFire(lightVal);

  if (!isnan(tempC)) {
    controlTemperature(tempC);
    updateFan(tempC, fire);
  }

  updateNeoPixelForTemp(tempC);
  digitalWrite(PIN_ONBOARD_LED, fire ? HIGH : LOW);

  sendStatusJson(tempC, lightVal, lightAvg, fire, now);

  // ---- affichage Serial ----
  //Serial.println(lastStatusJson);

  sendMQTT(TOPIC_PUBLISH, lastStatusJson);
}

// initialiser
void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(PIN_CLIM, OUTPUT);
  pinMode(PIN_HEAT, OUTPUT);
  pinMode(PIN_ONBOARD_LED, OUTPUT);

  initTemperature();
  initFan();
  initLight();
  initNeoPixel();
  
  // --- WIFI & MQTT SETUP ---
  wifi_connect_multi(hostname); 
  initMQTT(); // Initialise le client MQTT
  
  Serial.println(WiFi.localIP());
  // ------------------------

  SOUS_SEUIL_FAN = (SEUIL_HAUT + SEUIL_BAS) / 2.0;
}

void loop() {
  // --- MAINTIEN DE LA CONNEXION ---
  handleMQTT(); 
  // --------------------------------

  handleSerialInput();

  unsigned long now = millis();

  if (now - lastPrint >= LOOP_DELAY_MS) {
    lastPrint = now;
    updateSensorsAndActuators();
  }

  //delay(5000);
}


//mqtt_client.cpp
#include "mqtt_client.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "piscine_utils.h"

// --- Configuration Serveur ---
//const char* mqtt_server = "10.0.1.58"; 
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
bool wasConnected = false;

// --- Déclaration interne (ne pas toucher depuis l'extérieur) ---
void mqtt_pubcallback(char* topic, byte* payload, unsigned int length);
void mqtt_reconnect();

// ================= IMPLEMENTATION =================

void initMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqtt_pubcallback);
  client.setBufferSize(2048);
}

/*
void handleMQTT() {
  if (!client.connected()) {
    mqtt_reconnect();
  }
  client.loop();
}
*/

void handleMQTT() {
  bool mqtt_now = client.connected();

  if (!mqtt_now) {
    mqtt_reconnect();
  }

  client.loop();

  // Si on vient JUSTE de se connecter
  if (!wasConnected && mqtt_now) {
    Serial.println("MQTT vient de se connecter --> on publie l’état !");
    sendMQTT(TOPIC_PUBLISH, lastStatusJson);
  }

  wasConnected = mqtt_now;
}



void sendMQTT(const char* topic, String payload) {
  if (client.connected()) {
    bool ok = client.publish(topic, payload.c_str());
    Serial.println(ok ? "MQTT publish OK" : "MQTT publish FAILED");
  }
}

// --- Fonctions internes ---

void mqtt_reconnect() {
  // On ne bloque pas tout le programme indéfiniment, on tente juste une fois
  // Si ça échoue, on réessaiera au prochain tour de loop()
  if (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    
    String clientId = "ESP32-PoolMaster-";
    clientId += WiFi.macAddress();

    if (client.connect(clientId.c_str())) {
      Serial.println("Connecté !");
      // On se réabonne aux topics ici
      client.subscribe(TOPIC_PUBLISH);
    } else {
      Serial.print("Echec, rc=");
      Serial.print(client.state());
      // On ne met pas de delay(5000) bloquant ici pour ne pas arrêter la régulation
      // La reconnexion se fera naturellement au prochain passage
    }
  }
}
/*
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message reçu sur [");
  Serial.print(topic);
  Serial.print("] : ");

  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // --- TRAITEMENT DES COMMANDES ---
  if (String(topic) == TOPIC_LED) {
    // Exemple simple : allumer la LED embarquée
    // Note : Assure-toi que PIN_ONBOARD_LED est défini dans config.h
    if (message == "on") {
      digitalWrite(PIN_ONBOARD_LED, HIGH);
    } else if (message == "off") {
      digitalWrite(PIN_ONBOARD_LED, LOW);
    }
  }
}
*/

/*============== CALLBACK RECUPERATION DONNEES ===================*/
void mqtt_pubcallback(char* topic, byte* payload, unsigned int length) {
  
  // 1. Convertir le payload en String manipulable
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("DATA RECEIVED");
  Serial.println(message);
  
  // Debug léger (pour ne pas saturer si beaucoup de monde parle)
  // USE_SERIAL.print("Recu: "); USE_SERIAL.println(message);

  // 2. Extraire les infos du voisin (Parsing manuel simple)
  // On utilise notre helper défini dans piscine_utils.h
  /*String neighbor_id = extract_json_value(message, "\"ident\"");
  String s_temp = extract_json_value(message, "\"temperature\"");
  String s_lat = extract_json_value(message, "\"lat\"");
  String s_lon = extract_json_value(message, "\"lon\"");

  // Vérifier qu'on a bien tout trouvé avant de calculer
  if (neighbor_id != "" && s_temp != "" && s_lat != "" && s_lon != "") {
      float n_temp = s_temp.toFloat();
      float n_lat = s_lat.toFloat();
      float n_lon = s_lon.toFloat();

      // 3. LANCER LA LOGIQUE EDGE COMPUTING
      process_neighbor_data(my_ident, my_lat, my_lon, my_current_temp,
                            neighbor_id, n_lat, n_lon, n_temp);
  }*/

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("Erreur JSON: ");
    Serial.println(error.c_str());
    return;
  }

  String neighbor_id = doc["info"]["ident"] | "";
  float n_temp = doc["status"]["temperature"] | NAN;
  float n_lat  = doc["location"]["gps"]["lat"] | NAN;
  float n_lon  = doc["location"]["gps"]["lon"] | NAN;

  if (neighbor_id != "" && !isnan(n_temp) && !isnan(n_lat) && !isnan(n_lon)) {
    process_neighbor_data(my_ident, my_lat, my_lon, my_current_temp,
                          neighbor_id, n_lat, n_lon, n_temp);
  }
}
/*============= SUBSCRIBE ===================*/
void mqtt_subscribe_mytopics() {
  // Tant qu'on n'est pas connecté au MQTT
  while (!client.connected()) {
    USE_SERIAL.print("Attempting MQTT connection...");
    
    // Création d'un ID unique basé sur l'adresse MAC
    String clientId = "ESP32-";
    clientId += WiFi.macAddress();
    
    // Tentative de connexion
    if (client.connect(clientId.c_str(), NULL, NULL)) {
      USE_SERIAL.println("connected");
      
      // --- ABONNEMENTS ---
      // 1. Pour recevoir les infos des autres piscines (Hotspot)
      client.subscribe(TOPIC_PUBLISH); 
      
      // 2. Autres abonnements optionnels (LED, Temp...)
      // client.subscribe(TOPIC_TEMP); 
      
    } else {
      USE_SERIAL.print("failed, rc=");
      USE_SERIAL.print(client.state());
      USE_SERIAL.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}




//mqtt_client.cpp
#include "mqtt_client.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h" // Pour récupérer PIN_ONBOARD_LED ou autres

// --- Configuration Serveur ---
//const char* mqtt_server = "10.0.1.58"; 
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
bool wasConnected = false;

// --- Déclaration interne (ne pas toucher depuis l'extérieur) ---
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void mqtt_reconnect();

// ================= IMPLEMENTATION =================

void initMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqtt_callback);
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
      client.subscribe(TOPIC_LED);
    } else {
      Serial.print("Echec, rc=");
      Serial.print(client.state());
      // On ne met pas de delay(5000) bloquant ici pour ne pas arrêter la régulation
      // La reconnexion se fera naturellement au prochain passage
    }
  }
}

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
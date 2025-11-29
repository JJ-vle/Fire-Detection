//mqtt_client.h
#pragma once
#include <Arduino.h>

// Configuration MQTT
#define TOPIC_TEMP "uca/M1/iot/temp"
#define TOPIC_LED  "uca/M1/iot/led"
#define TOPIC_PUBLISH  "uca/iot/master"

// Initialisation (à mettre dans le setup)
void initMQTT();

// Gestion de la boucle (à mettre dans le loop)
void handleMQTT();

// Fonction pour envoyer un message (celle que tu voulais)
void sendMQTT(const char* topic, String payload);
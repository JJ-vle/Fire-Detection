/*** Fichier piscine_utils.h 
   Gère les capteurs et la logique Edge Computing (Occuped & Hotspot)
***/
#ifndef PISCINE_UTILS_H
#define PISCINE_UTILS_H

#include <Arduino.h>
#include <math.h> // Nécessaire pour le calcul de distance (sin, cos, sqrt...)
#include "piscine_utils.h"

// --- CONFIGURATION ---
const int photoResistorPin = 34; 
const int lightThreshold = 2000; 

// Variables globales pour l'état Hotspot
bool i_am_hotspot = true;             // Par défaut, on est le roi du monde
unsigned long last_hotter_neighbor_time = 0; // Timestamp du dernier voisin "plus fort" vu
const long HOTSPOT_TIMEOUT = 30000;   // Durée (30s) avant de redevenir hotspot si le voisin disparait

// --- FONCTIONS EXISTANTES ---
void init_piscine_sensors() {
    pinMode(photoResistorPin, INPUT);
}

const char* get_occupied_status() {
    return (analogRead(photoResistorPin) > lightThreshold) ? "true" : "false";
}

// --- NOUVELLES FONCTIONS MATHS & LOGIQUE ---

// 1. Formule de Haversine : Distance entre deux points GPS en km
double haversine(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0; // Rayon de la Terre en km
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;
    double a = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
    double c = 2 * asin(sqrt(a));
    return R * c;
}

// 2. Fonction principale : Traite les données d'un voisin reçu
void process_neighbor_data(String my_id, float my_lat, float my_lon, float my_temp, 
                           String neighbor_id, float neighbor_lat, float neighbor_lon, float neighbor_temp) {
    
    // A. Surtout, ne pas se comparer à soi-même !
    if (neighbor_id == my_id) return; 

    // B. Calculer la distance
    double dist = haversine(my_lat, my_lon, neighbor_lat, neighbor_lon);

    // C. La règle du Hotspot (Section 4.2.2)
    // "Supérieure à celle de tous les autres objets dans un rayon de 10 Kms"
    if (dist <= 10.0) {
        if (neighbor_temp > my_temp) {
            // J'ai trouvé plus chaud que moi près de chez moi -> Je perds mon titre
            i_am_hotspot = false;
            last_hotter_neighbor_time = millis(); // On note l'heure de la défaite
            
            // Debug optionnel
            Serial.printf("Perdu vs %s ! (Dist: %.2f km, Temp: %.2f > %.2f)\n", neighbor_id.c_str(), dist, neighbor_temp, my_temp);
        }
    }
}

// 3. Récupérer l'état actuel (à appeler dans la loop pour construire le JSON)
const char* get_hotspot_status() {
    // Si ça fait plus de 30s qu'on n'a pas vu de voisin plus chaud, on redevient hotspot
    if (millis() - last_hotter_neighbor_time > HOTSPOT_TIMEOUT) {
        i_am_hotspot = true;
    }
    return i_am_hotspot ? "true" : "false";
}

// 4. Helper pour extraire une valeur simple du JSON (sans bibliothèque complexe)
String extract_json_value(String json, String key) {
    int start = json.indexOf(key);
    if (start == -1) return "";
    
    // Cherche le début de la valeur (après le :)
    start = json.indexOf(":", start) + 1;
    
    // Cherche la fin (virgule ou accolade fermante)
    int end_comma = json.indexOf(",", start);
    int end_brace = json.indexOf("}", start);
    int end = (end_comma == -1) ? end_brace : (end_comma < end_brace ? end_comma : end_brace);
    
    String value = json.substring(start, end);
    
    // Nettoyage des guillemets pour les chaines
    value.replace("\"", ""); 
    value.trim();
    return value;
}

#endif
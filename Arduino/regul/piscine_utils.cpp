#include "piscine_utils.h"
#include <math.h>

// --------------------------
// VARIABLES GLOBALES
// --------------------------
bool i_am_hotspot = true;
unsigned long last_hotter_neighbor_time = 0;
const long HOTSPOT_TIMEOUT = 30000;

const int photoResistorPin = 34;
const int lightThreshold = 2000;


// --------------------------
// INITIALISATION
// --------------------------
void init_piscine_sensors() {
    pinMode(photoResistorPin, INPUT);
}


// --------------------------
// OCCUPATION
// --------------------------
const char* get_occupied_status() {
    return (analogRead(photoResistorPin) > lightThreshold) ? "true" : "false";
}


// --------------------------
// HAVERSINE
// --------------------------
double haversine(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0;
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double a = pow(sin(dLat / 2), 2) +
               pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
    double c = 2 * asin(sqrt(a));
    return R * c;
}


// --------------------------
// HOTSPOT
// --------------------------
void process_neighbor_data(String my_id, float my_lat, float my_lon, float my_temp,
                           String neighbor_id, float neighbor_lat, float neighbor_lon, float neighbor_temp) 
{
    if (neighbor_id == my_id) return;

    double dist = haversine(my_lat, my_lon, neighbor_lat, neighbor_lon);

    if (dist <= 10.0) {
        if (neighbor_temp > my_temp) {
            i_am_hotspot = false;
            last_hotter_neighbor_time = millis();
            Serial.println(i_am_hotspot);
            Serial.println(get_hotspot_status());
            Serial.printf("HOTSPOT LOST: %s hotter than me (%.1f > %.1f at %.2f km)\n",
                          neighbor_id.c_str(), neighbor_temp, my_temp, dist);
        }
    }
}

const char* get_hotspot_status() {
    if (millis() - last_hotter_neighbor_time > HOTSPOT_TIMEOUT) {
        i_am_hotspot = true;
    }
    return i_am_hotspot ? "true" : "false";
}


// --------------------------
// JSON SIMPLE
// --------------------------
String extract_json_value(String json, String key) {
    int start = json.indexOf(key);
    if (start == -1) return "";

    start = json.indexOf(":", start) + 1;
    int end1 = json.indexOf(",", start);
    int end2 = json.indexOf("}", start);

    int end = (end1 == -1) ? end2 : min(end1, end2);

    String v = json.substring(start, end);
    v.replace("\"", "");
    v.trim();
    return v;
}

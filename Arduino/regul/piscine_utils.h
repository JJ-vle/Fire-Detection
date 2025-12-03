#ifndef PISCINE_UTILS_H
#define PISCINE_UTILS_H

#include <Arduino.h>

// ---------------------
// VARIABLES GLOBALES
// ---------------------
extern bool i_am_hotspot;
extern unsigned long last_hotter_neighbor_time;

// ---------------------
// PROTOTYPES
// ---------------------
void init_piscine_sensors();

double haversine(double lat1, double lon1, double lat2, double lon2);

void process_neighbor_data(String my_id, float my_lat, float my_lon, float my_temp,
                           String neighbor_id, float neighbor_lat, float neighbor_lon, float neighbor_temp);

const char* get_occupied_status();
const char* get_hotspot_status();

String extract_json_value(String json, String key);

#endif

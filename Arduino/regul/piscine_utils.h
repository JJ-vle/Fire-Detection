#pragma once

extern bool i_am_hotspot;
extern unsigned long last_hotter_neighbor_time;

void init_piscine_sensors();
//bool get_occupied_status();
double haversine(double lat1, double lon1, double lat2, double lon2);
void process_neighbor_data(String id, float lat, float lon, float temp,
                           String nid, float nlat, float nlon, float ntemp);
//bool get_hotspot_status();
String extract_json_value(String json, String key);

const char* get_occupied_status();
const char* get_hotspot_status();


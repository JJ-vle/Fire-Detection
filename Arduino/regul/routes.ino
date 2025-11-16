// routes.ino
#include "ESPAsyncWebServer.h"
#include "routes.h"
#include "FS.h"
#include <LittleFS.h>
#include <WiFi.h>

#define USE_SERIAL Serial

extern String last_temp;
extern String last_light;

/*===================================================*/
String processor(const String & var){
  if (var == "TEMPERATURE") {
    return last_temp;
  }
  else if (var == "LIGHT") {
    return last_light;
  }
  else if (var == "UPTIME") {
    return String(millis() / 1000);
  }
  else if (var == "WHERE") {
    return "ESP32-LAB";
  }
  else if (var == "SSID") {
    return WiFi.SSID();
  }
  else if (var == "MAC") {
    return WiFi.macAddress();
  }
  else if (var == "IP") {
    return WiFi.localIP().toString();
  }
  else if (var == "COOLER") {
    return String((int)digitalRead(PIN_CLIM));
  }
  else if (var == "HEATER") {
    return String((int)digitalRead(PIN_HEAT));
  }
  else if (var == "LT") {
    return String( (int) SEUIL_BAS );
  }
  else if (var == "HT") {
    return String( (int) SEUIL_HAUT );
  }
  else if (var == "PRT_IP") {
    // target_ip declared in main if needed; to keep simple return empty
    return String("");
  }
  else if (var == "PRT_PORT") {
    return String("");
  }
  else if (var == "PRT_T") {
    return String("");
  }
  return String();
}

void setup_http_routes(AsyncWebServer* server) {
  /* 
   * Sets up AsyncWebServer and routes 
   */
  
  // La fonction serveStatic(); indique au serveur web d'envoyer les fichiers demandés 
  // par le navigateur (comme les images, les feuilles de style, les sources JavaScript, etc.) 
  // et qui sont stockés dans un système de fichiers
  // https://forum.arduino.cc/t/cherche-renseignements-sur-server-servestatic/1182775
  // => premier param la route et second param le repertoire servi (dans le LittleFS) 
  server->serveStatic("/", LittleFS, "/").setTemplateProcessor(processor);
  
  // Declaring root handler, and action (given as a lambda with "request" parameter) 
  // to be taken when root is requested
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    USE_SERIAL.printf("Root route requested !\n");
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  server->on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    USE_SERIAL.printf("GET /temperature request \n");
    request->send_P(200, "text/plain", last_temp.c_str());
  });

  server->on("/light", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", last_light.c_str());
  });

  server->on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasArg("light_threshold")) {
      int newt = atoi(request->arg("light_threshold").c_str());
      // stocker dans une variable globale si nécessaire
      request->send_P(200, "text/plain", "Threshold Set !");
    } else {
      request->send_P(400, "text/plain", "No arg");
    }
  });

  server->on("/target", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("Receive Request for a periodic report !");
    // Handled in main if you implement reporting; here we just write back index
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  server->onNotFound([](AsyncWebServerRequest *request){
    request->send(404);
  });
}

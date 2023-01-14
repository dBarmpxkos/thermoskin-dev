#ifndef FIRMWARE_ESP32_SERVER_FUNCTIONS_H
#define FIRMWARE_ESP32_SERVER_FUNCTIONS_H

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "lcd_handling.h"

/* WiFi */
extern char password[];

extern IPAddress localIP;
extern IPAddress gateway;
extern IPAddress subnet;
extern AsyncWebServer RESTServer;

extern int pwmValue;
extern String pwmValStr;
extern const int ledChannel;
extern float roomTemp;
extern float deviceTemp;
extern float resistance;
extern volatile bool wasPressed;

/* PFP */
void setup_AP(char *SSID, char *PWD,
              const IPAddress &softlocalIP, const IPAddress &softGateway, const IPAddress &softSubnet);

void not_found(AsyncWebServerRequest *request);
void increase_pwm(AsyncWebServerRequest *request);
void decrease_pwm(AsyncWebServerRequest *request);
void activate_heater(AsyncWebServerRequest *request);
void deactivate_heater(AsyncWebServerRequest *request);
void param_parser(AsyncWebServerRequest *request);
void setup_endpoints();


#endif
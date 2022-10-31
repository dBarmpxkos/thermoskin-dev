#ifndef FIRMWARE_ESP32_MAIN_H
#define FIRMWARE_ESP32_MAIN_H

/*-----------------------------------------------------------------------------------------------------------------
|   Includes
-----------------------------------------------------------------------------------------------------------------*/
#include <Arduino.h>
#include <WiFi.h>
#include "lcd_handling.h"
#include "sensor_handling.h"
#include "server_functions.h"
/*-----------------------------------------------------------------------------------------------------------------
|   Variables - Pins
-----------------------------------------------------------------------------------------------------------------*/
const char PROGMEM FW_VER[]  = "0.1";
const char PROGMEM FW_DATE[] = "01072022";
const char PROGMEM HW_VER[]  = "alpha_bread";
float targetTemp = 67.5;
float runningTemp = 0;
const unsigned int burnAfter = 2000, burnTime = 5000;
unsigned long timeMoved = 0, timeStarted = 0;
int progress = 0;
bool active = false;
#define FET_PIN 5

/* setting PWM properties */
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
int pwmValue = 0;
String pwmValStr = "0";

/*-----------------------------------------------------------------------------------------------------------------
|   Classes
-----------------------------------------------------------------------------------------------------------------*/
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MPU6050 mpu;

/*-----------------------------------------------------------------------------------------------------------------
|   PFP
-----------------------------------------------------------------------------------------------------------------*/
void  print_info();
float check_get_temp();

#endif
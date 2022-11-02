#ifndef FIRMWARE_ESP32_MAIN_H
#define FIRMWARE_ESP32_MAIN_H

/*-----------------------------------------------------------------------------------------------------------------
|   Includes
-----------------------------------------------------------------------------------------------------------------*/
#include <Arduino.h>
#include <WiFi.h>
#include "lcd_handling.h"
#include "sensor_handling.h"
#include "led_handling.h"
#include "server_functions.h"
/*-----------------------------------------------------------------------------------------------------------------
|   Variables - Pins
-----------------------------------------------------------------------------------------------------------------*/
const char PROGMEM FW_VER[]  = "0.2";
const char PROGMEM FW_DATE[] = "31102022";
const char PROGMEM HW_VER[]  = "alpha_bread";

/* Pins */
gpio_num_t ACC_INT_PIN      = GPIO_NUM_15;
gpio_num_t FET_PIN          = GPIO_NUM_5;

/* setting PWM properties */
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
int pwmValue = 0;
String pwmValStr = "0";

typedef enum {
    stateIDLE,
    stateSAMPLING,
    stateHEATING,
    stateSLEEPING,
} eSystemState;

eSystemState liveState = stateIDLE;

/*-----------------------------------------------------------------------------------------------------------------
|   Classes
-----------------------------------------------------------------------------------------------------------------*/
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MPU6050 mpu;
CRGB leds[NUM_LEDS];

hw_timer_t *breathTimer = nullptr;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
/*-----------------------------------------------------------------------------------------------------------------
|   PFP
-----------------------------------------------------------------------------------------------------------------*/
void  print_info();
void  breather(ledStatus ledState);
#endif
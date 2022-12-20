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
#include "ohm_meter.h"
/*-----------------------------------------------------------------------------------------------------------------
|   Variables - Pins
-----------------------------------------------------------------------------------------------------------------*/
const char PROGMEM FW_VER[]  = "0.5.0";
const char PROGMEM FW_DATE[] = "26112022";
const char PROGMEM HW_VER[]  = "v0.9.0";
const char PROGMEM LOGO[] = "\r\n\r\n\r\n\r\n\r\n\r\n\r\n\
 _______ __                               _______ __     __        \r\n\
|_     _|  |--.-----.----.--------.-----.|     __|  |--.|__|.-----.\r\n\
  |   | |     |  -__|   _|        |  _  ||__     |    < |  ||     |\r\n\
  |___| |__|__|_____|__| |__|__|__|_____||_______|__|__||__||__|__|\
";
/* Pins */
gpio_num_t ACC_INT_PIN      = GPIO_NUM_36;
gpio_num_t FET_PIN          = GPIO_NUM_25;
gpio_num_t P_FET_PIN        = GPIO_NUM_23;
gpio_num_t LED_PWM_PIN      = GPIO_NUM_2;

/* PWM */
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
int pwmValue = 0;
String pwmValStr = "0";
float runningTemp;
float resistance;

#define RES_SAMPLE_NUM 50

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
hw_timer_t *breathTimer = nullptr;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
/*-----------------------------------------------------------------------------------------------------------------
|   PFP
-----------------------------------------------------------------------------------------------------------------*/
void  print_info();
void  breather(ledStatus ledState);
#endif
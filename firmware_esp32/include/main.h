#ifndef FIRMWARE_ESP32_MAIN_H
#define FIRMWARE_ESP32_MAIN_H

/*-----------------------------------------------------------------------------------------------------------------
|   Includes
-----------------------------------------------------------------------------------------------------------------*/
#include <Arduino.h>
#include <WiFi.h>
#include "FastLED.h"
#include "lcd_handling.h"
#include "sensor_handling.h"
#include "server_functions.h"
#include "ohm_meter.h"

/*-----------------------------------------------------------------------------------------------------------------
|   Variables - Pins
-----------------------------------------------------------------------------------------------------------------*/
const char PROGMEM FW_VER[]  = "2.0.0";
const char PROGMEM FW_DATE[] = "13062023";
const char PROGMEM HW_VER[]  = "v2.0.0";
char serialNum[18] = {'\0'};

const char PROGMEM LOGO[] = "\r\n\r\n\r\n\r\n\r\n\r\n\r\n\
 _______ __                               _______ __     __        \r\n\
|_     _|  |--.-----.----.--------.-----.|     __|  |--.|__|.-----.\r\n\
  |   | |     |  -__|   _|        |  _  ||__     |    < |  ||     |\r\n\
  |___| |__|__|_____|__| |__|__|__|_____||_______|__|__||__||__|__|\
";
/* Pins */
gpio_num_t FET_PIN          = GPIO_NUM_25;

/* measurements */
RTC_DATA_ATTR float roomTemp;
RTC_DATA_ATTR float roomResistance;

float humidity;
float temperature;
float resistance;
float batLevel = 0;
float powLevel = 0;
#define TARGET_TEMP 75

unsigned long sampleTime = 10000000;    /* ms to sleep */
volatile bool byebye = false;
unsigned long timeTouched = 0;

typedef enum {
    sIdle = 0,
    sTouched,
    sFinished
} smHeatState;
smHeatState smCtrl = sTouched;

/* touch */
int   touchCaliSize = 1000;
volatile bool touchDetect = false;
RTC_DATA_ATTR unsigned int touchCounter;

/* LEDS */
#define LED_PIN     GPIO_NUM_2
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS   1

#define HEAT_COL CRGB(10, 5, 0)
#define WOKE_COL CRGB(250, 240, 250)
#define DONE_COL CRGB(27, 100, 0)
#define SLEEP_COL CRGB(111, 3, 168)
CRGB leds[NUM_LEDS];

/*-----------------------------------------------------------------------------------------------------------------
|   Classes
-----------------------------------------------------------------------------------------------------------------*/
hw_timer_t *sleepTimer = nullptr;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
/*-----------------------------------------------------------------------------------------------------------------
|   PFP
-----------------------------------------------------------------------------------------------------------------*/
void print_info();
void init_gpio_tim();
void get_serial_num(char *serNumOut);
int lazy_median(int arr[], uint8_t size);
#endif
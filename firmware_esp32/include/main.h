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
#include <QuickPID.h>

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
gpio_num_t P_FET_PIN        = GPIO_NUM_23;
gpio_num_t LED_PWM_PIN      = GPIO_NUM_2;

const int heatFreq = 5000;
const int heatChannel = 1;
const int heatResolution = 8;
int pwmValue = 0;
String pwmValStr = "0";

volatile bool sampler = false;
volatile bool hotPicker = false;
unsigned long hotTicker = 0;

float humidity;
float temperature;
float resistance;
float roomTemp = 19.00;
float roomResistance = 2.45;
int progressBarStatus = 0;
//#define TCR 0.00369

volatile bool lowHeat = false, medHeat = false, highHeat = false;

typedef enum {
    dummyCtrl = 0,
    sliderCtrl,
    PIDCtrl
} eCtrlMode;
eCtrlMode activeCtrl = PIDCtrl;

float pwmToHeat = 0;
unsigned long sampleTime = 2000000;

#define RES_SAMPLES_LARGE 20
#define RES_SAMPLES_MED   5
#define RES_SAMPLES_SMALL 1

/*-----------------------------------------------------------------------------------------------------------------
|   Classes
-----------------------------------------------------------------------------------------------------------------*/
hw_timer_t *senseTimer = nullptr;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
/*-----------------------------------------------------------------------------------------------------------------
|   PFP
-----------------------------------------------------------------------------------------------------------------*/
void print_info();
void init_gpio_tim();
void get_serial_num(char *serNumOut);
#endif
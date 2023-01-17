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
const char PROGMEM FW_VER[]  = "0.5.0";
const char PROGMEM FW_DATE[] = "26112022";
const char PROGMEM HW_VER[]  = "v0.9.0";
char serialNum[18] = {'\0'};

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

const int heatFreq = 5000;
const int heatChannel = 1;
const int heatResolution = 8;
int pwmValue = 0;
String pwmValStr = "0";

float temperature;
float resistance;
volatile bool lowHeat = false, medHeat = false, highHeat = false;

#define RES_SAMPLE_NUM 50

typedef enum {
    stateIDLE,
    stateSAMPLING,
    stateHEATING,
    stateSLEEPING,
} eSystemState;
eSystemState liveState = stateIDLE;

typedef enum {
    dummyCtrl = 0,
    sliderCtrl,
    PIDCtrl
} eCtrlMode;
eCtrlMode activeCtrl = dummyCtrl;


/* PID Controller */
float tempSetpointLow = 22.2, tempSetpointMid = 65, tempSetpointHigh = 80;
float Kp = 2, Ki = 5, Kd = 1;
float pwmToHeat = 0;
/*-----------------------------------------------------------------------------------------------------------------
|   Classes
-----------------------------------------------------------------------------------------------------------------*/
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
hw_timer_t *breathTimer = nullptr;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
QuickPID heaterPID_Low(&temperature, &pwmToHeat, &tempSetpointLow);
QuickPID heaterPID_Mid(&temperature, &pwmToHeat, &tempSetpointMid);
QuickPID heaterPID_High(&temperature, &pwmToHeat, &tempSetpointHigh);

/*-----------------------------------------------------------------------------------------------------------------
|   PFP
-----------------------------------------------------------------------------------------------------------------*/
void print_info();
void gpio_init();
void breather(ledStatus ledState);
void get_serial_num(char *serNumOut);
void dummy_ctrl();
void slider_ctrl();
void PID_ctrl();
#endif
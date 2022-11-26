#ifndef FIRMWARE_ESP32_LED_HANDLING_H
#define FIRMWARE_ESP32_LED_HANDLING_H

#ifdef CHIP_LED
#include <FastLED.h>

#define LED_PIN     32
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    1
#endif

volatile int brightness = 20;
volatile int fadeAmount          = 2;
uint8_t floorz = 4;
uint8_t ceilingz = 160;

uint8_t floorHeat = 120;
uint8_t ceilingHeat = 200;
#define FRAMES_PER_SECOND 5

typedef enum {
    ledOff = 0,
    ledBreath,
    ledSample,
    ledHeat,
} ledStatus;
ledStatus ledState = ledOff;


#endif //FIRMWARE_ESP32_LED_HANDLING_H

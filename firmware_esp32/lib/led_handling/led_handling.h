#ifndef FIRMWARE_ESP32_LED_HANDLING_H
#define FIRMWARE_ESP32_LED_HANDLING_H

#include <FastLED.h>

#define LED_PIN     32
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    1
volatile unsigned int brightness = 20;
volatile int fadeAmount          = 2;
uint8_t floorz = 4;
uint8_t ceilingz = 160;
#define FRAMES_PER_SECOND 5

typedef enum {
    ledOff = 0,
    ledBreath,
    ledAlarm,
} ledStatus;
ledStatus ledState = ledOff;


#endif //FIRMWARE_ESP32_LED_HANDLING_H

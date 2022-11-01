#ifndef FIRMWARE_ESP32_LED_HANDLING_H
#define FIRMWARE_ESP32_LED_HANDLING_H

#include <FastLED.h>

#define DIR_UP 1
#define DIR_DOWN 0
uint8_t dir = DIR_UP;

#define LED_PIN     32
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    1
uint8_t brightness = 10;
uint8_t floorz = 10;
uint8_t ceilingz = 70;
#define FRAMES_PER_SECOND 5


#endif //FIRMWARE_ESP32_LED_HANDLING_H

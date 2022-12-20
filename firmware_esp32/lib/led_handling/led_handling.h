#ifndef FIRMWARE_ESP32_LED_HANDLING_H
#define FIRMWARE_ESP32_LED_HANDLING_H

volatile int brightness = 20;
volatile int fadeAmount          = 2;
uint8_t floorz = 4;
uint8_t ceilingz = 160;

uint8_t floorHeat = 120;
uint8_t ceilingHeat = 200;

typedef enum {
    ledOff = 0,
    ledBreath,
    ledSample,
    ledHeat,
} ledStatus;
ledStatus ledState = ledOff;


#endif //FIRMWARE_ESP32_LED_HANDLING_H

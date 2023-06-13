#ifndef FIRMWARE_ESP32_OHM_METER_H
#define FIRMWARE_ESP32_OHM_METER_H

#include <Adafruit_INA219.h>

extern float temperature;
extern float resistance;
extern float roomTemp;
extern float roomResistance;
#define TCR 0.000353

double lazy_median(float arr[], uint8_t size);
bool initialize_ina219();
float get_resistance(int M);
double calculate_temp(float res);

#endif //FIRMWARE_ESP32_OHM_METER_H

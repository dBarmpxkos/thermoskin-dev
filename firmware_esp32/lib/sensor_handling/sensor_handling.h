#ifndef FIRMWARE_ESP32_SENSOR_HANDLING_H
#define FIRMWARE_ESP32_SENSOR_HANDLING_H

#include <Adafruit_SHTC3.h>

bool initialize_sensor();
void get_measurements(float &temperature, float &humidity);

#endif //FIRMWARE_ESP32_SENSOR_HANDLING_H

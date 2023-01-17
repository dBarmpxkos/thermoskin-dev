#ifndef FIRMWARE_ESP32_SENSOR_HANDLING_H
#define FIRMWARE_ESP32_SENSOR_HANDLING_H

#include <DallasTemperature.h>

const int oneWireBus = 32;
#define THRESHOLD_TOUCH 50

void init_mpu();
bool init_temp_sensor();

#endif //FIRMWARE_ESP32_SENSOR_HANDLING_H

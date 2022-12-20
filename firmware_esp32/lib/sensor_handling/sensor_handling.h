#ifndef FIRMWARE_ESP32_SENSOR_HANDLING_H
#define FIRMWARE_ESP32_SENSOR_HANDLING_H

#include <DallasTemperature.h>
#include <Adafruit_MPU6050.h>

const int oneWireBus = 32;
#define THRESHOLD_TOUCH 50

void init_mpu();
bool init_temp_sensor();
float get_temp();

#endif //FIRMWARE_ESP32_SENSOR_HANDLING_H

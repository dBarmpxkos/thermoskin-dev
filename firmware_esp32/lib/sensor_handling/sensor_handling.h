#ifndef FIRMWARE_ESP32_SENSOR_HANDLING_H
#define FIRMWARE_ESP32_SENSOR_HANDLING_H

#include <DallasTemperature.h>
#include <Adafruit_MPU6050.h>

const int oneWireBus = 16;

void init_mpu();
void init_temp_sensor();


#endif //FIRMWARE_ESP32_SENSOR_HANDLING_H

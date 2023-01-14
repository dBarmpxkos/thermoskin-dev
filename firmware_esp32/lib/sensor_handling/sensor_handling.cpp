#include "lcd_handling.h"
#include "sensor_handling.h"

extern Adafruit_MPU6050 mpu;
extern Adafruit_SSD1306 display;
extern DallasTemperature sensors;

void
init_mpu() {
    if         (!mpu.begin()) Serial.printf("\r\n[MPU]\tFailed to find MPU6050 chip");
    else       Serial.printf("\r\n[MPU]\tMPU6050 Init OK");
    mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
    mpu.setMotionDetectionThreshold(5);
    mpu.setMotionDetectionDuration(20);
    mpu.setInterruptPinLatch(true);
    mpu.setInterruptPinPolarity(true);
    mpu.setMotionInterrupt(true);
}

bool
init_temp_sensor(){
    sensors.begin();
    if (!sensors.getDeviceCount()) {
        return false;
    }
    else {
        Serial.printf("\r\n[TEMP]\tDS18b20 Init OK");
        return true;
    }
}

float
get_temp(){
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}
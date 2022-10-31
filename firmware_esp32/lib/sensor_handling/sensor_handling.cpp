#include "lcd_handling.h"
#include "sensor_handling.h"

extern Adafruit_MPU6050 mpu;
extern Adafruit_SSD1306 display;
extern DallasTemperature sensors;

void
init_mpu() {
    if (!mpu.begin()) Serial.println("Failed to find MPU6050 chip");
    else{
        Serial.printf("\r\n[MPU]\tMPU6050 Init OK");
//        display.setCursor(0,0);
//        display.printf("MPU6050 Init OK");
//        display.display();
//        delay(500);
    }

    mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
    mpu.setMotionDetectionThreshold(5);
    mpu.setMotionDetectionDuration(20);
    mpu.setInterruptPinLatch(true);
    mpu.setInterruptPinPolarity(true);
    mpu.setMotionInterrupt(true);
}

void
init_temp_sensor(){
    sensors.begin();
    if (!sensors.getDeviceCount()) Serial.println("Failed to find temp sensor");
    else {
        Serial.printf("\r\n[TEMP]\tDS18b20 Init OK");
        display.setCursor(0,0);
        display.printf("\r\nDS18b20 Init OK");
        display.display();
        delay(500);
    }

}
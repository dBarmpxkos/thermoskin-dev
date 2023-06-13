#include <Arduino.h>
#include "ohm_meter.h"

Adafruit_INA219 ina219;

double
lazy_median(float arr[], uint8_t size) {
    std::sort(arr, arr + size);
    if (size % 2)  return arr[size / 2];
    else           return (arr[(size - 1) / 2] + arr[size / 2]) / 2.0;
}

bool
initialize_ina219(){
    if (!ina219.begin()) {
        Serial.printf("\r\n[SENS]\tProblem with SHTC3 sensor");
        while (1) delay(1);
    } else return true;
}

float
get_resistance(int M) {
    float shuntvoltage = 0;
    float busvoltage = 0;
    float current_mA = 0;
    float loadvoltage = 0;
    float power_mW = 0;
    float resArray[M];

    for (int i=0; i<M; i++){
        shuntvoltage = ina219.getShuntVoltage_mV();
        busvoltage = ina219.getBusVoltage_V();
        current_mA = ina219.getCurrent_mA();
        power_mW = ina219.getPower_mW();
        loadvoltage = busvoltage + (shuntvoltage / 1000);
        resArray[i] = loadvoltage/current_mA;
    }
    return (float)lazy_median(resArray, M);
}

double
calculate_temp(float res){
    /* R = R0 ( 1 + αΔT ) */
    return ((res - roomResistance)/(TCR*roomResistance)) + roomTemp;
}

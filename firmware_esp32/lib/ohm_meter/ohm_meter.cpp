#include <Arduino.h>
#include "ohm_meter.h"

Adafruit_INA219 ina219;

double
lazy_median_float(float arr[], uint8_t size) {
    std::sort(arr, arr + size);
    if (size % 2)  return arr[size / 2];
    else           return (arr[(size - 1) / 2] + arr[size / 2]) / 2.0;
}

bool
initialize_ina219(){
    if (!ina219.begin()) {
        Serial.printf("\r\n[SENS]\tProblem with INA sensor");
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
    float resOut = 0;

    for (int i=0; i<M; i++){
        shuntvoltage = ina219.getShuntVoltage_mV();
        busvoltage = ina219.getBusVoltage_V();
        current_mA = ina219.getCurrent_mA()*10      /* change for our shunt resistor */;
        loadvoltage = busvoltage + (shuntvoltage / 1000);
        power_mW = loadvoltage * current_mA;
        resArray[i] = loadvoltage/ (current_mA/1000);
    }
    resOut = (float)lazy_median_float(resArray, M);
//    Serial.printf("\r\n[INA]\tshuntV: %.4f V\r\n\tbusV: %.4f V\r\n\tloadV: %.4f V\r\n\tI: %.4f mA\r\n\tP: %.4f mW\r\n\tres: %.3f",
//                  shuntvoltage, busvoltage, loadvoltage, current_mA, power_mW, resOut);
    batLevel = loadvoltage;
    powLevel = power_mW;
    return resOut;
}

double
calculate_temp(float res){
    /* R = R0 ( 1 + αΔT ) */
    return ((res - roomResistance)/(TCR*roomResistance)) + roomTemp;
}

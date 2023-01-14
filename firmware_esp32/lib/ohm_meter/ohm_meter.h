
#ifndef FIRMWARE_ESP32_OHM_METER_H
#define FIRMWARE_ESP32_OHM_METER_H

#include <Wire.h>

//  ADS1115 mux and gain settings
#define  ADS1115_CH01  0x00    // p = AIN0, n = AIN1
#define  ADS1115_CH03  0x01    // ... etc
#define  ADS1115_CH13  0x02
#define  ADS1115_CH23  0x03
#define  ADS1115_CH0G  0x04    // p = AIN0, n = GND
#define  ADS1115_CH1G  0x05    // ... etc
#define  ADS1115_CH2G  0x06
#define  ADS1115_CH3G  0x07

#define  ADS1115_6p144  0x00   // +/- 6.144 V full scale
#define  ADS1115_4p096  0x01   // +/- 4.096 V full scale
#define  ADS1115_2p048  0x02   // +/- 2.048 V full scale
#define  ADS1115_1p024  0x03   // +/- 1.024 V full scale
#define  ADS1115_0p512  0x04   // +/- 0.512 V full scale
#define  ADS1115_0p256  0x05   // +/- 0.256 V full scale
#define  ADS1115_0p256B 0x06   // same as ADS1115_0p256
#define  ADS1115_0p256C 0x07   // same as ADS1115_0p256

#define ROOM_TEMP   19
#define ROOM_RES    2.4513
#define TCR         0.00369
float ADS1115_read(byte channel, byte gain);
void drive_P();
void drive_N();
void drive_on();
void drive_off();
int calc_PGA(float sampleVoltage);
float get_resistance(int M);
float drive_measure_res(int M);
double calculate_temp(float res);

#endif //FIRMWARE_ESP32_OHM_METER_H

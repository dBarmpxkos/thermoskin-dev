#include <Arduino.h>
#include "ohm_meter.h"

extern gpio_num_t FET_PIN;
extern gpio_num_t P_FET_PIN;

gpio_num_t DRIVA1      = GPIO_NUM_19;
gpio_num_t DRIVA2      = GPIO_NUM_18;
gpio_num_t DRIVA3      = GPIO_NUM_17;
gpio_num_t DRIVA4      = GPIO_NUM_16;

gpio_num_t DRIVB1      = GPIO_NUM_26;
gpio_num_t DRIVB2      = GPIO_NUM_27;
gpio_num_t DRIVB3      = GPIO_NUM_14;
gpio_num_t DRIVB4      = GPIO_NUM_4;

/*
  reads a single sample from the ADS1115 ADC at a given mux (channel) and gain setting
    - channel is 3 bit channel number/mux setting (one of ADS1115_CHxx)
    - gain is 3 bit PGA gain setting (one of ADS1115_xpxxx)
    - returns voltage in volts
    - uses single shot mode, polling for conversion complete, default I2C address
    - conversion takes approximatly 9.25 msec
*/
float
ADS1115_read(byte channel, byte gain){
    const int    address = 0x48;                        // ADS1115 I2C address, A0=0, A1=0
    byte         hiByte, loByte;
    int          r;
    float x = 0;

    channel &= 0x07;                                    // constrain to 3 bits
    gain    &= 0x07;

    hiByte = B10000001 | (channel << 4) | (gain << 1); // conversion start command
    loByte = B10000011;

    Wire.beginTransmission(address);                    // send conversion start command
    Wire.write(0x01);                                   // address the config register
    Wire.write(hiByte);                                 // ...and send config register value
    Wire.write(loByte);
    Wire.endTransmission();

    do                                                  // loop until conversion complete
    {
        Wire.requestFrom(address, 2);                   // config register is still addressed
        while (Wire.available())
        {
            hiByte = Wire.read();                       // ... and read config register
            loByte = Wire.read();
        }
    }
    while ((hiByte & 0x80) == 0);                       // upper bit (OS) is conversion complete

    Wire.beginTransmission(address);
    Wire.write(0x00);                                   // address the conversion register
    Wire.endTransmission();

    Wire.requestFrom(address, 2);                       // ... and get 2 byte result
    while (Wire.available())
    {
        hiByte = Wire.read();
        loByte = Wire.read();
    }

    r = loByte | hiByte << 8;                           // convert to 16 bit int
    switch (gain)                                       // ... and now convert to volts
    {
        case ADS1115_6p144:  x = r * 6.144 / 32768.0; break;
        case ADS1115_4p096:  x = r * 4.096 / 32768.0; break;
        case ADS1115_2p048:  x = r * 2.048 / 32768.0; break;
        case ADS1115_1p024:  x = r * 1.024 / 32768.0; break;
        case ADS1115_0p512:  x = r * 0.512 / 32768.0; break;
        case ADS1115_0p256:
        case ADS1115_0p256B:
        case ADS1115_0p256C: x = r * 0.256 / 32768.0; break;
        default: break;
    }
    return x;
}

void
drive_P(){
    drive_off();
    digitalWrite( DRIVA1, HIGH);
    digitalWrite( DRIVA2, HIGH);
    digitalWrite( DRIVA3, HIGH);
    digitalWrite( DRIVA4, HIGH);

    digitalWrite( DRIVB1, LOW);
    digitalWrite( DRIVB2, LOW);
    digitalWrite( DRIVB3, LOW);
    digitalWrite( DRIVB4, LOW);
    drive_on();
}

void
drive_N(){
    drive_off();
    digitalWrite( DRIVA1, LOW);
    digitalWrite( DRIVA2, LOW);
    digitalWrite( DRIVA3, LOW);
    digitalWrite( DRIVA4, LOW);

    digitalWrite( DRIVB1, HIGH);
    digitalWrite( DRIVB2, HIGH);
    digitalWrite( DRIVB3, HIGH);
    digitalWrite( DRIVB4, HIGH);
    drive_on();
}

void
drive_on(){
    pinMode( DRIVA1, OUTPUT);      // enable source/sink in pairs
    pinMode( DRIVA2, OUTPUT);
    pinMode( DRIVA3, OUTPUT);
    pinMode( DRIVA4, OUTPUT);

    pinMode( DRIVB1, OUTPUT);
    pinMode( DRIVB2, OUTPUT);
    pinMode( DRIVB3, OUTPUT);
    pinMode( DRIVB4, OUTPUT);
    delayMicroseconds(5000);  // 5ms delay
}

void
drive_off(){
    pinMode( DRIVA1, INPUT);      // DISABLE source/sink in pairs
    pinMode( DRIVA2, INPUT);
    pinMode( DRIVA3, INPUT);
    pinMode( DRIVA4, INPUT);

    pinMode( DRIVB1, INPUT);
    pinMode( DRIVB2, INPUT);
    pinMode( DRIVB3, INPUT);
    pinMode( DRIVB4, INPUT);
}

int
calc_PGA(float sampleVoltage){
    sampleVoltage = abs(sampleVoltage);
    if (sampleVoltage > 3.680) return ADS1115_6p144;
    if (sampleVoltage > 1.840) return ADS1115_4p096;
    if (sampleVoltage > 0.920) return ADS1115_2p048;
    if (sampleVoltage > 0.460) return ADS1115_1p024;
    if (sampleVoltage > 0.230) return ADS1115_0p512;
    else                       return ADS1115_0p256;
}

float
get_resistance(int M) {
    const float      Rr = 10.10;            // reference resistor value, ohms
    const float      Rcal = 1.002419;       // calibration factor
    const int        N = 2;                 // number of cycles to average
//    const int        M = 50;                // samples per half cycle
    double           Rx;                    // calculated resistor under test, ohms
    byte             PGAr, PGAx;            // PGA gains (r = reference, x = test resistors)
    float            Vr, Vx, Wx, Wr;        // voltages in V
    float            Rn;                    // calculated resistor under test, ohms, single sample
    double           Avgr, Avgx;            // average ADC readings in mV
    int              j, k, n;

    drive_P();
    Wr =  ADS1115_read(ADS1115_CH01, ADS1115_6p144);
    Wx =  ADS1115_read(ADS1115_CH23, ADS1115_6p144);
    drive_N();
    Vr = -ADS1115_read(ADS1115_CH01, ADS1115_6p144);
    Vx = -ADS1115_read(ADS1115_CH23, ADS1115_6p144);
    drive_off();

    PGAr = calc_PGA(max(Vr, Wr));
    PGAx = calc_PGA(max(Vx, Wx));

    // measure resistance using synchronous detection
    Avgr = Avgx = 0.0;
    Rx = 0.0;
    n = 0;
    for (j = 0; j < N; j++) {
        drive_P();
        for (k = 0; k < M; k++) {
            Vx = ADS1115_read(ADS1115_CH23, PGAx);
            Vr = ADS1115_read(ADS1115_CH01, PGAr);
            Avgx += Vx;
            Avgr += Vr;
            Rn = Vx / Vr;
            if (Rn > 0.0 && Rn < 10000.0) {
                Rx += Rn;
                n++;
            }
        }

        drive_N();
        for (k = 0; k < M; k++) {
            Vx = ADS1115_read(ADS1115_CH23, PGAx);
            Vr = ADS1115_read(ADS1115_CH01, PGAr);
            Avgx -= Vx;
            Avgr -= Vr;
            Rn = Vx / Vr;
            if (Rn > 0.0 && Rn < 10000.0){
                Rx += Rn;
                n++;
            }
        }
    }

    drive_off();
    Rx   *= Rr * Rcal / n;                 // apply calibration factor and compute average
    Avgr *= 1000.0 / (2.0 * N * M);        // average in mV
    Avgx *= 1000.0 / (2.0 * N * M);
    Serial.printf("\r\n[Rx]\t%.5f", Rx);

    return Rx;
}

float
drive_measure_res(int M){
    digitalWrite(FET_PIN, LOW);         /*  make sure we switch heater off before connecting ADS1115 */
    digitalWrite(P_FET_PIN, LOW);
    return get_resistance(M);

}

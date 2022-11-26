#include "main.h"

void
IRAM_ATTR ticker() {
    portENTER_CRITICAL_ISR(&timerMux);

#ifndef CHIP_LED
    ledcWrite(ledChannel, brightness);
#endif
    brightness = brightness + fadeAmount;
    if (liveState != stateHEATING) {
        if (brightness <= floorz || brightness >= ceilingz)
            fadeAmount = -fadeAmount;
    } else if (liveState == stateHEATING) {
        if (brightness <= floorHeat || brightness >= ceilingHeat)
            fadeAmount = -fadeAmount;
    }
    portEXIT_CRITICAL_ISR(&timerMux);
}

/*-----------------------------------------------------------------------------------------------------------------
|   Main
-----------------------------------------------------------------------------------------------------------------*/
void
setup() {
    liveState = stateIDLE;

    Serial.begin(115200);
#ifdef CHIP_LED
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(brightness);
#else
    pinMode(LED_PWM_PIN, OUTPUT);
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(LED_PWM_PIN, ledChannel);
#endif

    Serial.printf("\r\n[LED]\tLED OK");

//    setup_AP(ssid, password, localIP, gateway, subnet);
//    setup_endpoints();
    pinMode(ACC_INT_PIN, INPUT_PULLUP);
    print_info();
    init_mpu();

    breather(ledBreath);

}

void
loop() {
#ifdef CHIP_LED
    FastLED.setBrightness(brightness);
    FastLED.show();
#else
    ledcWrite(ledChannel, brightness);
#endif
    int k = 0;

    if (!digitalRead(ACC_INT_PIN)) {
        Serial.printf("\r\n[SM]\tState: SAMPLING");
        liveState = stateSAMPLING;
        breather(ledSample);
    }

    if (liveState == stateHEATING) {
        Serial.printf("\r\n[SM]\tState: HEATING");
        breather(ledHeat);
    }

    if (liveState == stateSLEEPING) {
        Serial.printf("\r\n[SM]\tGracefully closing everything...");

        for (int i = brightness; i >= 0; i--){
#ifdef CHIP_LED
            FastLED.setBrightness(brightness);
            FastLED.show();
#else
            ledcWrite(ledChannel, brightness);
#endif
            Serial.printf("\r\n[SM]\tBrightness: %d", i);
            delay(40);
        }
#ifdef CHIP_LED
        FastLED.clear();
        FastLED.show();
#else
        ledcWrite(ledChannel, 0);
#endif
        Serial.printf("\r\n[SM]\tGoing to sleep");

        esp_sleep_enable_ext0_wakeup(ACC_INT_PIN, 0);
        esp_deep_sleep_start();
    }


    while (liveState == stateHEATING) {
        Serial.printf("\r\n[HEAT]\tOperating FET");
#ifdef CHIP_LED
        FastLED.setBrightness(brightness);
            FastLED.show();
#else
        ledcWrite(ledChannel, brightness);
#endif
        if (mpu.getMotionInterruptStatus()) {
            Serial.printf("\r\n[HEAT]\tClosing FET, moved while ON");
            liveState = stateSAMPLING;
            breather(ledSample);
            break;
        }

        if (millis() - heatTimekeep >= timeToStayHot) {
            Serial.printf("\r\n[HEAT]\tDone, exiting heat state");
            liveState = stateSLEEPING;
        }
    }

    while (liveState == stateSAMPLING) {
        if (mpu.getMotionInterruptStatus()) {    /* new interrupt? restart */
            Serial.printf("\r\n[SAMPLE]\tMoved again during sampling");
            k = 0;
        }
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        k++;
        if (k == 2000) {
            liveState = stateHEATING;
            heatTimekeep = millis();
            break;
        }
#ifdef CHIP_LED
        FastLED.setBrightness(brightness);
            FastLED.show();
#else
        ledcWrite(ledChannel, brightness);
#endif
    }

}

/*-----------------------------------------------------------------------------------------------------------------
|   Function Sources
-----------------------------------------------------------------------------------------------------------------*/
void
print_info() {
    char macAddr[20] = {'\0'};
    sprintf(macAddr, "%s", WiFi.macAddress().c_str());
    Serial.printf("\r\n\r\n\r\n\r\nSerial number: %s\r\nFirmware Version: %s:%s\r\nHardware Version: %s\r\n",
                  macAddr, FW_VER, FW_DATE, HW_VER);

}

void
breather(ledStatus ledState) {
    if (ledState == ledBreath) {
        breathTimer = timerBegin(0, 40, true);
        timerAttachInterrupt(breathTimer, &ticker, true);
        timerAlarmWrite(breathTimer, 100000, true);
        timerAlarmEnable(breathTimer);
#ifdef CHIP_LED
        leds[0] = CRGB(0xFF9329);
#endif
    } else if (ledState == ledSample) {
        breathTimer = timerBegin(0, 40, true);
        timerAttachInterrupt(breathTimer, &ticker, true);
        timerAlarmWrite(breathTimer, 10000, true);
        timerAlarmEnable(breathTimer);
#ifdef CHIP_LED
        leds[0] = CRGB(0x8B2DC2);
#endif
    } else if (ledState == ledHeat) {
        breathTimer = timerBegin(0, 40, true);
        timerAttachInterrupt(breathTimer, &ticker, true);
        timerAlarmWrite(breathTimer, 40000, true);
        timerAlarmEnable(breathTimer);
#ifdef CHIP_LED
        leds[0] = CRGB(0xFF0000);
#endif
    } else {
        timerAlarmDisable(breathTimer);
        brightness = 0;
    }
}




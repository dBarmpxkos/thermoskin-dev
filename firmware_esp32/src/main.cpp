#include "main.h"

void
IRAM_ATTR ticker() {
    portENTER_CRITICAL_ISR(&timerMux);

    brightness = brightness + fadeAmount;
    if (brightness <= floorz || brightness >= ceilingz)
        fadeAmount = -fadeAmount;


    portEXIT_CRITICAL_ISR(&timerMux);
}

/*-----------------------------------------------------------------------------------------------------------------
|   Main
-----------------------------------------------------------------------------------------------------------------*/
void
setup() {
    liveState = stateIDLE;

    Serial.begin(115200);
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(brightness);
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
    FastLED.setBrightness(brightness);
    FastLED.show();
    int k = 0;

    if (!digitalRead(ACC_INT_PIN)) {
        Serial.printf("\r\n[SM]\tState: SAMPLING");
        liveState = stateSAMPLING;
        breather(ledAlarm);
    }

    if (liveState == stateIDLE) {
        Serial.printf("\r\n[SM]\tState: IDLE");
    }

    while (liveState == stateSAMPLING) {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        FastLED.setBrightness(brightness);
        FastLED.show();
        k++;
        if (k == 2000) {
            mpu.getMotionInterruptStatus(); /* this clears the interrupt and goes to sleep */
            breather(ledBreath);
            liveState = stateIDLE;
        }
    }

//    Serial.printf("\r\n[SLEEP]\tGoing to sleep");
//    esp_sleep_enable_ext0_wakeup(ACC_INT_PIN, 0);
//    esp_deep_sleep_start();


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
        leds[0] = CRGB(0xFF9329);
    } else if (ledState == ledAlarm) {
        breathTimer = timerBegin(0, 40, true);
        timerAttachInterrupt(breathTimer, &ticker, true);
        timerAlarmWrite(breathTimer, 10000, true);
        timerAlarmEnable(breathTimer);
        leds[0] = CRGB(0x8B2DC2);
    } else {
        timerAlarmDisable(breathTimer);
        brightness = 0;
    }
}




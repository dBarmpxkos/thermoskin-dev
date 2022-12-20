#include "main.h"

void
IRAM_ATTR led_ticker() {
    portENTER_CRITICAL_ISR(&timerMux);
    ledcWrite(ledChannel, brightness);
    brightness = brightness + fadeAmount;
    if (liveState != stateHEATING) {
        if (brightness <= floorz || brightness >= ceilingz)
            fadeAmount = -fadeAmount;
    } else {
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
    drive_off();
    Wire.begin();
    Serial.begin(115200);
    print_info();

    pinMode(LED_PWM_PIN, OUTPUT);
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(LED_PWM_PIN, ledChannel);

    pinMode(FET_PIN, OUTPUT);
    digitalWrite(FET_PIN, LOW);
    pinMode(P_FET_PIN, OUTPUT);
    digitalWrite(P_FET_PIN, LOW);

    while (!init_temp_sensor()) { /* waits */ }

    setup_AP(ssid, password, localIP, gateway, subnet);
    setup_endpoints();
    breather(ledBreath);
    liveState = stateIDLE;

//    while(1){
//
//        digitalWrite(LED_PWM_PIN, LOW);
//        Serial.printf("\r\n[Rx]\tDisconnecting heater driver to measure resistance");
//        digitalWrite(FET_PIN, LOW);
//        digitalWrite(P_FET_PIN, LOW);
//
//        Serial.printf("\r\nRh, %.5f, Text, %.2f", get_resistance(50), get_temp());
//        delay(200);
//        Serial.printf("\r\n[Rx]\tConnect Vbat");
//        digitalWrite(LED_PWM_PIN, HIGH);
//        Serial.printf("\r\n[Rx]\tFIRE!!!!");
//        digitalWrite(FET_PIN, HIGH);
//        digitalWrite(P_FET_PIN, HIGH);
//        delay(5000);
//        Serial.printf("\r\n[Rx]\tAre we dead yet? :)");
//        digitalWrite(FET_PIN, LOW);
//        digitalWrite(P_FET_PIN, LOW);
//
//        delay(200);
//
//    }

}

void
loop() {

    Serial.printf("\r\n[PWM]\tManual PWM value: %d", pwmValStr.toInt());

    runningTemp = get_temp();
    Serial.printf("\r\n[TEMP]\tAmbient temperature: %.3f", runningTemp);

    resistance = drive_measure_res(RES_SAMPLE_NUM);
    Serial.printf("\r\n[RESx]\tResistance: %.4f", resistance);
    delay(500);

}

/*-----------------------------------------------------------------------------------------------------------------
|   Function Sources
-----------------------------------------------------------------------------------------------------------------*/
void
print_info() {
    char macAddr[20] = {'\0'};
    Serial.print(LOGO);
    sprintf(macAddr, "%s", WiFi.macAddress().c_str());
    Serial.printf("\r\nSerial number: %s\r\nFirmware Version: %s:%s\r\nHardware Version: %s\r\n",
                  macAddr, FW_VER, FW_DATE, HW_VER);

}

void
breather(ledStatus ledState) {
    if (ledState == ledBreath) {
        breathTimer = timerBegin(0, 40, true);
        timerAttachInterrupt(breathTimer, &led_ticker, true);
        timerAlarmWrite(breathTimer, 100000, true);
        timerAlarmEnable(breathTimer);

    } else if (ledState == ledSample) {
        breathTimer = timerBegin(0, 40, true);
        timerAttachInterrupt(breathTimer, &led_ticker, true);
        timerAlarmWrite(breathTimer, 10000, true);
        timerAlarmEnable(breathTimer);

    } else if (ledState == ledHeat) {
        breathTimer = timerBegin(0, 40, true);
        timerAttachInterrupt(breathTimer, &led_ticker, true);
        timerAlarmWrite(breathTimer, 40000, true);
        timerAlarmEnable(breathTimer);

    } else {
        timerAlarmDisable(breathTimer);
        brightness = 0;
    }
}




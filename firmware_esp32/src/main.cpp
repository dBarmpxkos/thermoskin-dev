#include "main.h"

/*
 *  Flash green led slow
 *  Polling / WiFi on
 *  heater on -> red led on 4 seconds
 *  When heater on, measure initial resistance, temperature
 *  Room temperature, heater temperature, times activated
 */

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
    char ssid[40] = {'\0'};
    char ssidPrefix[] = "thermoSkin";
    get_serial_num(serialNum);
    sprintf(ssid, "%s-%s", ssidPrefix, serialNum);
    setup_AP(ssid, password, localIP, gateway, subnet);
    setup_endpoints();

    drive_off();
    Wire.begin();
    Serial.begin(115200);
    print_info();

    pinMode(FET_PIN, OUTPUT);
    pinMode(P_FET_PIN, OUTPUT);
    digitalWrite(P_FET_PIN, LOW);

    pinMode(LED_PWM_PIN, OUTPUT);
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(LED_PWM_PIN, ledChannel);

    ledcSetup(heatChannel, heatFreq, heatResolution);
    ledcAttachPin(FET_PIN, heatChannel);

    while (!init_temp_sensor()) { /* waits */ }

    breather(ledBreath);
    liveState = stateIDLE;
    resistance = drive_measure_res(1);
}

void
loop() {

    roomTemp = get_temp();
    if (wasPressed){
        breather(ledHeat);

        unsigned long timePressed = millis();
        unsigned char ticker = 0;

        digitalWrite(P_FET_PIN, HIGH);
        ledcWrite(heatChannel, 255);

        do {
            delay(200);
        } while (millis() - timePressed < 3500);


        digitalWrite(P_FET_PIN, LOW);
        ledcWrite(heatChannel, 0);
        breather(ledBreath);

        wasPressed = false;

    }
//    int pwmToHeat = pwmValStr.toInt();
//    if (pwmToHeat){
//        digitalWrite(P_FET_PIN, HIGH);
//        ledcWrite(heatChannel, pwmToHeat);
//        ledcWrite(ledChannel, pwmToHeat);
//    } else {
//        digitalWrite(P_FET_PIN, LOW);
//        ledcWrite(heatChannel, 0);
//        ledcWrite(ledChannel, 0);
//        resistance = drive_measure_res(1);
//        deviceTemp = calculate_temp(resistance);
//    }


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
        timerAlarmWrite(breathTimer, 40000, true);
        timerAlarmEnable(breathTimer);

    } else if (ledState == ledSample) {
        breathTimer = timerBegin(0, 40, true);
        timerAttachInterrupt(breathTimer, &led_ticker, true);
        timerAlarmWrite(breathTimer, 10000, true);
        timerAlarmEnable(breathTimer);

    } else if (ledState == ledHeat) {
        breathTimer = timerBegin(0, 40, true);
        timerAttachInterrupt(breathTimer, &led_ticker, true);
        timerAlarmWrite(breathTimer, 1000, true);
        timerAlarmEnable(breathTimer);

    } else {
        timerAlarmDisable(breathTimer);
        brightness = 0;
    }
}

void
get_serial_num(char *serNumOut) {
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    sprintf(serNumOut, "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
}



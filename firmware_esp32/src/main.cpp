#include "main.h"

void
IRAM_ATTR sense_tick() {
    portENTER_CRITICAL_ISR(&timerMux);
    sampler = true;
    portEXIT_CRITICAL_ISR(&timerMux);
}

/*-----------------------------------------------------------------------------------------------------------------
|   Main
-----------------------------------------------------------------------------------------------------------------*/
void
setup() {
    /* network and web-related */
    char ssid[40] = {'\0'};
    char ssidPrefix[] = "thermoSkin";
    get_serial_num(serialNum);
    sprintf(ssid, "%s-%s", ssidPrefix, serialNum);
    setup_AP(ssid, password, localIP, gateway, subnet);
    setup_endpoints();

    /* peripherals */
    Serial.begin(115200);
    print_info();
    init_gpio_tim();
    Wire.begin();
    while (!initialize_sensor()) { delay(10); }
    while (!initialize_ina219()) { delay(10); }

    /* get initial values */
    get_measurements(roomTemp, humidity);
    roomResistance = get_resistance(RES_SAMPLES_LARGE);
    resistance = roomResistance;
    temperature = (float)calculate_temp(resistance);

}

void
loop() {
    if (sampler){
        sampler = false;
        resistance = get_resistance(RES_SAMPLES_LARGE);
        temperature = (float)calculate_temp(resistance);
        Serial.printf("\r\n[DATA]\tT: %.3f *C, R: %.4f ohm", temperature, resistance);
    }
}

/*-----------------------------------------------------------------------------------------------------------------
|   Functions
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
init_gpio_tim() {
    ledcSetup(heatChannel, heatFreq, heatResolution);
    ledcAttachPin(FET_PIN, heatChannel);
    pinMode(LED_PWM_PIN, OUTPUT);

    senseTimer = timerBegin(0, 80, true);
    timerAttachInterrupt(senseTimer, &sense_tick, true);
    timerAlarmWrite(senseTimer, sampleTime, true);
    timerAlarmEnable(senseTimer);

    ledcSetup(heatChannel, heatFreq, heatResolution);
    ledcAttachPin(FET_PIN, heatChannel);
}

void
get_serial_num(char *serNumOut) {
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    sprintf(serNumOut, "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
}

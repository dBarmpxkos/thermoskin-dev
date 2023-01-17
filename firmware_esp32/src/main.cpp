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
    char ssid[40] = {'\0'};
    char ssidPrefix[] = "thermoSkin";
    get_serial_num(serialNum);
    sprintf(ssid, "%s-%s", ssidPrefix, serialNum);
    setup_AP(ssid, password, localIP, gateway, subnet);
    setup_endpoints();

    Wire.begin();
    Serial.begin(115200);
    print_info();

    gpio_init();

    breather(ledBreath);
    liveState = stateIDLE;

    resistance = drive_measure_res(1);
    temperature = (float)calculate_temp(resistance);

    heaterPID_Low.SetTunings(Kp, Ki, Kd);
    heaterPID_Mid.SetTunings(Kp, Ki, Kd);
    heaterPID_High.SetTunings(Kp, Ki, Kd);

    heaterPID_Low.SetMode(QuickPID::Control::automatic);
    heaterPID_Mid.SetMode(QuickPID::Control::automatic);
    heaterPID_High.SetMode(QuickPID::Control::automatic);

}

void
loop() {

    if (activeCtrl == dummyCtrl)
        dummy_ctrl();
    else if (activeCtrl == sliderCtrl)
        slider_ctrl();
    else if (activeCtrl == PIDCtrl)
        PID_ctrl();

    resistance = drive_measure_res(1);
    temperature = (float)calculate_temp(resistance);
//
//    Serial.printf("\r\n[PID]\tmeasured temperature: %.4f, res: %.4f", temperature, resistance);
//    delay(500);
//    heaterPID_Low.Compute();
//    ledcWrite(heatChannel, (uint32_t)pwmToHeat);


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
gpio_init() {
    pinMode(FET_PIN, OUTPUT);
    pinMode(P_FET_PIN, OUTPUT);
    digitalWrite(P_FET_PIN, LOW);

    pinMode(LED_PWM_PIN, OUTPUT);

    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(LED_PWM_PIN, ledChannel);
    ledcSetup(heatChannel, heatFreq, heatResolution);
    ledcAttachPin(FET_PIN, heatChannel);
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

    } else if (ledState == ledHeatLowPulse) {
        breathTimer = timerBegin(0, 40, true);
        timerAttachInterrupt(breathTimer, &led_ticker, true);
        timerAlarmWrite(breathTimer, 5000, true);
        timerAlarmEnable(breathTimer);

    } else if (ledState == ledHeatMidPulse) {
        breathTimer = timerBegin(0, 40, true);
        timerAttachInterrupt(breathTimer, &led_ticker, true);
        timerAlarmWrite(breathTimer, 2500, true);
        timerAlarmEnable(breathTimer);

    } else if (ledState == ledHeatHighPulse) {
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

void
dummy_ctrl() {
    if (lowHeat) {
        breather(ledHeatLowPulse);
        unsigned long timePressed = millis();

        digitalWrite(P_FET_PIN, HIGH);
        ledcWrite(heatChannel, 180);

        do {
            delay(200);
        } while (millis() - timePressed < 2800);

        digitalWrite(P_FET_PIN, LOW);
        ledcWrite(heatChannel, 0);
        breather(ledBreath);
        lowHeat = false;

    } else if (medHeat) {
        breather(ledHeatMidPulse);
        unsigned long timePressed = millis();
        unsigned char ticker = 0;

        digitalWrite(P_FET_PIN, HIGH);
        ledcWrite(heatChannel, 200);

        do {
            delay(200);
        } while (millis() - timePressed < 3000);

        digitalWrite(P_FET_PIN, LOW);
        ledcWrite(heatChannel, 0);
        breather(ledBreath);
        medHeat = false;

    } else if (highHeat) {
        breather(ledHeatHighPulse);
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
        highHeat = false;
    }
}

void
slider_ctrl() {
    pwmToHeat = pwmValStr.toInt();
    if (pwmToHeat) {
        digitalWrite(P_FET_PIN, HIGH);
        ledcWrite(heatChannel, pwmToHeat);
        ledcWrite(ledChannel, pwmToHeat);
    } else {
        digitalWrite(P_FET_PIN, LOW);
        ledcWrite(heatChannel, 0);
        ledcWrite(ledChannel, 0);
        resistance = drive_measure_res(1);
        temperature = calculate_temp(resistance);
    }
}

void
PID_ctrl() {
    resistance = drive_measure_res(1);
    temperature = calculate_temp(resistance);       /* r0, tcr and t0 */

    if (lowHeat) {
        breather(ledHeatLowPulse);
        heaterPID_Low.Compute();
    } else if (medHeat) {
        heaterPID_Mid.Compute();
        breather(ledHeatMidPulse);
    } else if (highHeat) {
        heaterPID_High.Compute();
        breather(ledHeatHighPulse);
    }

    digitalWrite(P_FET_PIN, HIGH);
    ledcWrite(heatChannel, pwmToHeat);

}
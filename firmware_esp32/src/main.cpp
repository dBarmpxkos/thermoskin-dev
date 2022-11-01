#include "main.h"

/*-----------------------------------------------------------------------------------------------------------------
|   Main
-----------------------------------------------------------------------------------------------------------------*/
void
setup() {
    Serial.begin(115200);
//    setup_AP(ssid, password, localIP, gateway, subnet);
//    setup_endpoints();
    pinMode(ACC_INT_PIN, INPUT_PULLUP);
    print_info();
    init_mpu();

    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(brightness);
    leds[0] = CRGB(0xFF9329);
    FastLED.show();

    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(FET_PIN, ledChannel);

}

void
loop() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" degC");

    if (brightness >= ceilingz) dir = DIR_DOWN;
    else if (brightness <= floorz) dir = DIR_UP;
    dir == DIR_UP ? brightness++ : brightness--;

    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(10);
}

/*-----------------------------------------------------------------------------------------------------------------
|   Function Sources
-----------------------------------------------------------------------------------------------------------------*/
void
print_info() {
    char macAddr[20] = {'\0'};
    sprintf(macAddr, "%s", WiFi.macAddress().c_str());
    Serial.printf("Serial number: %s\r\nFirmware Version: %s:%s\r\nHardware Version: %s\r\n",
                  macAddr, FW_VER, FW_DATE, HW_VER);

}

float
check_get_temp() {
    sensors.requestTemperatures();
    delay(10);
    float temperatureC = sensors.getTempCByIndex(0);
    if (temperatureC == DEVICE_DISCONNECTED_C) {
        lcd_go_and_print(display, "ERROR");
        Serial.print("error in temp measurement");
        Serial.println(temperatureC);
        return -1;
    } else {
        lcd_print_temperature(display, temperatureC, targetTemp);
        Serial.printf("\r\n[T]\t%.2f\r\n[X]\t%.2f", temperatureC, targetTemp);
        return temperatureC;
    }
}
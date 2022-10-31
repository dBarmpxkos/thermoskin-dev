#include "main.h"

/*-----------------------------------------------------------------------------------------------------------------
|   Main
-----------------------------------------------------------------------------------------------------------------*/
void
setup() {
    Serial.begin(115200);

//    setup_AP(ssid, password, localIP, gateway, subnet);
//    setup_endpoints();

//    ledcSetup(ledChannel, freq, resolution);
//    ledcAttachPin(FET_PIN, ledChannel);

    lcd_begin(display);
    print_info();
    init_mpu();
//    init_temp_sensor();
}

void
loop() {
//    runningTemp = check_get_temp();

    if (timeMoved && (millis() - timeMoved >= burnAfter)) {
//        Serial.printf("\r\n[FET]\tPowering up FET");
//        lcd_go_and_print(display, "FET ON");
        timeStarted = millis();
        timeMoved = 0;
        active = true;
    }

    if (millis() - timeStarted >= burnTime) {
//        Serial.printf("\r\n[FET]\tSwitching off FET");
//        lcd_go_and_print(display, "FET OFF");
        timeStarted = 0;

    }
    if (mpu.getMotionInterruptStatus() && !timeMoved) {
        lcd_go_and_print(display, "!");
        timeMoved = millis();
    }

    if (active){
        display.clearDisplay();

        display.drawFastHLine(0, 17, progress, SSD1306_WHITE);
        display.drawFastHLine(0, 18, progress, SSD1306_WHITE);
        display.drawFastHLine(0, 19, progress, SSD1306_WHITE);
        display.drawFastHLine(0, 20, progress, SSD1306_WHITE);
        display.drawFastHLine(0, 21, progress, SSD1306_WHITE);
        display.drawFastHLine(0, 22, progress, SSD1306_WHITE);
        progress = (progress + 5);
        display.display();
        delay(10);
        if (progress >= 128){
            delay(1000);
            display.clearDisplay();
            display.display();
            progress = 0;
            active = false;
            display.drawBitmap(1, 1, virus, LOGO_WIDTH, LOGO_HEIGHT, 1);
            display.display();
            delay(5);

            display.clearDisplay();
            display.display();
            display.drawBitmap(1, 1, novirus1, LOGO_WIDTH, LOGO_HEIGHT, 1);
            display.display();
            delay(5);

            display.clearDisplay();
            display.display();
            display.drawBitmap(1, 1, novirus2, LOGO_WIDTH, LOGO_HEIGHT, 1);
            display.display();
            delay(5);

            display.clearDisplay();
            display.display();
            display.drawBitmap(1, 1, novirus3, LOGO_WIDTH, LOGO_HEIGHT, 1);
            display.display();
            delay(5);

            display.clearDisplay();
            display.display();
            display.drawBitmap(1, 1, novirus4, LOGO_WIDTH, LOGO_HEIGHT, 1);
            display.display();
            delay(500);

            display.clearDisplay();
            display.display();
        }
    }
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
    display.setCursor(0, 0);
    display.setTextColor(SSD1306_WHITE);
    display.print("HEAT");
    delay(1500);
    display.drawBitmap(40, 2, s1, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
    delay(300);
    display.drawBitmap(65, 2, s1, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
    delay(300);
    display.drawBitmap(90, 2, s1, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
    delay(1000);

    display.setCursor(0, 32);
    display.print("");
    display.display();
    delay(1500);


    display.clearDisplay();
    display.display();
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
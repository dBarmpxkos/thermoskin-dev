#include "main.h"

void IRAM_ATTR
knock_out() {
    byebye = true;
}

void IRAM_ATTR
touched() {
    timeTouched = millis();
    timeFinished = 0;
    smCtrl = sIdle;
    touchDetect = true;
}

void
setup() {
    touchCounter++;

    char ssid[40] = {'\0'}; /* network and web-related */
    char ssidPrefix[] = "thermoSkin";
    get_serial_num(serialNum);
    sprintf(ssid, "%s-%s", ssidPrefix, serialNum);
    setup_AP(ssid, password, localIP, gateway, subnet);
    setup_endpoints();

    Serial.begin(115200);   /* peripherals */

    FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds, 1).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(255);
    leds[0] = WOKE_COL;
    FastLED.show();

    delay(300);

    print_info();
    init_gpio_tim();

    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.printf("\r\n[MEM]\tSPIFFS Mount Failed");
        SPIFFSAOK = false;
    } else {
        Serial.printf("\r\n[MEM]\tSPIFFS Mount OK");
        SPIFFSAOK = true;
    }

    Wire.begin();
    while (!initialize_sensor()) { delay(10); }
    while (!initialize_ina219()) { delay(10); }

    if (touchCounter == 1){
        get_measurements(roomTemp, humidity);   /* get initial values */
        digitalWrite(FET_PIN, HIGH);
        roomResistance = get_resistance(1);
        digitalWrite(FET_PIN, LOW);
        resistance = roomResistance;
        temperature = (float)calculate_temp(resistance);
        char headers[100];
        sprintf(headers,"room temperature: %.2f, room resistance: %.4f", roomTemp, resistance);
        writeFile(SPIFFS, "/measurements.txt", headers);
    }

    Serial.printf("\r\n[INIT]\tInitial value is: %.4f ohm @ %.2f *C, woke up %d times",
                  roomResistance, roomTemp, touchCounter);

    smCtrl = sIdle;

}

void
loop() {

    if (smCtrl == sIdle){
        touchDetect = false;
        temperature = 0;
        while (millis() - timeTouched < 2000) { /* waits */ }
        smCtrl = sTouched;
        touchCounter++;
    }

    if (smCtrl == sTouched){

        uint8_t ledR = 10;
        float initRes = 0, initTemp = 0;
        leds[0] = HEAT_COL;
        FastLED.show();

        digitalWrite(FET_PIN, HIGH);
        initRes = get_resistance(1);
        initTemp = (float)calculate_temp(resistance);
        do {
            if (touchDetect) {
                touchDetect = false;
                break;
            }
            resistance = get_resistance(1);
            temperature = (float)calculate_temp(resistance);

            ledR = map((int)temperature, (int)roomTemp, TARGET_TEMP, 10, 255);
            leds[0].red = ledR;
            FastLED.show();

            Serial.printf("\r\n%lu, %.3f, %.4f", millis(), temperature, resistance);
        } while (temperature < TARGET_TEMP);
        digitalWrite(FET_PIN, LOW);
        timeFinished = millis();
        leds[0] = DONE_COL;
        FastLED.show();

        if (temperature >= TARGET_TEMP) {
            smCtrl = sFinished;
            Serial.printf("\r\n[NFO]\tFinished heating, moving to idle state");
            char dataToSPIFFS[100];
            sprintf(dataToSPIFFS, "\r\n%lu, %.4f, %.2f, %.4f, %.2f",
                    timeFinished - timeTouched, initRes, initTemp, resistance, temperature);
            appendFile(SPIFFS, "/measurements.txt", dataToSPIFFS);
        }
    }

    if (smCtrl == sFinished) {
        timerAlarmEnable(sleepTimer);
        timerWrite(sleepTimer, 0);
        while (!byebye) { /* wait for timer to knock this baby out, if touched here it will start working again */
            if (touchDetect) {
                leds[0] = WOKE_COL;
                FastLED.show();

                Serial.printf("\r\n[NFO]\tTouched again!!");
                touchDetect = false;
                break;
            }
        }

        if (smCtrl == sFinished) {  /* still finished or retouched? */
            Serial.printf("\r\n[NFO]\tNight-night");
            leds[0] = SLEEP_COL;
            FastLED.show();
            delay(500);
            leds[0] = CRGB(0, 0, 0);
            FastLED.show();
            esp_deep_sleep_start();
        }
    }

    /* testing for battery drain */

//        for (int i=0;i<500;i++) {
//            do {
//                leds[0] = HEAT_COL;
//                FastLED.show();
//                digitalWrite(FET_PIN, HIGH);
//                resistance = get_resistance(1);
//                temperature = (float)calculate_temp(resistance);
//                Serial.printf("\r\n%lu, %.3f, %.4f, %.3f, %.4f", millis(), temperature, resistance, batLevel, powLevel);
//            } while (temperature < TARGET_TEMP);
//
//            leds[0] = DONE_COL;
//            FastLED.show();
//            digitalWrite(FET_PIN, LOW);
//            delay(40000);
//        }
//        leds[0] = CRGB(0, 0, 0);
//        FastLED.show();
//        esp_deep_sleep_start();
//
//        while (1) { };

    /* testing for battery drain
    * */

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
//    ledcSetup(heatChannel, heatFreq, heatResolution);
//    ledcAttachPin(FET_PIN, heatChannel);
//    ledcWrite(heatChannel, 0);

    pinMode(FET_PIN, OUTPUT);
    digitalWrite(FET_PIN, LOW);

//    while (1) {
//        Serial.println(touchRead(T9));
//    }
    int touchCali = 12;
//
//    int touchDiff = 0;
//    while (touchCali == 0){
//        Serial.printf("\r\n[TS]\tCalibrating touch sensor, please touch");
//
//        int touchThreshold[touchCaliSize];
//        for (int i=0;i<touchCaliSize;i++){
//            touchThreshold[i] = touchRead(T9);
//            delay(10);
//        }
//
//        for (int i=0;i<touchCaliSize-1;i++){
//            touchDiff = touchThreshold[i+1] - touchThreshold[i];
//            if (touchDiff > 3){
//                touchCali = touchDiff + touchThreshold[i];
//            }
//        }
//    }
    Serial.printf("\r\n[TS]\tCalibrated touch sensor with threshold: %d", touchCali);
    touchAttachInterrupt(T9, touched, touchCali);  /* T9 is GPIO32 */
    esp_sleep_enable_touchpad_wakeup();
    
    sleepTimer = timerBegin(0, 80, true);
    timerAttachInterrupt(sleepTimer, &knock_out, true);
    timerAlarmWrite(sleepTimer, sampleTime, true);

}

void
get_serial_num(char *serNumOut) {
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    sprintf(serNumOut, "%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2]);
}

int
lazy_median(int arr[], uint8_t size) {
    std::sort(arr, arr + size);
    if (size % 2)  return arr[size / 2];
    else           return (arr[(size - 1) / 2] + arr[size / 2]) / 2;
}

void
listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void
readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void
writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("\r\n[MEM]\tWriting file: %s", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.printf("\r\n[MEM]\tFailed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.printf("\r\n[MEM]\tFile written");
    } else {
        Serial.printf("\r\n[MEM]\tWrite failed");
    }
    file.close();
}

void
appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("\r\n[MEM]\tAppending to file: %s", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.printf("\r\n[MEM]\tFailed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.printf("\r\n[MEM]\tData appended");
    } else {
        Serial.printf("\r\n[MEM]\tAppend failed");
    }
    file.close();
}

void
renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

void
deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

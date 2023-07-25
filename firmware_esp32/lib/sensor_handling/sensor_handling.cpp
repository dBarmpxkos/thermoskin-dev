#include "sensor_handling.h"

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

bool
initialize_sensor(){
    if (! shtc3.begin()) {
        Serial.printf("\r\n[SENS]\tProblem with SHTC3 sensor");
        while (1) delay(1);
    } else return true;
}

void
get_measurements(float &temperature, float &humidity){
    sensors_event_t liveHum, liveTemp;

    shtc3.getEvent(&liveHum, &liveTemp);
    temperature = liveTemp.temperature;
    humidity = liveHum.relative_humidity;

    Serial.printf("\r\n[SENS]\tHumidity: %.2f, Temperature: %.2f",
                  humidity, temperature);

}
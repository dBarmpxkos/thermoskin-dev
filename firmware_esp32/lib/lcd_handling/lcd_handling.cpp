#include "lcd_handling.h"

void
lcd_begin(Adafruit_SSD1306 &display){
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.display();
    delay(1000);
    display.clearDisplay();
    display.display();
}

void
lcd_clear(Adafruit_SSD1306 &display){
    display.clearDisplay();
    display.display();
}

void
lcd_go_and_print(Adafruit_SSD1306 &display, char *prefix, char *toPrint1){
    char preformat[100] = {'\0'};
    sprintf(preformat, "\r\n%s:%s", prefix, toPrint1);

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.printf("\r\n%s", preformat);
    display.display();
}

void
lcd_go_and_print(Adafruit_SSD1306 &display, const char *toPrint1){
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 5);
    display.setTextSize(3);
    display.printf("\r\n%s", toPrint1);
    display.display();
}

void
lcd_go_and_print_value(Adafruit_SSD1306 &display, char *prefix, int value){
    char preformat[100] = {'\0'};
    sprintf(preformat, "\r\n%s:%d", prefix, value);
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.printf("\r\n%s", preformat);
    display.display();
}

void
lcd_print_temperature(Adafruit_SSD1306 &display, float temperature, float target){
    lcd_clear(display);
    display.setCursor(0,0);
    display.setTextColor(SSD1306_WHITE);
    display.print("T: ");
    display.print(temperature);
    display.print((char)247);
    display.print("C");
    display.printf("\r\nX: ");
    display.print(target);
    display.print((char)247);
    display.print("C");
    display.display();
}
#include <Arduino.h>

#include "rgb_lcd.h"

rgb_lcd lcd;

#define PIN1 33
#define PIN2 26

const int color[] = {255, 0, 0};

void setup() {
    pinMode(PIN1, INPUT);
    pinMode(PIN2, OUTPUT);
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.setRGB(color[0], color[1], color[2]);
    lcd.print("hello, iot!");
}

void loop() {
    byte currentState = digitalRead(PIN1);

    if (currentState == HIGH) {
        digitalWrite(PIN2, HIGH);
    } else {
        digitalWrite(PIN2, LOW);
    }
    Serial.println(currentState);
}
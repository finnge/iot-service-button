#include <Arduino.h>

#define PIN1 33
#define PIN2 26

void setup() {
    pinMode(PIN1, INPUT);
    pinMode(PIN2, OUTPUT);
    Serial.begin(9600);
}

void loop() {
    byte currentState = !digitalRead(PIN1);

    if (currentState == HIGH) {
        digitalWrite(PIN2, HIGH);
    } else {
        digitalWrite(PIN2, LOW);
    }
    Serial.println(currentState);
}
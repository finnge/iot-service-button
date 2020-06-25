#include <Arduino.h>

#include "rgb_lcd.h"

#define PIN_BUTTON 33

// States
#define WAITING_TO_START 0
#define AUTHENTICATION 10
#define WAITING_TO_ABORT 20
#define ABORT 21
#define SENDING 30
#define RESET 40

rgb_lcd lcd;
byte isPressed = LOW;
int currentState = WAITING_TO_START;
bool firstTime = true;
bool isClear = false;
int countdown = 0;

const int color[] = {255, 0, 136};

void setup() {
    pinMode(PIN_BUTTON, INPUT);
    // Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.setRGB(color[0], color[1], color[2]);

    lcd.print("");
}

void loop() {
    switch (currentState) {
        case WAITING_TO_START:
            // enter state
            if (firstTime) {
                lcd.clear();
                lcd.print("waiting...");
                firstTime = false;
            }

            // leave state
            isPressed = digitalRead(PIN_BUTTON);
            if (isPressed == HIGH) {
                firstTime = true;
                currentState = AUTHENTICATION;
            }
            break;

        case AUTHENTICATION:
            // enter state
            if (firstTime) {
                lcd.clear();
                lcd.print("Halten Sie RFID an Sensor.");  // TODO: umbruch

                firstTime = false;
                isClear = false;
            }

            // enter state
            isPressed = digitalRead(PIN_BUTTON);
            if (!isClear && isPressed == LOW) {
                isClear = true;
            }
            if (isClear && isPressed == HIGH) {
                firstTime = true;
                currentState = WAITING_TO_ABORT;
            }
            break;

        case WAITING_TO_ABORT:
            // enter state
            if (firstTime) {
                lcd.clear();
                lcd.print("Verarbeiten – Abbrechen?");  // TODO: umbruch

                countdown = 3000;
                firstTime = false;
                isClear = false;
            }

            // go down
            countdown--;

            // check for button
            isPressed = digitalRead(PIN_BUTTON);
            if (!isClear && isPressed == LOW) {
                isClear = true;
            }
            if (isClear && isPressed == HIGH) {
                firstTime = true;
                currentState = ABORT;
            }

            // normal leave
            if (countdown == 0) {
                firstTime = true;
                currentState = SENDING;
            }
            break;

        case ABORT:
            lcd.clear();
            lcd.print("Vorgang abgebrochen!");
            delay(100);
            lcd.clear();
            delay(100);
            currentState = RESET;
            break;

        case SENDING:
            if (firstTime) {
                lcd.clear();
                lcd.print("Bestellung wurde gesendet");  // TODO: Umbruch

                firstTime = false;
            }

            // MQTT Nachricht senden

            // delay(100);
            currentState = RESET;
            break;

        case RESET:  // vllt überspringen
            if (firstTime) {
                lcd.clear();
                lcd.print("Danke!");  // TODO: Umbruch

                firstTime = false;
            }

            // delay(100);
            currentState = WAITING_TO_START;
            break;
    }
}
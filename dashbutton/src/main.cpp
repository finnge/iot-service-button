#include <Arduino.h>

#include "rgb_lcd.h"

#define PIN_BUTTON 33
#define PIN_SOUND 26

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
    pinMode(PIN_SOUND, OUTPUT);
    Serial.begin(9600);
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

                Serial.println("Enter WAITING_TO_START");
            }

            // leave state
            isPressed = digitalRead(PIN_BUTTON);
            if (isPressed == HIGH) {
                firstTime = true;
                currentState = AUTHENTICATION;

                Serial.println("Leave WAITING_TO_START");
            }
            break;

        case AUTHENTICATION:
            // enter state
            if (firstTime) {
                lcd.clear();
                lcd.print("Halten Sie RFID an Sensor.");  // TODO: umbruch

                firstTime = false;
                isClear = false;

                Serial.println("Enter AUTHENTICATION");
            }

            // check button
            isPressed = digitalRead(PIN_BUTTON);
            if (!isClear && isPressed == LOW) {
                isClear = true;
                Serial.println("> clear");
            }

            // leave state
            if (isClear && isPressed == HIGH) {
                firstTime = true;
                currentState = WAITING_TO_ABORT;

                Serial.println("Leave AUTHENTICATION");

                // make sound
                digitalWrite(PIN_SOUND, HIGH);
                delay(250);
                digitalWrite(PIN_SOUND, LOW);
            }
            break;

        case WAITING_TO_ABORT:
            // enter state
            if (firstTime) {
                lcd.clear();
                lcd.print("Verarbeiten - Abbrechen?");  // TODO: umbruch

                countdown = 300;
                firstTime = false;
                isClear = false;

                Serial.println("Enter WAITING_TO_ABORT");
            }

            // check for button
            isPressed = digitalRead(PIN_BUTTON);
            if (!isClear && isPressed == LOW) {
                isClear = true;
                Serial.println("> clear");
            }

            // go down
            if (isClear) {
                Serial.println(countdown);
                countdown = countdown - 1;
            }
            
            if (isClear && isPressed == HIGH) { // Bug
                firstTime = true;
                currentState = ABORT;

                Serial.println("Leaving WAITING_TO_ABORT");
            }

            // normal leave
            if (countdown == 0) {
                firstTime = true;
                currentState = SENDING;

                Serial.println("Leaving WAITING_TO_ABORT");
            }
            break;

        case ABORT:
            if (firstTime) {
                lcd.clear();
                lcd.print("Vorgang abgebrochen!");  // TODO: Umbruch

                firstTime = false;

                Serial.println("Enter ABORT");
                delay(1000);
                lcd.clear();
                delay(500);
            }

            // leaving
            firstTime = true;
            currentState = RESET;

            Serial.println("Leaving ABORT");
            break;

        case SENDING:
            if (firstTime) {
                lcd.clear();
                lcd.print("Bestellung wurde gesendet");  // TODO: Umbruch

                firstTime = false;

                Serial.println("Enter SENDING");
            }

            // MQTT Nachricht senden

            delay(1000); // Synthetisch
            firstTime = true;
            currentState = RESET;

            Serial.println("Leaving SENDING");

            // make sound
            digitalWrite(PIN_SOUND, HIGH);
            delay(250);
            digitalWrite(PIN_SOUND, LOW);
            break;

        case RESET:  // vllt überspringen
            if (firstTime) {
                lcd.clear();
                lcd.print("Danke!");  // TODO: Umbruch

                firstTime = false;

                Serial.println("Enter RESET");
            }

            delay(1000);
            firstTime = true;
            currentState = WAITING_TO_START;

            Serial.println("Leaving RESET");
            break;
    }
}
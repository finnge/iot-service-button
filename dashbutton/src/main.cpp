#include <Arduino.h>
#include <MFRC522.h>
#include <MQTTClient.h>
#include <SPI.h>
#include <WiFiClientSecure.h>
#include <stdlib.h>

#include "Helper.h"

// Objects
rgb_lcd lcd;
WiFiClientSecure network = WiFiClientSecure();
MQTTClient client = MQTTClient(256);
MFRC522 mfrc522(PIN_SLAVE_SELECT, PIN_RESET);

// settings
const int color[] = {255, 0, 136};

// runtime
byte isPressed = LOW;
int currentState = STANDBY;
bool firstTime = true;
bool isClear = false;
int countdown = 0;

String productname = "PRODUCT";
int order_counter = 5;
unsigned long uid = 1;
unsigned long currentUID = 1;
byte bufferATQA[20];
byte bufferSize = sizeof(bufferATQA);

/**
 * Setup
 *
 * Runs on startup of Microcontroller
 */
void setup() {
    pinMode(PIN_BUTTON, INPUT);
    pinMode(PIN_SOUND, OUTPUT);
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.setRGB(color[0], color[1], color[2]);
    lcd.setCursor(0, 0);

    lcd.print("Try connecting");
    lcd.setCursor(0, 1);
    lcd.print("to Network...");
    lcd.setCursor(0, 0);

    setupWiFi(&network);

    lcd.clear();
    lcd.print("Try connecting");
    lcd.setCursor(0, 1);
    lcd.print("to AWS...");
    lcd.setCursor(0, 0);

    connectAWS(&client, &network);

    SPI.begin();                        // Init SPI bus
    mfrc522.PCD_Init();                 // Init MFRC522
    mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card
}

/**
 * Loop
 *
 * Runs in an infinate loop every 'tick'
 */
void loop() {
    testConnectionAWS(&client);
    client.loop();

    switch (currentState) {
        case STANDBY:
            // enter state
            if (firstTime) {
                lcd.clear();
                lcd.print(productname);
                lcd.setCursor(0, 1);
                lcd.print("order 5x");
                lcd.setCursor(0, 0);

                firstTime = false;

                Serial.println("Enter STANDBY");
            }

            // leave state 1
            isPressed = digitalRead(PIN_BUTTON);
            if (isPressed == HIGH) {
                firstTime = true;
                currentState = WAITING_TO_START;

                Serial.println("Leave STANDBY");
            }

            // leave state 2
            if (mfrc522.PICC_IsNewCardPresent()) {
                uid = auth_getUID(&mfrc522);
                if (uid != -1) {
                    firstTime = true;
                    currentState = AUTHENTICATION;

                    Serial.println("Leave STANDBY");
                }
            }
            break;

        case WAITING_TO_START:
            // enter state
            if (firstTime) {
                lcd.clear();
                lcd.print("Hold RFID-Card");
                lcd.setCursor(0, 1);
                lcd.print("to Sensor");
                lcd.setCursor(0, 0);

                firstTime = false;

                Serial.println("Enter WAITING_TO_START");
            }

            // todo: timer

            // leave state
            if (mfrc522.PICC_IsNewCardPresent()) {
                uid = auth_getUID(&mfrc522);
                if (uid != -1) {
                    firstTime = true;
                    currentState = AUTHENTICATION;

                    Serial.println("Leave WAITING_TO_START");
                }
            }

            break;

        case AUTHENTICATION:
            // enter state
            if (firstTime) {
                lcd.clear();
                lcd.print("Checking...");
                lcd.setCursor(0, 1);

                firstTime = false;
                isClear = false;

                Serial.println("Enter AUTHENTICATION");
            }

            // check button
            if (uid == 2589037589 || uid == 2577276341 || uid == 3111100811) {
                // make sound
                digitalWrite(PIN_SOUND, HIGH);
                delay(250);
                digitalWrite(PIN_SOUND, LOW);

                // leaving state
                currentState = CONFIRMATION;
                firstTime = true;
                Serial.println("Leave AUTHENTICATION");
            } else {
                // leaving state
                currentState = FAILED;
                firstTime = true;
                Serial.println("Leave AUTHENTICATION");
            }
            break;

        case CONFIRMATION:
            // enter state
            if (firstTime) {
                lcd.clear();
                lcd.print("Checking...");
                lcd.setCursor(0, 1);
                lcd.print("Hold");
                lcd.setCursor(0, 0);

                countdown = 50;
                firstTime = false;
                isClear = false;

                Serial.println("Enter WAITING_TO_ABORT");
            }

            // check for RFID

            mfrc522.PICC_WakeupA(bufferATQA, &bufferSize);
            currentUID = auth_getUID(&mfrc522);

            Serial.printf("Curr: %lu <=> New %lu\n", uid, currentUID);

            if (currentUID != uid) {
                firstTime = true;
                currentState = ABORT;

                Serial.println("Leaving WAITING_TO_ABORT");
            } else if (countdown == 0) {
                firstTime = true;
                currentState = SEND_ORDER;

                Serial.println("Leaving WAITING_TO_SEND");
            }

            countdown = countdown - 1;
            Serial.println(countdown);

            if (countdown % 6 == 0) {
                lcd.print(".");
            }
            break;

        case ABORT:
            if (firstTime) {
                lcd.clear();
                lcd.print("Abort");
                lcd.setCursor(0, 1);
                lcd.print("Process");  // Umbruch
                lcd.setCursor(0, 0);

                firstTime = false;

                Serial.println("Enter ABORT");
                delay(1000);
                lcd.clear();
                delay(500);
            }

            // leaving
            firstTime = true;
            currentState = STANDBY;

            Serial.println("Leaving ABORT");
            break;

        case SEND_ORDER:
            // first time
            if (firstTime) {
                lcd.clear();
                lcd.print("Send");
                lcd.setCursor(0, 1);
                lcd.print("order");  // Umbruch
                lcd.setCursor(0, 0);

                firstTime = false;

                Serial.println("Enter SEND_ORDER");
            }

            // MQTT Nachricht senden
            publishMessage(&client);

            // make sound
            digitalWrite(PIN_SOUND, HIGH);
            delay(250);
            digitalWrite(PIN_SOUND, LOW);

            // leaving
            firstTime = true;
            currentState = STANDBY;

            Serial.println("Leaving SEND_ORDER");
            break;

        case FAILED:
            // first time
            if (firstTime) {
                lcd.clear();
                lcd.print("Not Autherized");
                lcd.setCursor(0, 1);

                firstTime = false;

                Serial.println("Enter FAILED");
            }

            // leaving
            firstTime = true;
            currentState = STANDBY;

            Serial.println("Leaving SENDING");
            break;
    }
}
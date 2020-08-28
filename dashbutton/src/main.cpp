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
bool firstIteration = true;
bool isClear = false;
unsigned long thisMillis = 0;
unsigned long lastMillis = 0;
int tmpcounter = 0;

int order_counter = 5;
unsigned long uid = 1;
unsigned long currentUID = 1;
byte bufferATQA[20];
byte bufferSize = sizeof(bufferATQA);

// configuration
String productname = "Schrauben";
int quantity = 15;

// authentication

#define AUTH 40
long long auth[AUTH] = {2589037589, 2577276341};

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

    setupWiFi(&network);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Try connecting");
    lcd.setCursor(0, 1);
    lcd.print("to AWS...");

    connectAWS(&client, &network);
    client.onMessageAdvanced(callback);

    // String authtopic = strcat("auth/", THINGNAME);
    // String setuptopic = strcat("setup/", THINGNAME);

    client.subscribe(DASHBUTTON_TOPIC_SETUP);
    client.subscribe(DASHBUTTON_TOPIC_AUTH);
    client.subscribe("auth");
    client.subscribe("setup");

    if (client.connected() == true) {
        Serial.println("MQTT is connected!");
    }

    client.publish("init", "init Dashbutton1");

    //"Button druecken "
    lcd.clear();
    lcd.print("Try connecting");
    lcd.setCursor(0, 1);
    lcd.print("to AWS...");
    lcd.setCursor(0, 0);

    SPI.begin();                        // Init SPI bus
    mfrc522.PCD_Init();                 // Init MFRC522
    mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card
}

void callback(MQTTClient *client, char topic[], char payload[],
              int payload_length) {
    Serial.print("Received messages: ");
    Serial.println(topic);
    Serial.println(payload);
    Serial.println(payload_length);

    if (strcmp(topic, DASHBUTTON_TOPIC_AUTH) == 0) {
        char delimiter[] = ",;";
        char *ptr;
        char *befehl;
        long long value;

        ptr = strtok(payload, delimiter);
        befehl = ptr;
        ptr = strtok(NULL, delimiter);
        value = atoll(ptr);
        Serial.println(befehl);

        if (strcmp(befehl, "POST") == 0) {
            for (int i = 0; i < AUTH; i++) {
                if (auth[i] == '\0') {
                    auth[i] = value;
                    break;
                }
            }
        }

        if (strcmp(befehl, "DEL") == 0) {
            for (int i = 0; i < AUTH; i++) {
                if (auth[i] == value) {
                    auth[i] = '\0';
                    break;
                }
            }
        }
    }

    if (strcmp(topic, DASHBUTTON_TOPIC_SETUP) == 0) {
        char delimiter[] = ",;";
        char *ptr;

        ptr = strtok(payload, delimiter);
        productname = ptr;
        Serial.println(productname);
        ptr = strtok(NULL, delimiter);
        quantity = atoi(ptr);
        Serial.println(quantity);
        firstIteration = true;
    }
}

/**
 * Loop
 *
 * Runs in an infinite loop every 'tick'
 */
void loop() {
    testConnectionAWS(&client);
    client.loop();

    switch (currentState) {
        case STANDBY:
            // enter state
            if (firstIteration) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(productname);
                lcd.setCursor(0, 1);
                lcd.print("Menge: ");
                lcd.print(quantity);
                lcd.print("x");

                firstIteration = false;

                Serial.println("Enter STANDBY");
            }

            // leave state 1
            isPressed = digitalRead(PIN_BUTTON);
            if (isPressed == HIGH) {
                firstIteration = true;
                currentState = WAITING_TO_START;

                Serial.println("Leave STANDBY");
            }

            // leave state 2
            if (mfrc522.PICC_IsNewCardPresent()) {
                uid = auth_getUID(&mfrc522);
                if (uid != -1) {
                    firstIteration = true;
                    currentState = AUTHENTICATION;

                    Serial.println("Leave STANDBY");
                }
            }
            break;

        case WAITING_TO_START:
            // enter state
            if (firstIteration) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Hold RFID-Card");
                lcd.setCursor(0, 1);
                lcd.print("to Sensor");

                firstIteration = false;

                Serial.println("Enter WAITING_TO_START");
            }

            // leave state
            if (mfrc522.PICC_IsNewCardPresent()) {
                uid = auth_getUID(&mfrc522);
                if (uid != -1) {
                    firstIteration = true;
                    currentState = AUTHENTICATION;

                    Serial.println("Leave WAITING_TO_START");
                }
            }

            break;

        case AUTHENTICATION:
            // enter state
            if (firstIteration) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Checking...");
                lcd.setCursor(0, 1);

                firstIteration = false;
                isClear = false;

                Serial.println("Enter AUTHENTICATION");
            }

            tmpcounter = 0;

            // check button
            for (int i = 0; i < AUTH; i++) {
                if (uid == auth[i]) {
                    // make sound
                    digitalWrite(PIN_SOUND, HIGH);
                    delay(250);
                    digitalWrite(PIN_SOUND, LOW);

                    // leaving state
                    currentState = CONFIRMATION;
                    firstIteration = true;
                    Serial.println("Leave AUTHENTICATION");
                } else {
                    tmpcounter = tmpcounter + 1;
                    Serial.println(tmpcounter);

                    if (tmpcounter == AUTH) {
                        // leaving state
                        currentState = FAILED;
                        firstIteration = true;
                        Serial.println("Leave AUTHENTICATION");
                    }
                }
            }
            break;

        case CONFIRMATION:
            // enter state
            if (firstIteration) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Checking...");
                lcd.setCursor(0, 1);
                lcd.print("Hold ");

                firstIteration = false;
                isClear = false;

                thisMillis = millis();
                lastMillis = 0;

                Serial.println("Enter WAITING_TO_ABORT");
            }

            // check for RFID

            mfrc522.PICC_WakeupA(bufferATQA, &bufferSize);
            currentUID = auth_getUID(&mfrc522);

            Serial.printf("Curr: %lu <=> New %lu\n", uid, currentUID);

            if (currentUID != uid) {
                firstIteration = true;
                currentState = ABORT;

                Serial.println("Leaving WAITING_TO_ABORT");
            } else if (millis() >= thisMillis + CONFIRMATION_PERIOD) {
                firstIteration = true;
                currentState = SEND_ORDER;

                Serial.println("Leaving WAITING_TO_SEND");
            }

            if ((millis() - lastMillis) > ((CONFIRMATION_PERIOD) / 3)) {
                lcd.print(".");
                lastMillis = millis();

                // make sound
                digitalWrite(PIN_SOUND, HIGH);
                delay(100);
                digitalWrite(PIN_SOUND, LOW);
            }
            break;

        case ABORT:
            if (firstIteration) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Abort");
                lcd.setCursor(0, 1);
                lcd.print("Process");

                firstIteration = false;

                Serial.println("Enter ABORT");
            }

            // make sound
            digitalWrite(PIN_SOUND, HIGH);
            delay(500);
            digitalWrite(PIN_SOUND, LOW);

            // wait a bit
            delay(1000);
            lcd.clear();
            delay(500);

            // leaving
            firstIteration = true;
            currentState = STANDBY;

            Serial.println("Leaving ABORT");
            break;

        case SEND_ORDER:
            // first time
            if (firstIteration) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Send");
                lcd.setCursor(0, 1);
                lcd.print("order");

                firstIteration = false;

                Serial.println("Enter SEND_ORDER");
            }

            // MQTT Nachricht senden
            publishMessage(&client, productname, quantity);

            // make sound
            digitalWrite(PIN_SOUND, HIGH);
            delay(50);
            digitalWrite(PIN_SOUND, LOW);
            delay(25);
            digitalWrite(PIN_SOUND, HIGH);
            delay(100);
            digitalWrite(PIN_SOUND, LOW);

            // wait a bit
            delay(500);

            // leaving
            firstIteration = true;
            currentState = STANDBY;

            Serial.println("Leaving SEND_ORDER");
            break;

        case FAILED:
            // first time
            if (firstIteration) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Not Autherized");

                firstIteration = false;

                Serial.println("Enter FAILED");
            }

            // make sound
            digitalWrite(PIN_SOUND, HIGH);
            delay(500);
            digitalWrite(PIN_SOUND, LOW);

            // wait a bit
            delay(500);

            // leaving
            firstIteration = true;
            currentState = STANDBY;

            Serial.println("Leaving SENDING");
            break;
    }
}
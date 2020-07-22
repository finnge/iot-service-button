#include <Arduino.h>
#include <PubSubClient.h>

#include "rgb_lcd.h"

// AWS-----
#include <ArduinoJson.h>
#include <MQTTClient.h>
#include <WiFiClientSecure.h>

#include "WiFi.h"
#include "secrets.h"

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);
// AWS-----

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

const char *broker = "broker.hivemq.com";
const char *topic = "thkoeln/iot/team03/bestellung";

// WiFiClient espClient;
// PubSubClient client(espClient);

const int color[] = {255, 0, 136};

void mqtt_callback(char *topic, byte *payload, unsigned int length);

// AWS----
void connectAWS() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.println("Connecting to Wi-Fi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // Configure WiFiClientSecure to use the AWS IoT device credentials
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);

    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    client.begin(AWS_IOT_ENDPOINT, 8883, net);

    // Create a message handler
    // client.onMessage(messageHandler);

    Serial.print("Connecting to AWS IOT");

    while (!client.connect(THINGNAME)) {
        Serial.print(".");
        delay(100);
    }

    if (!client.connected()) {
        Serial.println("AWS IoT Timeout!");
        return;
    }

    // Subscribe to a topic
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

    Serial.println("AWS IoT Connected!");
}

void publishMessage() {
    StaticJsonDocument<200> doc;
    doc["time"] = millis();
    doc["sensor_a0"] = analogRead(0);
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);  // print to client

    client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(String &topic, String &payload) {
    Serial.println("incoming: " + topic + " - " + payload);

    //  StaticJsonDocument<200> doc;
    //  deserializeJson(doc, payload);
    //  const char* message = doc["message"];
}
// AWS----

/***
    void setupWifi() {
    Serial.print("\nConnecting to");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.println(WiFi.status());
    }

    Serial.print("\nConnected to");
    Serial.println(WIFI_SSID);
}

// Connenct/Reconnect to MQTT Broker in Case of Connection loss
void reconnect() {
    while (!client.connected()) {
        Serial.print("\nConencting to ");
        Serial.println(broker);
        if (client.connect(
                "TH_Koeln_IoT_Team03_ESP8266Client-"))  // ClientName am Server,
                                                        // sollte unique sein
        {
            Serial.print("\nConnected to");
            Serial.println(broker);
            client.subscribe(topic);
        } else {
            Serial.print("\nTrying again...");
            delay(5000);
        }
    }
}
***/

void setup() {
    pinMode(PIN_BUTTON, INPUT);
    pinMode(PIN_SOUND, OUTPUT);
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.setRGB(color[0], color[1], color[2]);
    lcd.setCursor(0, 0);

    lcd.print("Try connecting");
    lcd.setCursor(0, 1);
    lcd.print("to AWS...");
    lcd.setCursor(0, 0);

    connectAWS();

    /***
        Serial.print("Connecting to ... ");
        Serial.println(WIFI_SSID);

        setupWifi();
        client.setServer(broker, 1883);
        client.setCallback(mqtt_callback);
        client.setCallback(mqtt_callback);

        Serial.print("Successful connected to ... ");
        Serial.println(WIFI_SSID);
        ***/
}

void loop() {
    // if (!client.connected()) {
    //    reconnect();
    //}

    client.loop();

    switch (currentState) {
        case WAITING_TO_START:
            // enter state
            if (firstTime) {
                lcd.clear();
                lcd.print("Button druecken");
                lcd.setCursor(0, 1);
                lcd.print("um zu starten");
                lcd.setCursor(0, 0);

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
                lcd.print("Halten Sie RFID");
                lcd.setCursor(0, 1);
                lcd.print("an den Sensor.");  // umbruch
                lcd.setCursor(0, 0);

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
                lcd.print("Verarbeiten - ");
                lcd.setCursor(0, 1);
                lcd.print("Abbrechen?");
                lcd.setCursor(0, 0);

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

            if (isClear && isPressed == HIGH) {  // Bug
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
                lcd.print("Vorgang");
                lcd.setCursor(0, 1);
                lcd.print("abgebrochen!");  // Umbruch
                lcd.setCursor(0, 0);

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
                lcd.print("Bestellung wurde");
                lcd.setCursor(0, 1);
                lcd.print("gesendet!");  // Umbruch
                lcd.setCursor(0, 0);

                firstTime = false;

                Serial.println("Enter SENDING");
            }

            // MQTT Nachricht senden
            publishMessage();

            delay(1000);  // Synthetisch
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

void mqtt_callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Recieved messages: ");
    Serial.println(topic);
    for (int i = 0; i < length; i++) {
        Serial.printf("%c",
                      (char)payload[i]);  // Ausgabe der gesamten Nachricht
    }
    Serial.println();
}
#include "AWS.h"

/**
 * Establishes a connection to a local WiFi network.
 */
WiFiClientSecure setupWiFi(WiFiClientSecure network) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.println("[1/2] Connecting to WiFi-Network");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");

    // Configure WiFiClientSecure to use the AWS IoT device credentials
    network.setCACert(AWS_CERT_CA);
    network.setCertificate(AWS_CERT_CRT);
    network.setPrivateKey(AWS_CERT_PRIVATE);

    return network;
}

/**
 * Establishes a connection to AWS.
 */
MQTTClient connectAWS(MQTTClient client, WiFiClientSecure network) {
    client.begin(AWS_IOT_ENDPOINT, 8883, network);

    Serial.println("[2/2] Connecting to AWS IoT");

    while (!client.connect(THINGNAME)) {
        Serial.print(".");
        delay(100);
    }

    Serial.println("");

    if (!client.connected()) {
        Serial.println("AWS IoT Timeout!");
    } else {
        Serial.println("AWS IoT Connected!");
    }

    client.subscribe("esp32/sub");
    // client.subscribe(DASHBUTTON_TOPIC_AUTH);

    return client;
}

/**
 * Try to reconnect to AWS.
 */
void testConnectionAWS(MQTTClient client) {
    while (!client.connected()) {
        Serial.println("Lost Connection to AWS");
        Serial.println("Try again");

        while (!client.connect(THINGNAME)) {
            Serial.print(".");
            delay(100);
        }

        Serial.println("");
    }

    Serial.println("AWS IoT Connected!");
}
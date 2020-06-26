#include "iot_wifi.h"

#include <PubSubClient.h>
#include <WiFi.h>

#include "secret.h"

const char *broker = "hivemq.dock.moxd.io";
const char *topic = "thkoeln/iot/team03/bestellung";

PubSubClient setupWifi() {
    WiFiClient espClient;
    PubSubClient client(espClient);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.println(WiFi.status());
    }

    client.setServer(broker, 1883);

    return client;
}

void reconnect(PubSubClient client) {
    while (!client.connected()) {
        // Serial.print("\nConencting to");
        // Serial.println(broker);
        if (client.connect("TH_IOT_03_ESP32lient-")){
            // Serial.print("\nConnected to");
            // Serial.println(broker);
            client.subscribe(topic);
        } else {
            // Serial.print("\nTrying again...");
            delay(5000);
        }
    }
}
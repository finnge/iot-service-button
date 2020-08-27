#include "Helper.h"

void publishMessage(MQTTClient* client, String product, int quantity) {
    char output[30];
    sprintf(output, "%s;%d;%s", product, quantity, THINGNAME);
    (*client).publish(DASHBUTTON_TOPIC_ORDER, output);
}

void messageHandler(String topic, String payload) {
    Serial.println("incoming: " + topic + " - " + payload);
}

void setDisplay(String firstLine, String secondLine) {}
#include "Helper.h"

void publishMessage(MQTTClient *client) {
    char output[30];
    sprintf(output, "Schrauben;10;%s;1234", THINGNAME);
    (*client).publish(DASHBUTTON_TOPIC_ORDER, output);
}

void messageHandler(String topic, String payload) {
    Serial.println("incoming: " + topic + " - " + payload);
}

void setDisplay(String firstLine, String secondLine) {
    
}
#ifndef AWS_SETUP
#define AWS_SETUP

#include <MQTTClient.h>
#include <WiFiClientSecure.h>

#include "secrets.h"

/**
 * AWS Topics
 */

// Change of the product name and number of orders. To a specific button.
#define DASHBUTTON_TOPIC_SETUP strcat("setup/", THINGNAME)

// Submit a new order.
#define DASHBUTTON_TOPIC_ORDER "order"

// New incoming authentication ID (via RFID sensor)
#define DASHBUTTON_TOPIC_AUTH "auth"

/**
 * Functions
 */

// Establishes a connection to a local WiFi network.
WiFiClientSecure setupWiFi();

// Establishes a connection to AWS.
MQTTClient connectAWS(WiFiClientSecure network);

// Try to reconnect to AWS.
void testConnectionAWS(MQTTClient client);

#endif
#ifndef AWS_SETUP
#define AWS_SETUP

#include <MQTTClient.h>
#include <WiFiClientSecure.h>

#include "secrets.h"

/**
 * AWS Topics
 */

// Change of the product name and number of orders. To a specific button.
#define DASHBUTTON_TOPIC_SETUP "setup/" THINGNAME

// Submit a new order.
#define DASHBUTTON_TOPIC_ORDER "order"

// New incoming authentication ID (via RFID sensor).
#define DASHBUTTON_TOPIC_AUTH "auth/" THINGNAME

// Microcontroller has no data to show.
#define DASHBUTTON_TOPIC_SEND_INIT "init"

/**
 * Functions
 */

// Establishes a connection to a local WiFi network.
void setupWiFi(WiFiClientSecure *network);

// Establishes a connection to AWS.
void connectAWS(MQTTClient *client, WiFiClientSecure *network);

// Try to reconnect to AWS.
void testConnectionAWS(MQTTClient *client);

void callback(MQTTClient *client, char topic[], char payload[],
              int payload_length);

#endif
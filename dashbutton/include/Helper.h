#ifndef HELPER
#define HELPER

// includes
#include "AWS.h"
#include "Authentication.h"
#include "rgb_lcd.h"
#include "secrets.h"

// Pins
#define PIN_BUTTON 33
#define PIN_SOUND 26
#define PIN_SLAVE_SELECT 12
#define PIN_RESET 14

// States
#define STANDBY 0
#define WAITING_TO_START 5
#define AUTHENTICATION 10
#define CONFIRMATION 20
#define ABORT 21
#define FAILED 30
#define SEND_ORDER 40
#define SUCCESS 45


/**
 * Functions
 */
void publishMessage(MQTTClient *client);

void messageHandler(String topic, String payload);

void setDisplay(String firstLine, String secondLine);

#endif
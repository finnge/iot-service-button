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
#define WAITING_TO_START 0
#define AUTHENTICATION 10
#define WAITING_TO_ABORT 20
#define ABORT 21
#define SENDING 30
#define RESET 40


/**
 * Functions
 */
void publishMessage(MQTTClient *client);

void messageHandler(String topic, String payload);

void setDisplay(String firstLine, String secondLine);

#endif
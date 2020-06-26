#ifndef IOT_WIFI_H
#define IOT_WIFI_H

#include <PubSubClient.h>

PubSubClient setupWifi();

void reconnect(PubSubClient);

#endif
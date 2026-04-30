#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <WiFi.h>
#include "esp_wpa2.h" // WPA2 Enterprise
#include <PubSubClient.h>
#include "secrets.h"

void wifi_setup();
void mqtt_sub(const char* topic);
void mqtt_pub(const char* topic, const char* text);

#endif
#include <WiFi.h>
#include "esp_wpa2.h" // WPA2 Enterprise
#include <PubSubClient.h>
#include "secrets.h"

// WiFi
const char* ssid = "rpi_wpa2";
const char* mqttServer = "129.161.190.151";
const uint16_t mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Callback function to handle incoming messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void wifi_setup() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);

  Serial.println("Connecting to WPA2-Enterprise network...");

  // Set identity, username, password
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_USERNAME, strlen(EAP_USERNAME));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));

  // Enable WPA2 Enterprise
  esp_wifi_sta_wpa2_ent_enable();

  WiFi.begin(ssid);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqtt_sub(const char* topic){
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  Serial.println("Connecting to MQTT...");
  while (!client.connected()) {
    if (client.connect("huanga9_esp32_w")) {
      Serial.println("MQTT connected!");
      client.subscribe(topic);
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
    // Publish and subscribe
    client.publish(topic, "Weight ESP32 Subbed");
}

void mqtt_pub(const char* topic, const char* text) {
  if (!client.connected()) {
    Serial.println("MQTT reconnecting...");
    mqtt_sub(topic);
  }

  client.loop();
  client.publish(topic, text);
}
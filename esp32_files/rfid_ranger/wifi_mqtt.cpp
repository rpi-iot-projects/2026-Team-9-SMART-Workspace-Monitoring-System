#include "wifi_mqtt.h"

const char* ssid = "rpi_wpa2";
const char* mqttServer = "129.161.190.151";
const uint16_t mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) Serial.print((char)payload[i]);
  Serial.println();
}

void wifi_setup() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_USERNAME, strlen(EAP_USERNAME));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wifi_sta_wpa2_ent_enable();
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void mqtt_sub(const char* topic){
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    if (client.connect("huanga9_esp32_rl")) {
      client.subscribe(topic);
    } else {
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
  }
  client.publish(topic, "Ranger_LED ESP32 Subbed");
}

void mqtt_pub(const char* topic, const char* text) {
  if (!client.connected()) mqtt_sub(topic);
  client.loop();
  client.publish(topic, text);
}
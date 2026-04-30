#include "Ranger.h"
#include "RFID_LED.h"
#include "wifi_mqtt.h"

const char* mqtt_topic = "huanga9/ranger_led";

//RTOS Globals
const uint8_t queue_len = 50; //max size of queue
static QueueHandle_t mqtt_queue; 
typedef struct {
  char payload[100];
} MqttMessage;

TaskHandle_t ranger_handle;
TaskHandle_t rfid_handle; 
TaskHandle_t mqtt_pub_handle;

//Read ranger and send to mqtt topic
void ranger_task(void *parameter){
  while(1){
    ranger_poll();
    MqttMessage msg;
    snprintf(msg.payload, sizeof(msg.payload),
            "{\"distance\":%.2f}",distanceCm);
    if(xQueueSend(mqtt_queue, &msg, 10) != pdTRUE){ //10s timeout
      Serial.println("Failed to add ranger value to queue");
    }
    vTaskDelay(400 / portTICK_PERIOD_MS);
  }
}

//Read rfid tags and send to mqtt topic
void rfid_task(void *parameter){
  byte scannedUID[10];
  byte scannedSize = 0;
  String activity;
  while(1){
    bool detected = readCard(scannedUID, scannedSize);

    if (detected) {
      activity = getActivity(scannedUID, scannedSize);
      if(currentActivity != activity){ //If new tag is found
        copyUID(scannedUID, scannedSize);
        currentActivity = activity;
        showActivityColor(currentActivity);
        Serial.println(currentActivity);

        vTaskDelay(100/ portTICK_PERIOD_MS);

        MqttMessage msg;
        snprintf(msg.payload, sizeof(msg.payload),
                "{\"rfid\":\"%s\"}",currentActivity.c_str());
        mqtt_pub(mqtt_topic, msg.payload);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

//Dequeue and publish to mqtt
void mqtt_pub_task(void *parameter){
  MqttMessage msg;
  while(1){
    if(xQueueReceive(mqtt_queue, &msg, 10)==pdTRUE){
      mqtt_pub(mqtt_topic, msg.payload);
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void setup(){  
  //Setup
  Serial.begin(115200);
  LED_setup();
  ranger_setup();
  wifi_setup();

  vTaskDelay(2000 / portTICK_PERIOD_MS);

  //RTOS
  mqtt_queue = xQueueCreate(queue_len, sizeof(MqttMessage)); 
  xTaskCreate(ranger_task, "Ranger Task", 2048, NULL, 1, &ranger_handle); //1KB of stack = 1024, 768 bytes is overhead
  xTaskCreate(rfid_task, "Rfid Task", 3072, NULL, 1, &rfid_handle); //1KB of stack = 1024, 768 bytes is overhead
  xTaskCreate(mqtt_pub_task, "Mqtt Pub Task", 3072, NULL, 1, &mqtt_pub_handle); //1KB of stack = 1024, 768 bytes is overhead
}

void loop(){
  
}
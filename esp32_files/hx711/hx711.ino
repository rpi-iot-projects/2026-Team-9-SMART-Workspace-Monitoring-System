// Calibrating the load cell
#include "HX711.h"

// HX711 circuit wiring
const int dout1 = 17;
const int sck1 = 16;

const int dout2 = 19;
const int sck2 = 18;

// Calibration factors
const float calib1 = -337.08;
const float calib2 = -391.96889;

HX711 scale;
HX711 scale2;

const char* mqtt_topic = "huanga9/weight";

void setup() {
  Serial.begin(115200);
  scale.begin(dout1, sck1);

  scale.set_scale(calib1); 
  scale.tare();       
  scale2.begin(dout2, sck2);

  scale2.set_scale(calib2); 
  scale2.tare();
  
  //Tare both scales again for accuracy 
  delay(2000);
  scale.tare();      
  scale2.tare();      

  wifi_setup();
  mqtt_sub(mqtt_topic);
}

void loop() {
  float reading1 = 0.0;
  float reading2 = 0.0;
  
  //Grab scale 1 reading
  if (scale.is_ready()) {
    reading1 = scale.get_units(5);
    Serial.printf("Read1:");
    Serial.println(reading1);
  } else {
    Serial.println("HX711 not found.");
  }

  //Grab scale 1 reading
  if (scale2.is_ready()) {
    reading2 = scale2.get_units(5);
    Serial.printf("Read2:");
    Serial.println(reading2);
  } else {
    Serial.println("HX711 not found.");
  }
  //Send to mqtt topic
  char payload[100];
  snprintf(payload, sizeof(payload),
           "{\"left_weight\":%.2f,\"right_weight\":%.2f}",
           reading1, reading2);
  mqtt_pub(mqtt_topic, payload);
  delay(200);
} 

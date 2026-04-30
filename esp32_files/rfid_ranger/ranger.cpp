#include "Ranger.h"

static const int trigPin = 21;
static const int echoPin = 15;

static long duration;
float distanceCm = 0;
float distanceInch = 0;

void ranger_setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void ranger_poll() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    return;
  }

  distanceCm = duration * 0.034 / 2.0;
  distanceInch = distanceCm * 0.393701;
}
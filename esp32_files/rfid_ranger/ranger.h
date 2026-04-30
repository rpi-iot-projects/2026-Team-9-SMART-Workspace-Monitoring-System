#ifndef RANGER_H
#define RANGER_H

#include <Arduino.h>

extern float distanceCm;
extern float distanceInch;

void ranger_setup();
void ranger_poll();

#endif
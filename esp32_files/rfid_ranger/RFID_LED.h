#ifndef RFID_LED_H
#define RFID_LED_H

#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>

extern String currentActivity;

void LED_setup();
bool readCard(byte *uidOut, byte &sizeOut);
void copyUID(byte *uid, byte size);
String getActivity(byte *uid, byte size);
void showActivityColor(String activity);
void updateGaming();

#endif
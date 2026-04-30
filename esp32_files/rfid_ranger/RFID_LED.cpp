#include "RFID_LED.h"

#define RST_PIN 22
#define SS_PIN 5
#define LED_PIN 4
#define NUM_PIXELS 100

MFRC522 mfrc522(SS_PIN, RST_PIN);
Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

byte activeUID[10];
byte activeUIDSize = 0;
String currentActivity = "";

unsigned long lastAnimUpdate = 0;
int gamingHue = 0;

//Initializes all LEDS
void LED_setup() {
  SPI.begin(18, 19, 23, 5);
  mfrc522.PCD_Init();
  strip.begin();
  strip.setBrightness(60);
  strip.clear();
  strip.show();
}

//Set non-gaming color LEDS
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_PIXELS; i++)
    strip.setPixelColor(i, strip.Color(r, g, b));
  strip.show();
}

//Set gaming color LEDS
void updateGaming() {
  if (millis() - lastAnimUpdate < 20) return;
  lastAnimUpdate = millis();
  for (int i = 0; i < NUM_PIXELS; i++)
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(gamingHue + i * 3000)));
  strip.show();
  gamingHue = (gamingHue + 256) % 65536;
}

void showActivityColor(String activity) {
  if (activity == "Studying")    setColor(0, 200, 180);
  else if (activity == "Eating") setColor(255, 60, 0);
  else if (activity == "Gaming") updateGaming();
  else                           setColor(255, 255, 0);
}

String getActivity(byte *uid, byte size) {
  if (size == 4 && uid[0] == 0x5C && uid[1] == 0x96 && uid[2] == 0x62 && uid[3] == 0x22) return "Studying";
  if (size == 4 && uid[0] == 0xB3 && uid[1] == 0x47 && uid[2] == 0x4B && uid[3] == 0x16) return "Gaming";
  if (size == 4 && uid[0] == 0x73 && uid[1] == 0x9D && uid[2] == 0x56 && uid[3] == 0x16) return "Eating";
  return "Unknown";
}

void copyUID(byte *uid, byte size) {
  activeUIDSize = size;
  for (byte i = 0; i < size; i++)
    activeUID[i] = uid[i];
}

bool readCard(byte *uidOut, byte &sizeOut) {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    byte buf[2];
    byte bufSize = sizeof(buf);
    if (mfrc522.PICC_WakeupA(buf, &bufSize) != MFRC522::STATUS_OK) return false;
  }
  if (!mfrc522.PICC_ReadCardSerial()) return false;
  sizeOut = mfrc522.uid.size;
  for (byte i = 0; i < sizeOut; i++)
    uidOut[i] = mfrc522.uid.uidByte[i];
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  return true;
}
#include "config.h"

TTGOClass *ttgo;
TFT_eSPI *tft;

void setup() {

  //Set up the display
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();

  tft = ttgo->tft;

  tft->fillScreen(TFT_BLACK);
  tft->setTextFont(2);
  tft->setTextSize(1);
  tft->setTextColor(TFT_WHITE);
  tft->setCursor(0, 0);

  //splash screen
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextSize(2);
  ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->setCursor(40, 5);
  ttgo->tft->println(F("Serial 115200"));

  Serial.begin(115200);

  delay(1000);
}

int counter = 0;
int COUNTER_MAX = 240;
int y_offset = 0;

void loop() {
  //TODO receive 240 bytes over serial port
  //push one row of pixels(
  tft->setAddrWindow(0, counter, 240, 1);
  tft->startWrite();
  //offset pixels
  y_offset = random(240);
  tft->pushBlock(0x0, counter);
  tft->pushBlock(0xFFFF, 240-y_offset);
  tft->endWrite();
  counter++;
  if(counter >= COUNTER_MAX){
    counter = 0;    
  }
}

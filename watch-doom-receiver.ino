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

void loop() {
  //TODO serial receive
  //push one row of pixels(
  tft->pushBlock(0x0, counter);
  tft->pushBlock(0xFFFF, 240-counter);
  counter++;
}

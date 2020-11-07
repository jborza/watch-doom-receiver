#include "config.h"

TTGOClass *ttgo;
TFT_eSPI *tft;

void setup() {

  //Set up the display
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();

  tft = ttgo->tft;

  //splash screen
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextSize(2);
  ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->setCursor(40, 5);
  ttgo->tft->println(F("Serial 115200"));

  Serial.begin(115200);

  delay(1000);
}

int y = 0;
int Y_MAX = 240;
int y_offset = 0;
char rxBuffer[240/8];
uint16_t lineBuffer[240];

void loop() {
  //receive 240 bytes over serial port
  //then push one row of pixels to the screen
  Serial.readBytes(rxBuffer, 30); 
  tft->setAddrWindow(0, y, 240, 1);
  //offset pixels
  //convert my rxBuffer to black and white pixels
  for(int i = 0; i < 240; i++){
    int remainder = 7 - i % 8;
    lineBuffer[i] = (((rxBuffer[i / 8] >> (remainder)) & 1) == 1 ? 0 : 0xFFFF);
  }
  tft->startWrite();
  tft->pushPixels(lineBuffer, 240);
  tft->endWrite();
  y++;
  if(y >= Y_MAX){
    y = 0;    
  }
}

#include "config.h"

TTGOClass *ttgo;
TFT_eSPI *tft;

#define INVERT_COLORS

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
  ttgo->tft->println(F("Serial 250000"));

  Serial.begin(250000);

  delay(1000);
}


#define DISPLAY_LINE_LENGTH 240
#define RECEIVE_LINE_BITS 120
#define RECEIVE_LINE_BYTES 120/8

int y = 0;
int Y_MAX = 240;
int y_offset = 0;
char rxBuffer[RECEIVE_LINE_BYTES];
uint16_t lineBuffer[DISPLAY_LINE_LENGTH];


//now let's do 2x2 scaling

void loop() {
  //receive 240 bytes over serial port
  //then push one row of pixels to the screen
  Serial.readBytes(rxBuffer, RECEIVE_LINE_BYTES); 
  tft->setAddrWindow(0, y, DISPLAY_LINE_LENGTH, 2);
  //convert 120 received bits to 240 line pixels
  //convert my rxBuffer to black and white pixels
  for(int i = 0; i < RECEIVE_LINE_BITS; i++){
    //bit 0 -> pixel 0,1; bit 1 -> pixel 1,2
    int remainder = 7 - i % 8;
#ifdef INVERT_COLORS
    int color = (((rxBuffer[i / 8] >> (remainder)) & 1) == 1 ? 0xFFFF : 0);
#else
    int color = (((rxBuffer[i / 8] >> (remainder)) & 1) == 1 ? 0 : 0xFFFF);
#endif
    lineBuffer[i*2] = color;
    lineBuffer[i*2+1] = color;
  }
  tft->startWrite();
  tft->pushPixels(lineBuffer, DISPLAY_LINE_LENGTH);
  tft->pushPixels(lineBuffer, DISPLAY_LINE_LENGTH);
  tft->endWrite();
  y+=2;
  if(y >= Y_MAX){
    y = 0;    
  }
}

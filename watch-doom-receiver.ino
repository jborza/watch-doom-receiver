#include "config.h"

TTGOClass *ttgo;
TFT_eSPI *tft;

#define INVERT_COLORS

#define DISPLAY_LINE_LENGTH 240
#define RECEIVE_LINE_BITS 240
#define RECEIVE_LINE_BYTES 240/8
#define BAUD_RATE 500000
#define HSYNC 0x48
#define VSYNC 0x56

int y = 0;
int Y_MAX = 240;
uint8_t rxBuffer[RECEIVE_LINE_BYTES];
uint16_t lineBuffer[DISPLAY_LINE_LENGTH];

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
  ttgo->tft->print(F("Serial @ "));
  ttgo->tft->print(BAUD_RATE);
  Serial.begin(BAUD_RATE);

  delay(2000);
}


//now let's do 2x2 scaling
void loop() {
  uint8_t rx;
  uint8_t idx;
  //pixel doubling loop
  //receive 120 pixels (120 bits = 15 bytes) over serial port
  //then push two rows of pixels to the screen
  int read = Serial.readBytes(rxBuffer, RECEIVE_LINE_BYTES);
  if(read == 0)
  {
    Serial.write(VSYNC);
    return;
  }
  tft->setAddrWindow(0, y, DISPLAY_LINE_LENGTH, 1);
    //convert my rxBuffer (240 bits = 30 bytes) to 240 black and white pixels
    for(int x = 0; x < RECEIVE_LINE_BYTES; x++){
      rx = rxBuffer[x];
      idx = x << 3;
      lineBuffer[idx] = (((rx >> 7) & 1) == 1 ? 0xFFFF : 0);
      lineBuffer[idx+1] = (((rx >> 6) & 1) == 1 ?  0xFFFF : 0);
      lineBuffer[idx+2] = (((rx >> 5) & 1) == 1 ?  0xFFFF : 0);
      lineBuffer[idx+3] = (((rx >> 4) & 1) == 1 ?  0xFFFF : 0);
      lineBuffer[idx+4] = (((rx >> 3) & 1) == 1 ?  0xFFFF : 0);
      lineBuffer[idx+5] = (((rx >> 2) & 1) == 1 ?  0xFFFF : 0);
      lineBuffer[idx+6] = (((rx >> 1) & 1) == 1 ?  0xFFFF : 0);
      lineBuffer[idx+7] = (((rx) & 1) == 1 ?  0xFFFF : 0);
   }
  
  tft->startWrite();
  tft->pushPixels(lineBuffer, DISPLAY_LINE_LENGTH);
  tft->endWrite();
  y+=1;
  if(y >= Y_MAX){
    y = 0;    
    Serial.write(VSYNC);
  }
}

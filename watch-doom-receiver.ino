#include "config.h"

TTGOClass *ttgo;
TFT_eSPI *tft;

#define INVERT_COLORS


#define DISPLAY_LINE_LENGTH 240
#define RECEIVE_LINE_BITS 120
#define RECEIVE_LINE_BYTES 120/8
#define BAUD_RATE 500000
#define HSYNC 0x48
#define VSYNC 0x56

int y = 0;
int Y_MAX = 240;
uint8_t rxBuffer[RECEIVE_LINE_BYTES];
uint32_t lineBuffer[DISPLAY_LINE_LENGTH/2];

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
  //pixel doubling loop
  //receive 120 pixels (120 bits = 15 bytes) over serial port
  //then push two rows of pixels to the screen
  int read = Serial.readBytes(rxBuffer, RECEIVE_LINE_BYTES);
  if(read == 0)
  {
    Serial.write(VSYNC);
    return;
  }
  tft->setAddrWindow(0, y, DISPLAY_LINE_LENGTH, 2);
    //convert 120 received bits to 240 line pixels
    //convert my rxBuffer to black and white pixels

  int bit = 7;
  for (int i = 0; i < RECEIVE_LINE_BITS; i++) {
    //bit 0 -> pixel 0,1; bit 1 -> pixel 1,2
    //int remainder = 7 - i % 8;
#ifdef INVERT_COLORS
    int color = (((rxBuffer[i >> 3] >> (bit)) & 1) == 1 ? 0xFFFFFFFF : 0);
#else
    int color = (((rxBuffer[i / 8] >> (remainder)) & 1) == 1 ? 0 : 0xFFFF);
#endif
    lineBuffer[i] = color;
    if (bit == 0)
      bit = 7;
        else
        bit--;
  }
  tft->startWrite();
  tft->pushPixels(lineBuffer, DISPLAY_LINE_LENGTH);
  tft->pushPixels(lineBuffer, DISPLAY_LINE_LENGTH);
  tft->endWrite();
  y+=2;
  if(y >= Y_MAX){
    y = 0;    
    Serial.write(VSYNC);
  }
}

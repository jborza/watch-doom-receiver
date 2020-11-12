#include "config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/task.h>

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
uint8_t rxBufferCopy[RECEIVE_LINE_BYTES];
uint16_t lineBuffer[DISPLAY_LINE_LENGTH];

//task that handles the display part
TaskHandle_t displayTask;

//task that handles the receiver part
TaskHandle_t serialTask;

void serial_task(void * parameter){
  for(;;){
    //get notified by the display task, or time out on the initial processing
    ulTaskNotifyTake(pdTRUE, ( TickType_t ) 100 );
    int read = Serial.readBytes(rxBuffer, RECEIVE_LINE_BYTES);
    if(read == 0)
    {
      Serial.write(VSYNC);
      return;
    }
    //ulTaskNotifyTakeIndexed(1, pdTrue, (TickType_t) 100);
    //send notification to displayTask, bringing it out of Blocked state
    xTaskNotifyGive(displayTask);
  }
}

void display_task( void * parameter) {
  uint8_t rx;
  uint8_t idx;
  for(;;) {
    // black to wait for loopTask to notify this task
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
    // copy the received data
    memcpy(rxBufferCopy, rxBuffer, RECEIVE_LINE_BYTES);
    //notify the serial task that it can receive more data
    
    //clear the display buffer to remove junk from previous transmission
    tft->setAddrWindow(0, y, DISPLAY_LINE_LENGTH, 1);
    //convert my rxBuffer (240 bits = 30 bytes) to 240 black and white pixels
    for(int x = 0; x < RECEIVE_LINE_BYTES; x++){
      rx = rxBufferCopy[x];
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
    xTaskNotifyGive(serialTask);
    tft->startWrite();
    tft->pushPixels(lineBuffer, DISPLAY_LINE_LENGTH);
    tft->endWrite();
    y+=1;
    if(y >= Y_MAX){
      y = 0;    
      Serial.write(VSYNC);
    }
  }
}

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

  delay(2000);
  
    //setup my core 1 task
  xTaskCreatePinnedToCore(
      serial_task, //task callback
      "SerialTask",// name of the task 
      10000, //task stack size
      NULL,  //task input parameter
      0,     //Priority of the task - 0 = lowest */
      &serialTask, //task handle
      1);  //task core (0 is available, 1 is used by arduino loop()

    //setup my core 0 task
  xTaskCreatePinnedToCore(
      display_task, //task callback
      "DisplayTask",// name of the task 
      10000, //task stack size
      NULL,  //task input parameter
      1,     //Priority of the task - 0 = lowest */
      &displayTask, //task handle
      0);  //task core (0 is available, 1 is used by arduino loop()

  
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(10000);

}


//now let's do 2x2 scaling
void loop() {
 delay(500); 
 
}

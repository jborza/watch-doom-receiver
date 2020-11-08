#include "config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/task.h>
#include <freertos/queue.h>

TTGOClass *ttgo;
TFT_eSPI *tft;

#define INVERT_COLORS


#define DISPLAY_LINE_LENGTH 240
#define RECEIVE_LINE_BITS 120
#define RECEIVE_LINE_BYTES 120/8

int y = 0;
int Y_MAX = 240;
int y_offset = 0;
char rxBuffer[RECEIVE_LINE_BYTES];
char rxBufferTask[RECEIVE_LINE_BYTES];
uint16_t lineBuffer[DISPLAY_LINE_LENGTH];

TaskHandle_t displayTask;
QueueHandle_t vidQueue;
QueueHandle_t doneQueue;


void display_task( void * parameter) {
  int *queueData = NULL;
  //notify UART we can receive
  xQueueSend(doneQueue, &queueData, ( TickType_t ) 10);
  for(;;) {
    if(xQueueReceive(vidQueue, &queueData, ( TickType_t ) 10) == pdPASS){
       tft->setAddrWindow(0, y, DISPLAY_LINE_LENGTH, 2);
      //convert 120 received bits to 240 line pixels
      //convert my rxBuffer to black and white pixels
      for(int i = 0; i < RECEIVE_LINE_BITS; i++){
        //bit 0 -> pixel 0,1; bit 1 -> pixel 1,2
        int remainder = 7 - i % 8;
    #ifdef INVERT_COLORS
        int color = (((rxBufferTask[i / 8] >> (remainder)) & 1) == 1 ? 0xFFFF : 0);
    #else
        int color = (((rxBufferTask[i / 8] >> (remainder)) & 1) == 1 ? 0 : 0xFFFF);
    #endif
        lineBuffer[i*2] = color;
        lineBuffer[i*2+1] = color;
      }
      tft->startWrite();
      tft->pushPixels(lineBuffer, DISPLAY_LINE_LENGTH);
      tft->pushPixels(lineBuffer, DISPLAY_LINE_LENGTH);
      tft->endWrite();
      //we need to notify the UART task that we can receive data
      xQueueSend(doneQueue, &queueData, ( TickType_t ) 10);
    }
  }
}

void setup() {

  //Set up the display
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();

  tft = ttgo->tft;

  //render queue
  vidQueue = xQueueCreate(1, sizeof(uint32_t *));
  //done queue
  doneQueue = xQueueCreate(1, sizeof(uint32_t *));
  

  //setup my core 0 task
  xTaskCreatePinnedToCore(
      display_task, //task callback
      "DisplayTask",// name of the task 
      10000, //task stack size
      NULL,  //task input parameter
      0,     //Priority of the task - 0 = lowest */
      &displayTask, //task handle
      0);  //task core (0 is available, 1 is used by arduino loop()

  //splash screen
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextSize(2);
  ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->setCursor(40, 5);
  ttgo->tft->println(F("Serial @ 250000"));
  Serial.begin(250000);

  delay(2000);
}

int temp = 4;

//now let's do 2x2 scaling
void loop() {
  int *queueData = &temp;
  int *doneQueueData;
  //pixel doubling loop
  //receive 120 pixels (120 bits = 15 bytes) over serial port
  //then push two rows of pixels to the screen
  //Serial.print("?");
  Serial.readBytes(rxBuffer, RECEIVE_LINE_BYTES);
  //wait to be notified by the 
  xQueueReceive(doneQueue, queueData, ( TickType_t ) 10);
  memcpy(rxBufferTask, rxBuffer, RECEIVE_LINE_BYTES);
  xQueueSend(vidQueue, queueData, ( TickType_t ) 10); //wait for 10 ticks (how long?)
  //Serial.print(".");
  
  y+=2;
  if(y >= Y_MAX){
    y = 0;    
  }
}

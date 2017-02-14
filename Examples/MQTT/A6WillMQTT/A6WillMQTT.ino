/*
 *   Using GPRS A6 development board, powewr comes from USB so no need to supply from Arduino
 *   RX/TX always connect to a hardware Serial port so we can utilize serialevent
 *   PWR always connected to Arduino VCC, RESET to TRANSISTOR_CONTROL
 *   
 *   Simple example publishes and subscribes to the same topic - see testTopic
 *   If using a public broker, choose a unique topic so you dont get swamped by other peoples stuff
 */
#include "A6Services.h"
#include "A6MQTT.h"

char buff[100];    
#define APN "uinternet"  // write your APN here

A6GPRS gsm(Serial1,500,200);    // allocate 500 byte circular buffer, largest message 200 bytes
#define KEEP_ALIVE_TIME 30
#define MAX_MQTT_MESSAGE_LENGTH  100
A6MQTT MQTT(gsm,KEEP_ALIVE_TIME,MAX_MQTT_MESSAGE_LENGTH);

uint32_t nextpublish;
char *willtopic = "Henry/will";
char *willmessage = "byebye2";

char imei[20];
#define PUB_DELTA 25000 // publish every 20 secs
bool allowConnect = true;   // only allow the first connect
#define DEBUG_SERIAL Serial
void setup() {
  DEBUG_SERIAL.begin(115200);
  Serial1.begin(115200);
   // A6 uses default baud 115200
   // power up the board, do hardware reset & get ready to execute commands
   DEBUG_SERIAL.println("A6 MQTT Will demo");
   DEBUG_SERIAL.print("Open up a client and subscribe to ");
   DEBUG_SERIAL.println(willtopic);
   DEBUG_SERIAL.print(KEEP_ALIVE_TIME); DEBUG_SERIAL.println(" secs after connecting to broker the Will message should appear");
   
  if (gsm.begin()) 
  {
    DEBUG_SERIAL.println("GSM up");
    // we need a unique userid when logging on to the broker. We also need a unique topic
    // name. We'll use this devices IMIE tp get that.
    if (!gsm.getIMEI(imei))
      strcpy(imei,"defaultime");
    // setup GPRS connection with your provider
    if (gsm.startIP(APN))
    {
      DEBUG_SERIAL.println("IP up");
      // AutoConnect sets up TCP session with the broker and makes a user connection
      MQTT.AutoConnect();
    }
    else
      DEBUG_SERIAL.println("IP down");
  }
  else
    DEBUG_SERIAL.println("GSM down");
}
void loop() {
  /*
   * We send nothing which will cause the broker to disconnect us after KeepAlive seconds
   * THe Will & Testament will be sent to anyone subscrobed
   */
  if (gsm.connectedToServer || MQTT.waitingforConnack)
  {
    byte *mm;
    unsigned l;
    mm = gsm.Parse(&l);
    if (l != 0)
      MQTT.Parse(mm,l);
  }
  else
    MQTT.AutoConnect();
}


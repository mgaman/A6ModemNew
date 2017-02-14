/*
 *   Using GPRS A6 development board, powewr comes from USB so no need to supply from Arduino
 *   RX/TX always connect to a hardware Serial port so we can utilize serialevent
 *   PWR always connected to Arduino VCC, RESET to TRANSISTOR_CONTROL
 *   
 *   Simple example publishes and subscribes to the same topic - see testTopic
 *   If using a public broker, choose a unique topic so you dont get swamped by other peoples stuff
 */

 /*
  * this exasmple does not ppublish any messages but ping keeps the connection alive,
  * To show what happens if no ping is sent , comment out the next line
  * After KEEP_ALIVE_TIME secconds there will be a disconnect  event followed by reconnect
  * You may send publish messages using the same parameters from another MQTT client and you will see the message
  * displayed on the serial terminal. However it doesnt stop the disconnect mechanism working
  */
//#define KEEP_PINGING // comment out to demonstrate the effect of not ping and the connection going down
#include "A6Services.h"
#include "A6MQTT.h"

char buff[60];    // We must send at least 1 packet within the chosen keepalive time
                  // If you have nothing else to send, send at least a  ping
#define APN "uinternet"  // write your APN here
A6GPRS gsm(Serial1,500,200);    // allocate 500 byte circular buffer, largest message 200 bytes

#define KEEP_ALIVE_TIME 30
#define MAX_MQTT_MESSAGE_LENGTH  100
A6MQTT MQTT(gsm,KEEP_ALIVE_TIME,MAX_MQTT_MESSAGE_LENGTH);

char imei[20];

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
   // A6 uses default baud 115200
   // power up the board, do hardware reset & get ready to execute commands
  // gsm.enableDebug = true;
  if (gsm.begin()) 
  {
    gsm.debugWrite("GSM up");
    // we need a unique userid when logging on to the broker. We also need a unique topic
    // name. We'll use this devices IMIE to get that.
    if (!gsm.getIMEI(imei))
      strcpy(imei,"defaultime");
    // setup GPRS connection with your provider
    if (gsm.startIP(APN))
    {
      gsm.debugWrite("IP up");
      // AutoConnect sets up TCP session with the broker and makes a user connection
      MQTT.AutoConnect();
    }
    else
      gsm.debugWrite("IP down");
  }
  else
    gsm.debugWrite("GSM down");
}

uint16_t messageid = 0;
void loop() {
  byte *mm;
  unsigned l;
  /*
   *  THe condition below checks if the modem is able to process data from the broker
   */
  if (gsm.connectedToServer || MQTT.waitingforConnack)
  {
    /*
     * This machanism ensures that the connection with the broker is not disconnected du to inactivirt
     */
#ifdef KEEP_PINGING
    if (MQTT._PingNextMillis < millis())
    {
      if (MQTT.ping())
      {
        sprintf(buff,"RX %lu TX %lu",gsm.rxcount,gsm.txcount);
        gsm.debugWrite(buff);
      }
    }
#endif
    /*
     * Process data received from the broker
     * Note that this may be mixed up with unsolicited messages from the modem. Parse takes car of that
     */
    mm = gsm.Parse(&l);
    if (l != 0)
    {
      sprintf(buff,"Raw data length %u\r\n",l);
      gsm.debugWrite(buff);
      MQTT.Parse(mm,l);
    }
  }
  else
     MQTT.AutoConnect();
}



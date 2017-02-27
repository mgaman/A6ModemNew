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
#define BROKER_ADDRESS "test.mosquitto.org"  // public broker
#define BROKER_PORT 1883

uint32_t nextpublish;
char *willtopic = "Henry/will";
char *willmessage = "byebye";

char imei[20];
#define PUB_DELTA 25000 // publish every 20 secs
bool allowConnect = true;   // only allow the first connect
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
   // A6 uses default baud 115200
   // power up the board, do hardware reset & get ready to execute commands
   Serial.println("A6 MQTT Will demo");
   Serial.print("Open up a client and subscribe to ");
   Serial.println(willtopic);
   Serial.print(KEEP_ALIVE_TIME); Serial.println(" secs after connecting to broker the Will message should appear");
   
  if (gsm.begin()) 
  {
    Serial.println("GSM up");
    // we need a unique userid when logging on to the broker. We also need a unique topic
    // name. We'll use this devices IMIE tp get that.
    if (!gsm.getIMEI(imei))
      strcpy(imei,"defaultime");
    // setup GPRS connection with your provider
    if (gsm.startIP(APN))
      Serial.println("IP up");
    else
      Serial.println("IP down");
  }
  else
    Serial.println("GSM down");
}
void loop() {
    byte *mm;
    unsigned l;
  /*
   * We send nothing which will cause the broker to disconnect us after KeepAlive seconds
   * THe Will & Testament will be sent to anyone subscrobed
   */
  AutoConnect();
  mm = gsm.Parse(&l);
  if (l != 0)
    MQTT.Parse(mm,l);
}

void serialEvent1() {
  while (Serial1.available())
    gsm.push((char)Serial1.read());
}

/*
 * This function is called once in main setup
 * OnDisconnect below also calls AutoConnect but it is not coumpulsory
 */
void AutoConnect()
{
  if (!gsm.connectedToServer)
  {
    if (allowConnect)
    {
      if (gsm.connectTCPserver(BROKER_ADDRESS,BROKER_PORT))
      {
        allowConnect = false;
        Serial.println("TCP up");
        // connect, no userid, password or Will
        MQTT.connect(imei,false,             // broker IP address
           false, false, "", "",                                // no credentials
           true, MQTT.QOS_0, false, willtopic, willmessage); // Will stuff
        
      }
      else
        Serial.println("TCP down");
    }
    else
      Serial.println("Already Done");
  }
}



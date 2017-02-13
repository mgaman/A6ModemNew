/*
 * None of these callback routines HAVE to be implemented but it makes life a lot easier if they are
 * AutoConnect kicks off the whole process and can be recalled when necessary e.g. if the broker disconnects
 * OnConnect is called when a CONNACK was received. Do not assume that the connection was successful - check it
 * OnSubscribe is called when a subscribe request ccompleted successfuilly
 * OnMessage is called when a publish message was received.
 * OnPubAck is called when a publish was isssued with QOS > 0 and completed successfuilly
 */

#include <Arduino.h>
#include "A6Services.h"
#include "A6MQTT.h"

#define BROKER_ADDRESS "test.mosquitto.org"  // public broker
//#define BROKER_ADDRESS "m2m.eclipse.org"  // public broker
#define BROKER_PORT 1883
extern char imei[];
extern A6MQTT MQTT;
extern uint16_t messageid;
char *topic = "blahblah";

static char linebuff[50];
#define PACKET_ID 12345
/*
 * This function is called once in main setup
 */
void A6MQTT::AutoConnect()
{
  if (!gsm.connectedToServer)
  {
    if (gsm.connectTCPserver(BROKER_ADDRESS,BROKER_PORT))
    {
      Serial.println("TCP up");
      // connect, no userid, password or Will
      Serial.print("Connect with ID ");
      Serial.println(imei);
      MQTT.waitingforConnack = connect(imei, false);
    }
    else
      Serial.println("TCP down");
  }
}

/*
 * This function ic called upon receiving a CONNACK message
 * Note that you should not assume that the connection was successful - check it!
 */
void A6MQTT::OnConnect(enum eConnectRC rc)
{
  switch (rc)
  {
    case MQTT.CONNECT_RC_ACCEPTED:
      Serial.print("Connected to broker ");
      Serial.println(BROKER_ADDRESS);
      MQTT._PingNextMillis = millis() + (MQTT._KeepAliveTimeOut*1000) - 2000;
      MQTT.subscribe(PACKET_ID,topic,MQTT.QOS_1);
      Serial.print("Subscribed to topic ");
      Serial.println(topic);
      Serial.println("Publish something from your client and it should appear here.");
     break;
    case MQTT.CONNECT_RC_REFUSED_PROTOCOL:
      Serial.println("Protocol error");
      break;
    case MQTT.CONNECT_RC_REFUSED_IDENTIFIER:
      Serial.println("Identity error");
      break;
  }
}

/*
 * Called if the subscribe request completed OK
 */
void A6MQTT::OnSubscribe(uint16_t pi)
{
  sprintf(linebuff,"Subscribed to packet id %u, response %u",PACKET_ID,pi);
  Serial.println(linebuff);
}

/*
 * Called when a piblish message is received.
 */
void A6MQTT::OnMessage(char *topic,char *message,bool dup, bool ret,A6MQTT::eQOS qos)
{
  if (dup)
    Serial.print("DUP ");
  if (ret)
    Serial.print("RET ");
  Serial.print("QOS ");
  Serial.println(qos);
  Serial.print("Topic: ");Serial.println(topic);
  Serial.print("Message: ");Serial.println(message);
}

/*
 * This function when the the client published a message with QOS > 0 and received confirmation that
 * publish completed OK
 */
void A6MQTT::OnPubAck(uint16_t mid)
{
  sprintf(linebuff,"Message %u published, %u acknowledged",messageid,mid);
  Serial.println(linebuff);
}


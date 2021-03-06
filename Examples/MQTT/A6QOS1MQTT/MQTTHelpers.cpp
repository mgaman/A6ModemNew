/*
 * None of these callback routines HAVE to be implemented but it makes life a lot easier if they are
 * AutoConnect kicks off the whole process and can be recalled when necessary e.g. if the broker disconnects
 * OnConnect is called when a CONNACK was received. Do not assume that the connection was successful - check it
 * OnSubscribe is called when a subscribe request ccompleted successfuilly
 * OnMessage is called when a publish message was received.
 * OnPubAck is called when a publish was isssued with QOS > 0 and completed successfuilly
 * OnDisconnect is called when the TCP connection to the broker disconnected (for whatever reason)
 */

 /*
  * In this example MTT.QOS_1 may be replaced by MTT.QOS_2
  * You should see the same results
  */
#include <Arduino.h>
#include "A6Services.h"
#include "A6MQTT.h"

extern char topic[];
extern char imei[];
//extern A6MQTT MQTT;
//extern uint16_t messageid;
extern bool setupDone;

static char linebuff[50];
/*
 * This function ic called upon receiving a CONNACK message
 * Note that you should not assume that the connection was successful - check it!
 */
#define PACKET_ID 1234
void A6MQTT::OnConnect(enum eConnectRC rc)
{
  switch (rc)
  {
    case CONNECT_RC_ACCEPTED:
      Serial.print("Connected to broker ");
      _PingNextMillis = millis() + (_KeepAliveTimeOut*1000) - 2000;
      Serial.print("Subscribing to: ");Serial.println(topic);
      subscribe(PACKET_ID,topic,QOS_1);
     break;
    case CONNECT_RC_REFUSED_PROTOCOL:
      Serial.println("Protocol error");
      break;
    case CONNECT_RC_REFUSED_IDENTIFIER:
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
  setupDone = true;
}

/*
 * Called when a piblish message is received.
 */
void A6MQTT::OnMessage(char *topic,char *message,bool dup, bool ret,A6MQTT::eQOS qos,uint16_t mid)
{
  if (dup)
    Serial.print("DUP ");
  if (ret)
    Serial.print("RET ");
  Serial.print("QOS ");
  Serial.println(qos);
  if (qos > QOS_0)
  {
    Serial.print("Message Id ");
    Serial.println(mid);
  }
  Serial.print("Topic: ");Serial.println(topic);
  Serial.print("Message: ");Serial.println(message);
}

/*
 * This function when the the client published a message with QOS > 0 and received confirmation that
 * publish completed OK
 */
void A6MQTT::OnPubAck(uint16_t mid)
{
  sprintf(linebuff,"Message %u acknowledged",mid);
  Serial.println(linebuff);
}

void A6MQTT::OnPubComplete(uint16_t mid)
{
  sprintf(linebuff,"Message %u completed",mid);
  Serial.println(linebuff);
}


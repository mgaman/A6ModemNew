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

extern char imei[];
extern uint16_t messageid;

static char linebuff[50];
#define PACKET_ID 12345

/*
 * This function ic called upon receiving a CONNACK message
 * Note that you should not assume that the connection was successful - check it!
 */
void A6MQTT::OnConnect(enum eConnectRC rc)
{
  char actualtopic[30];
  strcpy(actualtopic,imei);
  strcat(actualtopic,"/test");
  switch (rc)
  {
    case CONNECT_RC_ACCEPTED:
      Serial.print("Connected to broker ");
      _PingNextMillis = millis() + (_KeepAliveTimeOut*1000) - 2000;
      subscribe(PACKET_ID,actualtopic,QOS_1);
      Serial.print("Subscribed to topic: ");
      Serial.println(actualtopic);
      Serial.println("Publish something from your client and it should appear here.");
     break;
    case CONNECT_RC_REFUSED_PROTOCOL:
      Serial.println("Protocol error");
      break;
    case CONNECT_RC_REFUSED_IDENTIFIER:
      Serial.println("Identity error");
      break;
    default:
      Serial.print("Unknown: ");Serial.println(rc);
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
void A6MQTT::OnMessage(char *topic,char *message,bool dup, bool ret,A6MQTT::eQOS qos,uint16_t msgid)
{
  if (dup)
    Serial.print("DUP ");
  if (ret)
    Serial.print("RET ");
  Serial.print("QOS ");
  Serial.println(qos);
  if (qos > QOS_0)
  {
    Serial.print("MsgId ");
    Serial.println(msgid);
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
  sprintf(linebuff,"Message %u published, %u acknowledged",messageid,mid);
  Serial.println(linebuff);
}


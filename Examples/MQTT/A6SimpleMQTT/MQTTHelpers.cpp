/*
 * None of these callback routines HAVE to be implemented but it makes life a lot easier if they are
 * AutoConnect kicks off the whole process and can be recalled when necessary e.g. if the broker disconnects
 * OnConnect is called when a CONNACK was received. Do not assume that the connection was successful - check it
 * OnSubscribe is called when a subscribe request ccompleted successfuilly
 * OnMessage is called when a publish message was received.
 * OnPubAck is called when a publish was isssued with QOS > 0 and completed successfuilly
 * OnDisconnect is called when the TCP connection to the broker disconnected (for whatever reason)
 */
#include <Arduino.h>
#include <A6Services.h>
#include <A6MQTT.h>

extern char imei[],topic[];
extern A6GPRS gsm;
extern char buff[]; 
extern uint32_t nextpublish;
#define PUB_DELTA 25000 // publish every 20 secs
extern bool setupFinished;
/*
 * This function is called once in main setup
 * OnDisconnect below also calls AutoConnect but it is not coumpulsory
 */
/*
 * This function ic called upon receiving a CONNACK message
 * Note that you should not assume that the connection was successful - check it!
 */
void A6MQTT::OnConnect(enum eConnectRC rc)
{
  switch (rc)
  {
    case CONNECT_RC_ACCEPTED:
      Serial.println("Connected to broker ");
      _PingNextMillis = millis() + (_KeepAliveTimeOut*1000) - 2000;
     Serial.print("Subscribing to ");Serial.print(topic);
     if (subscribe(1234,topic,QOS_0))
       Serial.println(" succeeded");
     else
       Serial.println(" failed");
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
  Serial.print("Subscribed to ");
  Serial.println(pi);
  setupFinished = true;
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
    Serial.print("Message ID ");
    Serial.println(mid);
  }
  Serial.print("Topic: ");Serial.println(topic);
  Serial.print("Message: ");Serial.println(message);
  sprintf(buff,"RX %lu TX %lu",gsm.rxcount,gsm.txcount);
  Serial.println(buff);
}

/*
 * This function when the the client published a message with QOS > 0 and received confirmation that
 * publish completed OK
 */
void A6MQTT::OnPubAck(uint16_t messageid)
{
  Serial.print("Packet ");
  Serial.print(messageid,HEX);
  Serial.println(" Acknowledged");
}


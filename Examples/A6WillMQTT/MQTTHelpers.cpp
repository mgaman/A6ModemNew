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
#include "A6Services.h"
#include "A6MQTT.h"
#include <SoftwareSerial.h>

#define BROKER_ADDRESS "test.mosquitto.org"  // public broker
#define BROKER_PORT 1883
extern char *willtopic;
extern char *willmessage;
extern char imei[];
extern A6MQTT MQTT;
extern char buff[]; 
extern bool allowConnect;

#define DEBUG_SERIAL Serial

/*
 * This function is called once in main setup
 * OnDisconnect below also calls AutoConnect but it is not coumpulsory
 */
void A6MQTT::AutoConnect()
{
  if (allowConnect)
  {
    if (gsm.connectTCPserver(BROKER_ADDRESS,BROKER_PORT))
    {
      allowConnect = false;
      DEBUG_SERIAL.println("TCP up");
      // connect, no userid, password or Will
      MQTT.waitingforConnack = connect(imei,false,             // broker IP address
         false, false, "", "",                                // no credentials
         true, A6MQTT::QOS_0, false, willtopic, willmessage); // Will stuff
    }
    else
      DEBUG_SERIAL.println("TCP down");
  }
  else
    DEBUG_SERIAL.println("No connect allowed");
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
      DEBUG_SERIAL.println("Connected to broker");
     break;
    case MQTT.CONNECT_RC_REFUSED_PROTOCOL:
      DEBUG_SERIAL.println("Protocol error");
      break;
    case MQTT.CONNECT_RC_REFUSED_IDENTIFIER:
      DEBUG_SERIAL.println("Identity error");
      break;
  }
}

/*
 * Called if the subscribe request completed OK
 */
void A6MQTT::OnSubscribe(uint16_t pi)
{
  DEBUG_SERIAL.print("Subscribed to ");
  DEBUG_SERIAL.println(pi);
}

/*
 * Called when a piblish message is received.
 */
void A6MQTT::OnMessage(char *topic,char *message,bool dup, bool ret,A6MQTT::eQOS qos)
{
  if (dup)
    DEBUG_SERIAL.print("DUP ");
  if (ret)
    DEBUG_SERIAL.print("RET ");
  DEBUG_SERIAL.print("QOS ");
  DEBUG_SERIAL.println(qos);
  DEBUG_SERIAL.print("Topic: ");DEBUG_SERIAL.println(topic);
  DEBUG_SERIAL.print("Message: ");DEBUG_SERIAL.println(message);
  sprintf(buff,"RX %lu TX %lu",gsm.rxcount,gsm.txcount);
  DEBUG_SERIAL.println(buff);
}

/*
 * This function when the the client published a message with QOS > 0 and received confirmation that
 * publish completed OK
 */
void A6MQTT::OnPubAck(uint16_t messageid)
{
  DEBUG_SERIAL.print("Packet ");
  DEBUG_SERIAL.print(messageid,HEX);
  DEBUG_SERIAL.println(" Acknowledged");
}


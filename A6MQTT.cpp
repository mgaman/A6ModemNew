/*
  MQTT.h - Library for GSM MQTT Client.
  Created by Nithin K. Kurian, Dhanish Vijayan, Elementz Engineers Guild Pvt. Ltd, July 2, 2016.
  Released into the public domain.
*/
/*
 *   add Reset function -- see begin()
 *   print tcpstatus correctly
 *   Get APN from main program
 *   Replace all Serial by HW_SERIAL
 *   Set HW serial via macro #define HW_SERIAL Serial1  etc
 *   Replace all serialEvent by SERIALEVENT
 *   Set hw serial event via macro #define SERIALEVENT serialEvent1 etc
 *   
 *   change MQTT_PORT to int
 *   Add USERID, PASSWORD
 *   Replace printMessageType and printConnectAck by printConstString
 *   Bugfix replace 200 by UART_BUFFER_LENGTH in SERIALEVENT
 *   
 *   Changed all mySerial.print to new method DebugPrint. Now user must implement
 *   DebugPrint as he sees fit
 */
#include "Arduino.h"
#include "A6Services.h"
#include "A6MQTT.h"
#include <avr/pgmspace.h>

A6MQTT::A6MQTT(A6GPRS& a6gprs,unsigned long KeepAlive,unsigned MaxMessageLength)
{
  _KeepAliveTimeOut = KeepAlive;
//  gsm.doParsing = false;  // dont parse until connected to server
  modemMessageLength = 0;
  ParseState = GETMM;
  _PingNextMillis = 0xffffffff;
 // connectedToServer = false;  // got ACK & it was  CONNECTION_ACCEPTED
  waitingforConnack = false;  // send connection request and waiting for ACK
  maxmessagelength = MaxMessageLength;
  CombinedTopicMessageBuffer = new byte[maxmessagelength];
  modemmessage = new byte[maxmessagelength];
  _a6gprs = &a6gprs;
}

#define MQTT_BUFFER_SIZE 50
static byte mqttbuffer[MQTT_BUFFER_SIZE];  // build up message here
static char line[30];
uint16_t bswap(uint16_t w)
{
  return ((w&0xff)*256) + (w/256);
}
char *Protocolname = "MQTT";
// Using version 3.1.1  Protocol name MQTT, version 4
bool A6MQTT::connect(char *ClientIdentifier, bool CleanSession, bool UserNameFlag, bool PasswordFlag, char *UserName, char *Password, bool WillFlag, eQOS WillQoS, bool WillRetain, char *WillTopic, char *WillMessage)
{
 // ConnectionAcknowledgement = MQ_NO_ACKNOWLEDGEMENT ;
  // calculate overall size of connect headers + payload;
  int connsize = sizeof(struct sFixedHeader)+sizeof(struct sConnectVariableHeader)+sizeof(uint16_t)+strlen(ClientIdentifier);
  if (UserNameFlag)
    connsize+= sizeof(uint16_t) + strlen(UserName);
  if (PasswordFlag)
    connsize+= sizeof(uint16_t) + strlen(Password);
  if (WillFlag)
    connsize+= sizeof(uint16_t) + strlen(WillTopic) + sizeof(uint16_t) + strlen(WillMessage);
  struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
  int bindex = 0;
  pFH->dup = 0;
  pFH->qos = 0;
  pFH->retain = 0;
  pFH->controlpackettype = MQ_CONNECT;
  pFH->rl = connsize-sizeof(struct sFixedHeader); // do not encode in data
  bindex += sizeof(struct sFixedHeader);
  struct sConnectVariableHeader *pVH = (struct sConnectVariableHeader *)&mqttbuffer[bindex];
  pVH->length = bswap(strlen(Protocolname));
  strcpy(pVH->pname,Protocolname);
  pVH->protocolLevel = 4; // always
  pVH->connectFlags = (CleanSession ? CLEAN_SESSION_FLAG : 0) |
                        (WillFlag ? WILL_FLAG : 0) | (WillFlag ? WillQoS<<3 : 0 ) | (WillFlag ? WILL_RETAIN : 0) |
                        (UserNameFlag ? USERNAME_FLAG : 0) |
                        (PasswordFlag ? USERNAME_FLAG : 0);
  pVH->keepalive = bswap(_KeepAliveTimeOut);
  bindex += sizeof(struct sConnectVariableHeader);
  struct sVariableString *pVS = (struct sVariableString *)&mqttbuffer[bindex];
  pVS->length = bswap(strlen(ClientIdentifier));
  strcpy(pVS->string,ClientIdentifier);
  bindex += sizeof(int16_t) + strlen(ClientIdentifier);
  if (WillFlag)
  {
    pVS = (struct sVariableString *)&mqttbuffer[bindex];
    pVS->length = bswap(strlen(WillTopic));
    strcpy(pVS->string,WillTopic);
    bindex += sizeof(int16_t) + strlen(WillTopic);
    pVS = (struct sVariableString *)&mqttbuffer[bindex];
    pVS->length = bswap(strlen(WillMessage));
    strcpy(pVS->string,WillMessage);
    bindex += sizeof(int16_t) + strlen(WillMessage);
  }
  if (UserNameFlag)
  {
    pVS = (struct sVariableString *)&mqttbuffer[bindex];
    pVS->length = bswap(strlen(UserName));
    strcpy(pVS->string,UserName);
    bindex += sizeof(int16_t) + strlen(UserName);
  }
  if (PasswordFlag)
  {
    pVS = (struct sVariableString *)&mqttbuffer[bindex];
    pVS->length = bswap(strlen(Password));
    strcpy(pVS->string,Password);
    bindex += sizeof(int16_t) + strlen(Password);
  }
  return _a6gprs->sendToServer(mqttbuffer,connsize);
}

bool A6MQTT::connect(char *ClientIdentifier, bool CleanSession)
{
	return connect(ClientIdentifier,CleanSession,false,false,NULL,NULL,false,A6MQTT::QOS_0,false,NULL,NULL);
}
bool A6MQTT::subscribe(unsigned int MessageID, char *SubTopic, eQOS SubQoS)
{
  bool rc = false;
  if (_a6gprs->connectedToServer)
  {
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    int bindex = 0;
    pFH->dup = 0;
    pFH->qos = 1;
    pFH->retain = 0;
    pFH->controlpackettype = MQ_SUBSCRIBE;
    pFH->rl = sizeof(struct sSubscribeVariableHeader)+sizeof(uint16_t)+strlen(SubTopic) + 1; // includes QOS b 
    bindex += sizeof(struct sFixedHeader);
    struct sSubscribeVariableHeader *pVH = (struct sSubscribeVariableHeader *)&mqttbuffer[bindex];
    pVH->packetid = bswap(MessageID);
    bindex += sizeof(struct sSubscribeVariableHeader);
    struct sVariableString *pVS = (struct sVariableString *)&mqttbuffer[bindex];
    pVS->length = bswap(strlen(SubTopic));
    strcpy(pVS->string,SubTopic);
    bindex += sizeof(int16_t) + strlen(SubTopic);
    mqttbuffer[bindex] = SubQoS; 
    rc = _a6gprs->sendToServer(mqttbuffer,pFH->rl + sizeof(struct sFixedHeader));
  }
  return rc;
}

/*
 *  Serial input is a mixture of modem unsolicited messages e.g. +CGREG: 1, expected messages
 *  suxh as the precursor to data +CIPRCV:n, and data from the broker
 *  We look for complete modem messages & react to +CIPRCV, tgr4ansfer n bytes tp mqtt parser
 *  wait for cr/lf cr , as terminators 
 */
void A6MQTT::Parse()
{
  char c = _a6gprs->pop();
  while (c != -1)
  {
    switch (ParseState)
    {
      case GETMM:
        modemmessage[modemMessageLength++] = c;
        if (c==0x0a || c== 0x0d || c== ':') // expected delimiter
        {
          if (modemMessageLength == strlen("+CIPRCV:") && strncmp(modemmessage,"+CIPRCV:",8) == 0)
          {
            ParseState = GETLENGTH;
            modemMessageLength = 0;
          }
          else if (modemMessageLength == strlen("+TCPCLOSED:") && strncmp(modemmessage,"+TCPCLOSED:",11) == 0)
          {
            _a6gprs->debugWrite(F("Server closed connection\r\n"));
            _a6gprs->connectedToServer = false;
            _a6gprs->stopIP();
            _a6gprs->getCIPstatus();
            OnDisconnect();
          }
          else
            modemMessageLength = 0; // just discard      
        }
        break;
      case GETLENGTH:
        modemmessage[modemMessageLength++] = c;
        if (c == ',')
        {
          modemmessage[modemMessageLength] = 0;
          mqttmsglength = atoi(modemmessage);
		  _a6gprs->rxcount += mqttmsglength;
          modemMessageLength = 0;
          ParseState = GETDATA;
        }
        break;
      case GETDATA:
        if (mqttmsglength <= MQTT_BUFFER_SIZE)   // only copy data if there is space
          modemmessage[modemMessageLength++] = c;
        else
          modemMessageLength++;   // else just discard
        if (modemMessageLength == mqttmsglength)
        {
          ParseState = GETMM;
          modemMessageLength = 0;
          mqttparse();
        }
        break;
    }
    c = _a6gprs->pop();
  }
}

/*
 * buffer modemmessage contains mqttmsglength bytes
 */
void A6MQTT::mqttparse()
{
  struct sFixedHeader *pFH = (struct sFixedHeader *)modemmessage;
  struct sSubackVariableHeader *pSVH;
  struct sVariableString *pVS;
  int16_t slengtht,slengthm;  // topic and message lengths
  uint16_t *pW,pi;
  _a6gprs->debugWrite(F("<<"));
  for (int ii=0;ii<pFH->rl+2;ii++)
  {
    sprintf(line,"%02X,",modemmessage[ii]);
    _a6gprs->debugWrite(line);
  }
  _a6gprs->debugWrite(F("\r\n"));
  switch (pFH->controlpackettype)
  {
    case MQ_CONNACK:
      if (pFH->rl == 2)
      {
        // skip to next part of message
        struct sConnackVariableHeader *pCAVH = (struct sConnackVariableHeader *)&modemmessage[sizeof(struct sFixedHeader)];
        _a6gprs->connectedToServer = pCAVH->returncode == CONNECT_RC_ACCEPTED;
        waitingforConnack = false;
        OnConnect(pCAVH->returncode);
      }
      break;
    case MQ_SUBACK:
      pSVH = (struct sSubackVariableHeader *)&modemmessage[sizeof(struct sFixedHeader)];
      _PingNextMillis = millis() + (_KeepAliveTimeOut*1000) - 2000;
      OnSubscribe(bswap(pSVH->packetid));
      break;
    case MQ_PUBLISH:
      pVS = (struct sVariableString *)&modemmessage[sizeof(struct sFixedHeader)];
      slengtht = bswap(pVS->length);
      memcpy(CombinedTopicMessageBuffer,pVS->string,slengtht);  // copy out topic
      CombinedTopicMessageBuffer[slengtht] = 0;  // add end marker
	  // get the packet index (if any)
	  if (pFH->qos > QOS_0)
	  {
		pi = *(uint16_t *)(pVS->string+slengtht);
		slengthm = pFH->rl-slengtht-(2*sizeof(int16_t));
		memcpy(&CombinedTopicMessageBuffer[slengtht+1],pVS->string+slengtht+sizeof(uint16_t),slengthm);  
	  }
	  else
	  {
		slengthm = pFH->rl-slengtht-sizeof(int16_t);
		memcpy(&CombinedTopicMessageBuffer[slengtht+1],pVS->string+slengtht,slengthm);  
	  }
      CombinedTopicMessageBuffer[slengtht+slengthm+1] = 0;
	  switch (pFH->qos)
	  {
		  case QOS_0:  // do nothing
			break;
		  case QOS_1:  // send a PUBACK
			puback(pi);
			break;
		  case QOS_2:  // send a PUBREL
			pubcomp(pi);
		    break;
	  }
      OnMessage(CombinedTopicMessageBuffer,&CombinedTopicMessageBuffer[slengtht+1],pFH->dup,pFH->retain,pFH->qos ); // dup & retain flags, QOS
      break;
    case MQ_PINGRESP:
      _a6gprs->debugWrite(F("ping response\r\n"));
      _PingNextMillis = millis() + (_KeepAliveTimeOut*1000) - 2000;
      break;
    case MQ_PUBACK:
      pW = (uint16_t *)&modemmessage[2];
      OnPubAck(bswap(*pW));
      break;
    case MQ_UNSUBACK:
      pW = (uint16_t *)&modemmessage[2];
      _a6gprs->debugWrite(F("unsuback: "));
      _a6gprs->debugWrite(bswap(*pW));
      _a6gprs->debugWrite(F("\r\n"));
      OnUnsubscribe(bswap(*pW));
      break;
    case MQ_PUBREC:
      pW = (uint16_t *)&modemmessage[2];
      _a6gprs->debugWrite(F("pubrec: "));
      _a6gprs->debugWrite(bswap(*pW));
      _a6gprs->debugWrite(F("\r\n"));
      pubrel(bswap(*pW));
      break;  
    case MQ_PUBCOMP:  
      pW = (uint16_t *)&modemmessage[2];
      _a6gprs->debugWrite(F("pubcomp: "));
      _a6gprs->debugWrite(bswap(*pW));
      _a6gprs->debugWrite(F("\r\n"));
      OnPubAck(bswap(*pW));
      break;  
  }
}

bool A6MQTT::ping()
{
  bool rc = false;
  if (_a6gprs->connectedToServer)
  {
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    pFH->controlpackettype = MQ_PINGREQ;
    pFH->dup = 0;
    pFH->qos = 0;
    pFH->retain = 0;
    pFH->rl = 0;
    // stop spiunning in ping loop
    _PingNextMillis = millis() + (_KeepAliveTimeOut*1000) - 2000;
    // no variable header or payload
    rc =  _a6gprs->sendToServer(mqttbuffer,2);
  }
  return rc;
}

bool A6MQTT::publish(char *Topic, char *Message)
{
  return publish(Topic,Message,false,false,A6MQTT::QOS_0,0);
}

bool A6MQTT::publish(char *Topic, char *Message, bool dup, bool ret)
{
  return publish(Topic,Message,dup,ret,A6MQTT::QOS_0,0);
}

bool A6MQTT::publish(char *Topic, char *Message, bool dup , bool retain ,eQOS qos ,uint16_t packetid) // dup,retain,qos
{
  bool rc = false;
  uint16_t *pW;
  if (_a6gprs->connectedToServer)
  {
    memset(mqttbuffer,0x55,sizeof(mqttbuffer));
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    pFH->controlpackettype = MQ_PUBLISH;
    pFH->dup = dup;
    pFH->qos = qos;
    pFH->retain = retain;
    pFH->rl = strlen(Topic) + strlen(Message) + sizeof(uint16_t);  // QOS 0 no messageid
    if (qos != QOS_0)
      pFH->rl += sizeof(int16_t);
    // copy topic as variable string
    struct sVariableString *pVS = (struct sVariableString *)&mqttbuffer[2];
    pVS->length = bswap(strlen(Topic));
    memcpy(pVS->string,Topic,strlen(Topic));
    pW = (uint16_t *)(pVS->string + strlen(Topic));  // point to where messageid will go (if needed)
    if (qos != QOS_0)  // packetid after topic if qos 1 or 2
    {
 //     sprintf(line,"\r\nPI %x %x\r\n",packetid,bswap(packetid));
 //     gsm.debugWrite(line);
      *pW++ = bswap(packetid);  // if messageid written, increment pointer
    }
    // copy message after messageid which may or may not have been written 
    memcpy((char*)pW,Message,strlen(Message));
    _PingNextMillis = millis() + (_KeepAliveTimeOut*1000) - 2000;
    rc = _a6gprs->sendToServer(mqttbuffer,pFH->rl + sizeof(struct sFixedHeader));
  }
  return rc;
}

bool A6MQTT::disconnect()
{
  bool rc = false;
  if (_a6gprs->connectedToServer)
  {
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    pFH->controlpackettype = MQ_DISCONNECT;
    pFH->dup = 0;
    pFH->qos = 0;
    pFH->retain = 0;
    pFH->rl = 0;
    _a6gprs->connectedToServer = false;
    rc = _a6gprs->sendToServer(mqttbuffer,2);
  }
  return rc;  
}

 bool A6MQTT::unsubscribe(unsigned int MessageID, char *SubTopic)
 {
  bool rc = false;
  if (_a6gprs->connectedToServer)
  {
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    pFH->controlpackettype = MQ_UNSUBSCRIBE;
    pFH->dup = 0;
    pFH->qos = 1;
    pFH->retain = 0;
    pFH->rl = (2*sizeof(int16_t)) +strlen(SubTopic); // messageid + length + string
    int16_t *pW = (int16_t *)&mqttbuffer[2];
    *pW++ = bswap(MessageID);
    struct sVariableString *pVS = (struct sVariableString *)pW;
    pVS->length = bswap(strlen(SubTopic));
    strcpy(pVS->string,SubTopic);
    rc = _a6gprs->sendToServer(mqttbuffer,pFH->rl + 2);  
  }
  return rc;
 }

 bool A6MQTT::pubrel(uint16_t pi)
 {
  bool rc = false;
  if (_a6gprs->connectedToServer)
  {
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    pFH->controlpackettype = MQ_PUBREL;
    pFH->dup = 0;
    pFH->qos = 1;
    pFH->retain = 0;
    pFH->rl = sizeof(int16_t);
    int16_t *pW = (int16_t *)&mqttbuffer[2];
    *pW = bswap(pi);
    rc = _a6gprs->sendToServer(mqttbuffer,pFH->rl + 2);  
  } 
  return rc; 
 }

bool A6MQTT::puback(uint16_t pi) // pi is BE format
{
  bool rc = false;
  if (_a6gprs->connectedToServer)
  {
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    pFH->controlpackettype = MQ_PUBACK;
    pFH->dup = 0;
    pFH->qos = 0;
    pFH->retain = 0;
    pFH->rl = sizeof(int16_t);
    int16_t *pW = (int16_t *)&mqttbuffer[2];
    *pW = pi;
    rc = _a6gprs->sendToServer(mqttbuffer,pFH->rl + sizeof(struct sFixedHeader));  
  } 
  return rc; 	
}
bool A6MQTT::pubcomp(uint16_t pi) // pi is BE format
{
  bool rc = false;
  if (_a6gprs->connectedToServer)
  {
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    pFH->controlpackettype = MQ_PUBCOMP;
    pFH->dup = 0;
    pFH->qos = 0;
    pFH->retain = 0;
    pFH->rl = sizeof(int16_t);
    int16_t *pW = (int16_t *)&mqttbuffer[2];
    *pW = pi;
    rc = _a6gprs->sendToServer(mqttbuffer,pFH->rl + sizeof(struct sFixedHeader));  
  } 
  return rc; 	
}

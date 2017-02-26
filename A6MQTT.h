/*
 * Mods:  make max mesage length parameter to constructor
*/
#ifndef A6_MQTT_H_
#define A6_MQTT_H_

// ######################################################################################################################
#define MQ_CONNECT     1   //Client request to connect to Server                Client          Server
#define MQ_CONNACK     2   //Connect Acknowledgment                             Server/Client   Server/Client
#define MQ_PUBLISH     3   //Publish message                                    Server/Client   Server/Client
#define MQ_PUBACK      4   //Publish Acknowledgment                             Server/Client   Server/Client
#define MQ_PUBREC      5   //Publish Received (assured delivery part 1)         Server/Client   Server/Client
#define MQ_PUBREL      6   //Publish Release (assured delivery part 2)          Server/Client   Server/Client
#define MQ_PUBCOMP     7   //Publish Complete (assured delivery part 3)         Server/Client   Server/Client
#define MQ_SUBSCRIBE   8   //Client Subscribe request                           Client          Server
#define MQ_SUBACK      9   //Subscribe Acknowledgment                           Server          Client
#define MQ_UNSUBSCRIBE 10  //Client Unsubscribe request                         Client          Server
#define MQ_UNSUBACK    11  //Unsubscribe Acknowledgment                         Server          Client
#define MQ_PINGREQ     12  //PING Request                                       Client          Server
#define MQ_PINGRESP    13  //PING Response                                      Server          Client
#define MQ_DISCONNECT  14  //Client is Disconnecting                            Client          Server

// QoS value bit 2 bit 1 Description
//   0       0       0   At most once    Fire and Forget         <=1
//   1       0       1   At least once   Acknowledged delivery   >=1
//   2       1       0   Exactly once    Assured delivery        =1
//   3       1       1   Reserved
#define DUP_Mask      8   // Duplicate delivery   Only for QoS>0
#define QoS_Mask      6   // Quality of Service
#define QoS_Scale     2   // (()&QoS)/QoS_Scale
#define RETAIN_Mask   1   // RETAIN flag

#define MQ_DISCONNECTED          0
#define MQ_CONNECTED             1
#define MQ_NO_ACKNOWLEDGEMENT  255

struct sFixedHeader {
  bool retain:1;
  byte qos:2;
  bool dup:1;
  byte controlpackettype:4;
  byte rl;
};
struct sConnectVariableHeader {
  uint16_t length;
  byte pname[4];  // always MQIsdp
  byte protocolLevel;
  byte connectFlags;
  uint16_t keepalive;
};

struct sConnackVariableHeader {
  byte flags;
  byte returncode;
};

struct sSubscribeVariableHeader {
  uint16_t packetid;
};

struct sSubackVariableHeader {
  uint16_t packetid; 
};

struct sSubackPayload {
  byte rc[1];
};

struct sVariableString {
  uint16_t length;
  char string[1];
};
// connectFlags masks
#define CLEAN_SESSION_FLAG (1<<1)
#define WILL_FLAG (1<<2)
#define WILL_QOS (3<<3)
#define WILL_RETAIN (1<<5)
#define PASSWORD_FLAG (1<<6)
#define USERNAME_FLAG (1<<7)

class A6MQTT
{
  public:
    enum eConnectRC {CONNECT_RC_ACCEPTED, CONNECT_RC_REFUSED_PROTOCOL,CONNECT_RC_REFUSED_IDENTIFIER};
    enum eQOS {QOS_0,QOS_1,QOS_2};
    A6MQTT(A6GPRS& a6gprs,unsigned long KeepAlive,unsigned MaxMessageLength);
 //   bool connectedToServer;  // moved to A6Services
    bool waitingforConnack;
    volatile unsigned long _PingNextMillis = 0;
    bool connect(char *ClientIdentifier, bool CleanSession, bool UserNameFlag, bool PasswordFlag, char *UserName, char *Password, bool WillFlag,
               	eQOS WillQoS, bool WillRetain, char *WillTopic, char *WillMessage);
    bool connect(char *ClientIdentifier, bool CleanSession);
	bool publish(char *Topic, char *Message); // QOS 0 no dup no retain
    bool publish(char *Topic, char *Message, bool , bool ); // dup,retain,qos=0
    bool publish(char *Topic, char *Message, bool , bool, eQOS,uint16_t packetid ); // dup,retain,qos
    bool subscribe(unsigned int MessageID, char *SubTopic, eQOS SubQoS);
    bool unsubscribe(unsigned int MessageID, char *SubTopic);
    bool disconnect(void);
    bool ping(void);
    void Parse(byte *,unsigned);
//    void AutoConnect(void);
	// callbacks
    void OnConnect(eConnectRC) __attribute__((weak));
    void OnSubscribe(uint16_t)  __attribute__((weak));
    void OnMessage(char *Topic,char *Message,bool,bool,eQOS)   __attribute__((weak));
    void OnPubAck(uint16_t messageid) __attribute__((weak));
    void OnUnsubscribe(uint16_t)  __attribute__((weak));
    A6GPRS *_a6gprs;
  private:
    void mqttparse(byte[]);
    volatile unsigned long _KeepAliveTimeOut;
    byte *CombinedTopicMessageBuffer;
    bool pubrel(uint16_t);
	bool puback(uint16_t);
	bool pubcomp(uint16_t);
    unsigned maxmessagelength;
};
#endif /* A6_MQTT_H_ */


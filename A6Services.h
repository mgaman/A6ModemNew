#ifndef A6SERVICE_H
#define  A6SERVICE_H
#include "A6ModemNew.h"

class A6GPRS: public virtual A6GPRSDevice
{
  public:
    enum eCIPstatus {IP_INITIAL,IP_START,IP_CONFIG,IP_IND,IP_GPRSACT,IP_STATUS,TCPUDP_CONNECTING,IP_CLOSE,CONNECT_OK,IP_STATUS_UNKNOWN };
    enum ePSstate { DETACHED,ATTACHED,PS_UNKNOWN };
	enum ecallState {IDLE,CALLER_RINGING,SPEAKING,DIALLING_OUT,CALLERID,DISCONNECTED};
    A6GPRS(Stream &comm,unsigned,unsigned); // constructor uartstream, circular buffer size, max message size
    ~A6GPRS(); // destructor
    bool getIMEI(char[]);
    bool getCIMI(char[]);
    bool getRTC(char[]);
    bool setRTC(char[]);
    enum eCIPstatus getCIPstatus();
    char *getCIPstatusString(enum eCIPstatus);
    char *getCIPstatusString();   // current value
    bool startIP(char []);  // apn
    bool startIP(char [],char[],char []);  // apn, username, password
    ePSstate getPSstate();
    bool setPSstate(ePSstate);
    bool stopIP();
    bool getLocalIP(char []);
    bool connectTCPserver(char*,int);
    bool sendToServer(char[]); // single C string
    bool sendToServer(char*[],unsigned); // array of C strings
    bool sendToServer(char[],int);  // for ascii strings, no delimiter
    bool sendToServer(byte[],int);  // for byte arrays
	bool connectedToServer;
	byte *Parse(unsigned *);
	bool dial(char[]);
	bool answer();
	bool hangup();
	bool callerID(bool);
	enum ecallState callState;
	bool sendDTMF(char,unsigned);
	bool sendDTMF(char);
	bool sendSMS(char [],char []);
	bool setSmsTextMode();
  private:
    eCIPstatus CIPstatus;
	unsigned maxMessageLength;
    volatile unsigned modemMessageLength;
    byte *modemmessage;
    enum eParseState {GETMM,GETLENGTH,GETDATA,GETTELEVENT};
    eParseState ParseState;
	unsigned clientMsgLength;
	bool nextLineSMS;
};
#endif


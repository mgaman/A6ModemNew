#ifndef A6SERVICE_H
#define  A6SERVICE_H
#include "A6ModemNew.h"

class A6GPRS: public virtual A6GPRSDevice
{
  public:
    enum eCIPstatus {IP_INITIAL,IP_START,IP_CONFIG,IP_IND,IP_GPRSACT,IP_STATUS,TCPUDP_CONNECTING,IP_CLOSE,CONNECT_OK,IP_STATUS_UNKNOWN };
    enum ePSstate { DETACHED,ATTACHED,PS_UNKNOWN };
    A6GPRS(Stream &comm,unsigned); // constructor uartstream & buffer size
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
    bool sendToServer(char[]);
    bool sendToServer(char[],int);  // for ascii strings
    bool sendToServer(byte[],int);  // for byte arrays
	bool connectedToServer;
  private:
    eCIPstatus CIPstatus;
  //  int cid;  // only 1 per instance of class
};

extern A6GPRS gsm;  // instance
#endif


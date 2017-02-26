#ifndef A6DEVICE_H
#define A6DEVICE_H

class A6GPRSDevice
{
  public:
	enum eExceptions {BUFFER_OVERFLOW,URL_TOO_LONG,CIRC_BUFFER_OVERFLOW};
    A6GPRSDevice();
    ~A6GPRSDevice();
    virtual bool begin();
    void modemWrite(byte);
    bool waitresp(char const *response_string,int32_t);
    bool GetLineWithPrefix(char const *px,char *outbuf, int l,int32_t timeout);
    // weak methods are helpers - implement only if you really want them
    void HWReset() __attribute__((weak));   // may be implemented by the caller 
    void debugWrite(int) __attribute__((weak));
    void debugWrite(uint16_t) __attribute__((weak));
    void debugWrite(char) __attribute__((weak));
    void debugWrite(char[]) __attribute__((weak));
    void debugWrite(const __FlashStringHelper*) __attribute__((weak));
    void modemPrint(const __FlashStringHelper*);
    void modemPrint(char []);
    void modemPrint(int);
    void RXFlush();  // clear all data in input buffer
    bool enableDebug;
    char pop();       // get data from buffer, -1 if none available
    void push(char);   // add incoming data to buffer
	  int32_t rxcount,txcount;
    char *comm_buf;  // communication buffer +1 for 0x00 termination
    unsigned commbuffsize;
    int inlevel, outlevel;        // data in comm_buf
    Stream *_comms;
	void onException(eExceptions,int) __attribute__((weak));
  private:
};
#endif


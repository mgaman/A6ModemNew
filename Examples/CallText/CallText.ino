#include "A6Services.h"
#include "A6Call.h"

A6GPRS gsm(Serial1,1000,200);
A6CALL call(gsm);
static char tempbuf[200];
#define DIALOUT "0545919886"
void setup() {
  // put your setup code here, to run once:
  Serial1.begin(115200);
  Serial.begin(115200);
  delay(1000);
  gsm.enableDebug = true;
  if (gsm.begin())
  {
    Serial.println("gsm up");
    if (gsm.callerID(true))
      Serial.println("Caller ID enabled");
  }
  else
    Serial.println("gsm down");
}

void loop() {
  byte *mm;
  unsigned len;
  if (Serial.available())
  {
    switch (Serial.read())
    {
      case 'd':
        sprintf(tempbuf,"Dialling %s %s",DIALOUT,
        gsm.dial(DIALOUT) ? "succeeded" : "failed");
        Serial.println(tempbuf);
        break;
      case 'a': 
        sprintf(tempbuf,"Answer %s",gsm.answer() ? "succeeded" : "failed");
        Serial.println(tempbuf);
        break;
      case 'h': 
        sprintf(tempbuf,"Hangup %s",gsm.hangup() ? "succeeded" : "failed");
        Serial.println(tempbuf);
        break;
      case 'f': 
        sprintf(tempbuf,"dtmf %s",gsm.sendDTMF('#',5) ? "succeeded" : "failed");
        Serial.println(tempbuf);
        break;      
      case '?':
        Serial.println("a Answer");
        Serial.println("h Hangup");
        Serial.println("d dial");        
        Serial.println("s send SMS");        
        Serial.println("f send DTMF string");        
        Serial.println("? menu");        
         break;
      case 's':
        Serial.print("SMS send ");
        Serial.println(gsm.sendSMS(DIALOUT,"hello world") ? "success" : "fail");
        break;
    }
  }
  mm = gsm.Parse(&len);
  if (len != 0)
    call.Parse(mm,len);
}
/*
 *  You MUST provide the appropriate serialEvent function for the hardware serial
 *  port defined in the A6GPRS constructor.
 *  It simply copied all incoming traffic to the libraries circular buffer for later processing
 */
void serialEvent1() {
  while (Serial1.available())
    gsm.push((char)Serial1.read());
}


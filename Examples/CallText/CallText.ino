#include "A6Services.h"
#include "A6Call.h"

A6GPRS gsm(Serial1,1000,100);
A6CALL call(gsm);
static char tempbuf[200];
//#define DIALOUT "048721601"
#define DIALOUT "0545919886"
void setup() {
  // put your setup code here, to run once:
  Serial1.begin(115200);
  Serial.begin(115200);
  delay(1000);
  gsm.enableDebug = false;
  if (gsm.begin())
  {
    Serial.println("gsm up");
    if (call.clip(true))
      Serial.println("CLIP enabled");
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
        call.dial(DIALOUT) ? "succeeded" : "failed");
        Serial.println(tempbuf);
        break;
      case 'a': 
        sprintf(tempbuf,"Answer %s",call.answer() ? "succeeded" : "failed");
        Serial.println(tempbuf);
        break;
      case 'h': 
        sprintf(tempbuf,"Hangup %s",call.hangup() ? "succeeded" : "failed");
        Serial.println(tempbuf);
        break;
      case 'f': 
        sprintf(tempbuf,"dtmf %s",call.sendDTMF('#',5) ? "succeeded" : "failed");
        Serial.println(tempbuf);
        break;      
      case '?':
        Serial.println("a Answer");
        Serial.println("h Hangup");
        Serial.println("d dial");        
        Serial.println("f send DTMF string");        
         break;
      case 's':
        Serial.print("SMS send ");
        Serial.println(call.sendSMS(DIALOUT,"hello world") ? "success" : "fail");
        break;
    }
  }
  mm = gsm.Parse(&len);
  if (len != 0)
    call.Parse(mm,len);
}


#include <Arduino.h>
#include "A6Services.h"
#include <A6Call.h>
extern int fileToPlay;

void A6CALL::OnPhoneEvent(enum ephoneEvent e,int parm)
{
  switch(e)
  {
    case SOUNDER:
      if (parm==0)
        Serial.println("Callee answered");
      break;
    case CALL:
      if (parm==0)
        Serial.println("Callee hungup");
      break;
    case SMS_ARRIVED:
      Serial.print("Incoming SMS: ");Serial.println(parm);Serial.println(smsSender);Serial.println(smsbuffer);
      break;
  }
}

void A6CALL::OnDialin(char n[])
{
  Serial.print(n);
  Serial.println(" is calling.");
}


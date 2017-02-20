#include "A6Services.h"
#include "A6Call.h"
#include <SoftwareSerial.h>
#include "mp3player.h"

A6GPRS gsm(Serial1,1000,100);
A6CALL call(gsm);
static char tempbuf[200];
//#define DIALOUT "048721601"
#define DIALOUT "0545919886"
//SoftwareSerial mp3uart(10,11); 
//MP3Player mp3(mp3uart);

int fileToPlay = -1;
enum eMp3Status {TIDLE,PLAYING,ENDED} mp3Status = TIDLE;
void setup() {
  // put your setup code here, to run once:
  Serial1.begin(115200);
  Serial.begin(115200);
//  mp3uart.begin(9600);
 // mp3.begin(true);
//  mp3.SetVolume(30);
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
#if 1
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
      case 'x':
        gsm.enableDebug = false;
        break;
      case 'X':
        gsm.enableDebug = true;
        break;
      case 's':
        Serial.print("SMS send ");
        Serial.println(call.sendSMS(DIALOUT,"hello world") ? "success" : "fail");
        break;
    }
  }
#endif
  mm = gsm.Parse(&len);
  if (len != 0)
    call.Parse(mm,len);
}

void MP3Player::FeedbackCallback(byte fb[])
{
//  Serial.print(fb[0],HEX);
 // Serial.print(' ');
 // Serial.println(fb[3]);
  if (fb[0] == TTRACK_FINISHED)
    mp3Status = ENDED;
}

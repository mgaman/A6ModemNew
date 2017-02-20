#include "A6Services.h"
#include "A6HTTP.h"

#define APN "uinternet"  // write your APN here

A6GPRS gsm(Serial1,2000,1000);
A6HTTP http(gsm);

char ipaddress[20];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  gsm.enableDebug = false;
  if (gsm.begin())
  {
    Serial.println("gsm up");
    if (gsm.startIP(APN))
    {
      Serial.println("IP up");
      if (gsm.getLocalIP(ipaddress))
        Serial.println(ipaddress);
    }
  }
}

char pbuff[] = "david-henry.dyndns.tv";
//char pbuff[] = "david-henry.dyndns.tv/WC/last3.php";
//char pbuff[] = "google.co.il";
uint32_t nextget = 0;
void loop() {
  byte *mm;
  unsigned l;
  // put your main code here, to run repeatedly:
  if (nextget < millis())
  {
    nextget = millis() + 60000;
    if (gsm.connectTCPserver("david-henry.dyndns.tv",80))
//    if (gsm.connectTCPserver("google.co.il",80))
    {
      Serial.println("TCP up");
      http.get(pbuff);    
    }    
  }
  mm = gsm.Parse(&l);
  if (l != 0)
    http.Parse(mm,l);
}


void A6HTTP::OnDataReceived(char text[],unsigned length)
{
  text[length]=0;
  Serial.println("onData");
  Serial.print(text);
  gsm.stopIP();
}



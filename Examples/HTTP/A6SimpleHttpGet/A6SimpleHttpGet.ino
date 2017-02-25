#include "A6Services.h"
#include "A6HTTP.h"

#define APN "uinternet"  // write your APN here

A6GPRS gsm(Serial1,3000,1700);
A6HTTP http(gsm);
char ipaddress[40];
bool netup = false;

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
      netup = true;
#if 0
      if (gsm.getCIMI(ipaddress))
      {
        Serial.print("CIMI: ");
        Serial.println(ipaddress);
      }
      else
        Serial.println("didn't get CIMI");
#endif
       if (gsm.getLocalIP(ipaddress))
      {
        Serial.print("IP: ");
        Serial.println(ipaddress);
      }
      else
        Serial.println("didn't get IP address");
    }
    else
      Serial.println("IP down");
  }
  else
    Serial.println("gsm down");
}

char server[] = "google.com";
char path[] = "/index.html";
uint32_t nextget = 0;
void loop() {
  byte *mm;
  unsigned l;
  if (netup && nextget < millis())
  {
    nextget = millis() + 60000;
    if (gsm.connectTCPserver("google.com",80))
    {
      Serial.println("Server up");
      http.get(server,path);
    } 
    else
       Serial.println("Server down");      
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



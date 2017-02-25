#include <Arduino.h>
#include "A6Services.h"
#include "A6http.h"
A6HTTP::A6HTTP(A6GPRS& a6gprs)
{
  _a6gprs = &a6gprs;
}

bool A6HTTP::get(char server[], char path[])
{
	// avoid sprintf if possible, send as an array of strings
	char *strings[5];	// send in 5 parts
	strings[0] = "GET ";
	strings[1] = path;
	strings[2] = " HTTP/1.1\r\nUser-Agent Arduino\r\nAccept: text/plain\r\nHost: ";
	strings[3] = server;
	strings[4] = "\r\n\r\n";
	return gsm.sendToServer(strings,5);		
}

bool A6HTTP::get(char server[])
{
	return get(server,"/");
}

bool  A6HTTP::post(char server[],char path[],char parms[])
{
	char *strings[8];	// send in 5 parts
	char buff[8];
	sprintf(buff,"%u",strlen(parms));
	strings[0] = "POST ";
	strings[1] = path;
	strings[2] = " HTTP/1.1\r\nUser-Agent Arduino\r\nAccept: text/plain\r\nHost: ";
	strings[3] = server;
	strings[4] = "\r\nContent-Length: ";
	strings[5] = buff;
	strings[6] = "\r\n\r\n";
	strings[7] = parms;
	
	return gsm.sendToServer(strings,8);		
	
}
void A6HTTP::Parse(byte *rawData,unsigned length)
{
	if (length == 1)
		return;	// single trailing LF
	rawData[length] = 0;
	if (strncmp((char *)rawData,"+CGREG",6) == 0)
	{		
	}
	else
		OnDataReceived(rawData,length);
}

unsigned A6HTTP::urlencode(char src[], char tgt[])
{
  unsigned offset = 0;
//  const char rfc3986[] = "/?#[]@";
  const char rfc3986[] = " !*'();:@&=+$,/?#[]";
//  const char rfc3986[] = " !*'();:@+$,/?#[]";  // without & = 
  while (*src)
  {
    if (strchr(rfc3986,*src))
    {
	   sprintf(&tgt[offset],"%%%02X",*src);
	   offset += 3;
    }
    else
	{
		tgt[offset++] = *src;
	}
    src++;
  }
  tgt[offset] = 0;
  return offset;
}

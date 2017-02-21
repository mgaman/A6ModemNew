#include <Arduino.h>
#include "A6Services.h"
#include "A6http.h"
//static char temp[200];
A6HTTP::A6HTTP(A6GPRS& a6gprs,unsigned maxurllength)
{
  _a6gprs = &a6gprs;
  maxUrlLength = maxurllength;
  urlbuffer = new char[maxUrlLength];
}
/*
	Split url into host and payload parts
*/
bool A6HTTP::get(char url[])
{
	// copy URL server name up to first slash
	char *cp = url;
	unsigned limit = strlen(url);
	unsigned urloffset = 0;
	while (*cp != '/' && urloffset != limit)
		urlbuffer[urloffset++] = *cp++;
	urlbuffer[urloffset++] = 0;  // end servername
	unsigned serverlength = strlen(urlbuffer);
	unsigned parmlength = strlen(&url[serverlength]);
	if (parmlength == 0)
		strcpy(&urlbuffer[serverlength+1],"/");
	else
		strcpy(&urlbuffer[serverlength+1],&url[serverlength]);
	if ((serverlength+parmlength+2) > maxUrlLength)  // too long to encode
	{
		_a6gprs->onException(A6GPRS::URL_TOO_LONG,serverlength+parmlength);
		return false;
	}
	else
	{
		// avoid sprintf if possible, send as an array of strings
		char *strings[5];	// send in 5 parts
		strings[0] = "GET ";
		strings[1] = &urlbuffer[serverlength+1];
		strings[2] = " HTTP/1.1\r\nHost: ";
		strings[3] = urlbuffer;
		strings[4] = "\r\n\r\n";
		return gsm.sendToServer(strings,5);		
	}
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

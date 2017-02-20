#include <Arduino.h>
#include "A6Services.h"
#include "A6http.h"
static char temp[200];
A6HTTP::A6HTTP(A6GPRS& a6gprs)
{
  _a6gprs = &a6gprs;
}
/*
	Split url into host and payload parts
*/
bool A6HTTP::get(char url[])
{
	char URL[100];
	strcpy(URL,url);
	char *slash = strchr(URL,'/');  // find separator (if any)
	if (slash == 0)
		slash = "";
	else
		*slash++ = 0; // separate and move to next character
	sprintf(temp,"GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n",slash,URL);
//	_a6gprs->debugWrite(temp);
	Serial.println("GET");
	Serial.println(temp);
	return gsm.sendToServer(temp);
}

void A6HTTP::Parse(byte *rawData,unsigned length)
{
	if (length == 1)
		return;	// single trailing LF
	sprintf(temp,"http parse %u\r\n",length);
	rawData[length] = 0;
	if (strncmp((char *)rawData,"+CGREG",6) == 0)
	{		
	}
	else
		OnDataReceived(rawData,length);
}

int A6HTTP::urlencode(char src[], char tgt[])
{
  int offset = 0;
//  const char rfc3986[] = " !*'();:@&=+$,/?#[]";
  const char rfc3986[] = " !*'();:@+$,/?#[]";  // without & = 
  while (*src)
  {
    if (strchr(rfc3986,*src))
    {
      sprintf(&tgt[offset],"%%%02X",*src);
      offset += 3;
    }
    else
      tgt[offset++] = *src;
    src++;
  }
  tgt[offset] = 0;
  return offset;
}

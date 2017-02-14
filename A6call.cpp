#include "Arduino.h"
#include "A6Services.h"
#include "A6Call.h"

static char printbuff[120];
A6CALL::A6CALL(A6GPRS& a6gprs)
{
	_a6gprs = &a6gprs;
	callState = IDLE;
}
A6CALL::~A6CALL(){}
bool A6CALL::dial(char number[])
{
	_a6gprs->modemPrint("ATD");
	_a6gprs->modemPrint(number);
	_a6gprs->modemPrint("\r");
	return _a6gprs->waitresp("OK",1000);
}
bool A6CALL::answer()
{
	_a6gprs->modemPrint("ATA\r");
	return _a6gprs->waitresp("OK",1000);
}
bool A6CALL::hangup()
{
	_a6gprs->modemPrint("ATH\r");
	return _a6gprs->waitresp("OK",1000);
}

void A6CALL::Parse(byte buffer[],unsigned length)
{
	int parm;
	char *start;
#if 0
	Serial.print("Parsing ");
	buffer[length] = 0; // add terminator
	Serial.println((char *)buffer);
#endif
	if (strncmp(buffer,"+CIEV:",6) == 0)
	{
		start = strchr(buffer,'"');	// point to begining of string
		if (start)
		{
			start++;
			if (strncmp(start,"SOUNDER\",",9) == 0)
			{
				start += 9;
				parm = atoi(start);
				OnPhoneEvent(SOUNDER,parm);
			}
			else if (strncmp(start,"CALL\",",6) == 0)
			{
				start += 6;
				parm = atoi(start);
				OnPhoneEvent(CALL,parm);
			}
		}
	}
	//+CLIP: "0545919886",129,,,,1
	else if (strncmp(buffer,"+CLIP:",6) == 0)
	{
		start = strchr(buffer,'"');	// point to begining of string
		if (start)
		{
			start++;
			char *end = strchr(start,'"');	// point to end of number
			if (end)
				*end = 0;
		//	Serial.println(start);
			OnDialin(start);
		}
	}
}

bool A6CALL::clip(bool enable)
{
	sprintf(printbuff,"AT+CLIP=%d\r",enable);	
	_a6gprs->modemPrint(printbuff);
	return _a6gprs->waitresp("OK",1000);	
}

bool A6CALL::sendDTMF(char c,unsigned t)
{
	sprintf(printbuff,"AT+VTS=%c,%u\r",c,t);
	_a6gprs->debugWrite(printbuff);
	_a6gprs->modemPrint(printbuff);
	return _a6gprs->waitresp("OK",1000);
}

bool A6CALL::sendDTMF(char c)
{
	return sendDTMF(c,1);
}
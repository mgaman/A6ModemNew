#include "Arduino.h"
#include "A6Services.h"
#include "A6Call.h"

static char printbuff[120];
A6CALL::A6CALL(A6GPRS& a6gprs)
{
	_a6gprs = &a6gprs;
	callState = IDLE;
	nextLineSMS = false;
}
A6CALL::~A6CALL(){}
bool A6CALL::dial(char number[])
{
	bool rc = false;
	_a6gprs->modemPrint("ATD");
	_a6gprs->modemPrint(number);
	_a6gprs->modemPrint("\r");
	if (_a6gprs->waitresp("OK",1000))
	{
		callState = DIALLING_OUT;
		rc = true;
	}
	return rc;
}
bool A6CALL::answer()
{
	_a6gprs->modemPrint("ATA\r");
	callState = SPEAKING;
	return _a6gprs->waitresp("OK",1000);
}
bool A6CALL::hangup()
{
	_a6gprs->modemPrint("ATH\r");
	callState = IDLE;
	return _a6gprs->waitresp("OK",1000);
}

void A6CALL::Parse(byte buffer[],unsigned length)
{
	int parm;
	char *start;
//	Serial.println((char *)buffer);
	if (length == 1)
		return;	// single trailing LF
#if 0
	Serial.print("Parsing ");
	buffer[length] = 0; // add terminator
	Serial.println((char *)buffer);
#endif
	if (strncmp(buffer,"RING",4) == 0)
		callState = CALLER_RINGING;
	else if (strncmp(buffer,"+CIEV:",6) == 0)
	{
		start = strchr(buffer,'"');	// point to begining of string
		if (start)
		{
			start++;
			if (strncmp(start,"SOUNDER\",",9) == 0)
			{
				start += 9;
				parm = atoi(start);
				if (parm == 0)
					callState = SPEAKING;
				OnPhoneEvent(SOUNDER,parm);
			}
			else if (strncmp(start,"CALL\",",6) == 0)
			{
				start += 6;
				parm = atoi(start);
				if (parm == 0)
					callState = IDLE;
				OnPhoneEvent(CALL,parm);
			}
		}
	}
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
			callState = CALLERID;
			OnDialin(start);
		}
	}
	else if (strncmp(buffer,"+CMT:",5) == 0)
	{
		// extract sender
		start = strchr(buffer,'"');
		if (start)
		{
			start++;
			char *end = strchr(start,'"');
			*end = 0;
			strcpy(smsSender,start);
		}
		nextLineSMS = true;
	}
	else if (nextLineSMS)
	{
		strcpy(smsbuffer,buffer);
		nextLineSMS = false;
		OnPhoneEvent(SMS_ARRIVED,strlen(smsbuffer));
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

bool A6CALL::sendSMS(char addr[],char text[])
{
	sprintf(printbuff,"AT+CMGS=\"%s\"\r",addr);	
	_a6gprs->modemPrint(printbuff);
	_a6gprs->waitresp(">",1000);	
	_a6gprs->modemPrint(text);
	_a6gprs->modemWrite(0x1a);
	return _a6gprs->waitresp("+CMGS:",5000);
}
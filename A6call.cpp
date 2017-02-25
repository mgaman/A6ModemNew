#include "Arduino.h"
#include "A6Services.h"
#include "A6Call.h"

A6CALL::A6CALL(A6GPRS& a6gprs)
{
	_a6gprs = &a6gprs;
}
A6CALL::~A6CALL(){}
void A6CALL::Parse(byte buffer[],unsigned length)
{
	int parm;
	char *start;
	if (length == 1)
		return;	// single trailing LF
	if (strncmp(buffer,"RING",4) == 0)
		_a6gprs->callState = _a6gprs->CALLER_RINGING;
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
					_a6gprs->callState = _a6gprs->SPEAKING;
				OnPhoneEvent(SOUNDER,parm);
			}
			else if (strncmp(start,"CALL\",",6) == 0)
			{
				start += 6;
				parm = atoi(start);
				if (parm == 0)
					_a6gprs->callState = _a6gprs->IDLE;
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
			_a6gprs->callState = _a6gprs->CALLERID;
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

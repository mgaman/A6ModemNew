/*
 * Mods:  make max mesage length parameter to constructor
*/
#ifndef A6_CALL_H_
#define A6_CALL_H_
#endif
class A6CALL
{
	public:
		enum ephoneEvent {SOUNDER,CALL,SMS_ARRIVED};
		enum ecallState {IDLE,CALLER_RINGING,SPEAKING,DIALLING_OUT,CALLERID};
		A6CALL(A6GPRS& a6gprs);
		~A6CALL();
		bool dial(char[]);
		bool answer();
		bool hangup();
		bool clip(bool);
		// callbacks
		void OnPhoneEvent(enum ephoneEvent,int) __attribute__((weak));
		void OnDialin(char[]);
		void Parse(byte[],unsigned);
		bool sendDTMF(char,unsigned);
		bool sendDTMF(char);
		enum ecallState callState;
		char smsbuffer[160];
		char smsSender[20];
		bool sendSMS(char [],char []);
	private:
		A6GPRS *_a6gprs;
		bool nextLineSMS;
};

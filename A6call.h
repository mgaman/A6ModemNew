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
		A6CALL(A6GPRS& a6gprs);
		~A6CALL();
		// callbacks
		void OnPhoneEvent(enum ephoneEvent,int) __attribute__((weak));
		void OnDialin(char[]);
		void Parse(byte[],unsigned);
		char smsbuffer[160];
		char smsSender[20];
	private:
		A6GPRS *_a6gprs;
		bool nextLineSMS;
};

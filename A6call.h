/*
 * Mods:  make max mesage length parameter to constructor
*/
#ifndef A6_CALL_H_
#define A6_CALL_H_
#endif
class A6CALL
{
	public:
		enum ephoneEvent {SOUNDER,CALL};
		enum ecallState {IDLE,RINGING,SPEAKING};
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
	private:
		A6GPRS *_a6gprs;
};

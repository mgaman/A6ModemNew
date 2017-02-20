#ifndef A6_HTTP_H_
#define A6_HTTP_H_
#endif

class A6HTTP
{
  public:
    A6HTTP(A6GPRS& a6gprs);
	bool get(char[]);
	void OnDataReceived(char[],unsigned);
    void Parse(byte[],unsigned);
  private:
    A6GPRS *_a6gprs;
	int urlencode(char [], char []);
};

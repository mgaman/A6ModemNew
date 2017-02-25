#ifndef A6_HTTP_H_
#define A6_HTTP_H_
#endif

class A6HTTP
{
  public:
    A6HTTP(A6GPRS&);
	bool get(char[]);
	bool get(char[],char[]);
	bool post(char[],char[],char[]);
	void OnDataReceived(char[],unsigned);
    void Parse(byte[],unsigned);
	unsigned urlencode(char [], char []);
  private:
    A6GPRS *_a6gprs;
};

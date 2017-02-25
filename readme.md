# A6ModemNew
I have named this library A6ModemNew to differentiate it from my previous offering <b>A6General</b> which is now obsolete.<p>
The difficulty of any modem handling library is how to handle unsolicited messages from the modem and how to correctly define buffer
sizes for data and avoid buffer overflow situations.<p>
This library does that.
The A6 modem theoretically can work at any baud rate but I have never managed to get it to change from the default of 115200bps.
Accordingly I work only via a hardware serial port.
SoftwareSerial <b>does not</b> work with this library.<p>
There are 4 main classes on offer:
<ul>
<li>
A6GPRS
This is the class that handles all UART traffic to and from the modem. This is the only class where you will see AT commands and their 
responses.
</li>
<li>
A6CALL
This class offers phone services and SMS services. Calls may be sent or received. SMS messages (text mode only) may be sent or received.
</li>
<li>
A6HTTP
This class offers HTTP GET and HTTP POST services
</li>
<li>
A6MQTT
This class implements the MQTT protocol (including QOS 1, 2 and 3)
</li>
</ul>  
The Users application create one instance of the A6GPRS class and one of the other classes.
<h2>Callbacks</h2>
Extensive use is made of callback mechanisms whick make handling asynchronous events much simpler. The User goes not have to implement
all callbacks but may miss important imformation if not. For example the A6Call class has an OnPhoneEvent callback. 
If you have no need to answer incoming calls or
read incoming SMSs the callback need not be implemented.
<h2>SerialEvent</h2>
The User must implement the appropriate serialEvent function as this is the only way that incoming data from the modem is
passed to the library for processing. Example code shows how this is done.
<h2>Constructors</h2>
<h3>A6GPRS</h3>
A6GPRS(Stream,CircularBufferSize,LargestItemSize)<br>
Stream may be any hardware serial port, Serial, Serial1, Serial2, Serial3.<br>
Incoming data is saved in a circular buffer of size <b>CircularBufferSize</b> bytes.<br>
Incoming data is parsed into individual items, usually strings ending in <cr><lf> or blocks of data coming from a TCP server.
<b>LargestItemSize</b> as its name implies is the largest item you expect to receive.
<h3>A6CALL</h3>
A6CALL(A6GPRS instance)
<h3>A6HTTP</h3>
A6HTTP(A6GPRS instance)
The LargestItemSize parameter of the A6GPRS constructor should reflect the largest response you will get from and HTTP GET or POST.
<h3>A6MQTT</h3>
A6MQTT(A6GPRS instance,KeepAliveTime,maxMessageLength)
KeepAliveTime is the value (in seconds) passed to the MQTT broker when connecting.<br>
maxMessageLength is the maximum length of an outgoing message. The buffersize of incoming messages is defined by the LargestItemSize parameter
of the A6GPRS constructor.


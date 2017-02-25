/*
 *   Following functions are implemented on behalf of the A6GPRSDevice class
 *   None of them HAVE to be present but I would recommend that at least HWRESET is implemented
 *   else the modem may never get  going
 */
#include <Arduino.h>
#include "A6Services.h"
//#include "A6MQTT.h"

#define TRANSISTOR_CONTROL 7  // connect to base of transistor
#define A6_RESET_TIME 50 // ms

extern A6GPRS gsm;

void A6GPRSDevice::HWReset()
{
  /*
   * The A6 modem takes about 70mA throuh the reset pin which is too much for an Arduino GPIO pin
   * Instead connect an Arduino GPIO pin to the base of a transistor and toggle that.
   * Connect the emitter of the transistor to ground and the collector to the A6 reset pin
   * http://electronics.stackexchange.com/questions/82222/how-to-use-a-transistor-to-pull-a-pin-low
   */
  pinMode(TRANSISTOR_CONTROL,OUTPUT);
  digitalWrite(TRANSISTOR_CONTROL,LOW);
  digitalWrite(TRANSISTOR_CONTROL,HIGH);
  delay(A6_RESET_TIME);
  digitalWrite(TRANSISTOR_CONTROL,LOW); 
  Serial.println("reset"); 
}

void A6GPRSDevice::debugWrite(uint16_t c)
{
  if (enableDebug)
    Serial.print(c);
}
void A6GPRSDevice::debugWrite(int c)
{
  if (enableDebug)
    Serial.print(c);
}
void A6GPRSDevice::debugWrite(char c)
{
  if (enableDebug)
    Serial.write(c);
}
void A6GPRSDevice::debugWrite(char *s)
{
  if (enableDebug)
    Serial.print(s);
}
void A6GPRSDevice::debugWrite(const __FlashStringHelper*s)
{
  if (enableDebug)
    Serial.print(s);  
}

void A6GPRSDevice::onException(eExceptions ex,int other)
{
  switch(ex)
  {
    case BUFFER_OVERFLOW:
      Serial.print("buffer overflow ");Serial.println(other);
      break;
    case URL_TOO_LONG:
      Serial.print("URL overflow ");Serial.println(other);
      break; 
    case CIRC_BUFFER_OVERFLOW:   
      Serial.print("Circ. overflow ");Serial.println(other);
      break; 
    default:
      Serial.print("Unknown exception ");Serial.println(other);
      break;
  }
}


void serialEvent1() {
  while (Serial1.available())
    gsm.push((char)Serial1.read());
}


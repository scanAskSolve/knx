#include "arduino_platform.h"
#include "knx/bits.h"

#include <Arduino.h>
/*#ifndef KNX_NO_SPI
#include <SPI.h>
#endif*/





#ifndef KNX_PRINT_DISABLE
	Stream* ArduinoPlatform::SerialDebug = &KNX_DEBUG_SERIAL;
#endif

ArduinoPlatform::ArduinoPlatform() : _knxSerial(nullptr)
{
}

ArduinoPlatform::ArduinoPlatform(HardwareSerial* knxSerial) : _knxSerial(knxSerial)
{
}

void ArduinoPlatform::fatalError()
{
    while (true)
    {
#ifdef KNX_LED
        static const long LED_BLINK_PERIOD = 200;

        if ((millis() % LED_BLINK_PERIOD) > (LED_BLINK_PERIOD / 2))
            digitalWrite(KNX_LED, HIGH);
        else
            digitalWrite(KNX_LED, LOW);
#endif
    }
}

void ArduinoPlatform::knxUart( HardwareSerial* serial )
{
    if (_knxSerial)
        closeUart();
    _knxSerial = serial;
    setupUart();
}

HardwareSerial* ArduinoPlatform::knxUart()
{
    return _knxSerial;
}

void ArduinoPlatform::setupUart()
{
    _knxSerial->begin(19200, SERIAL_8E1);
    while (!_knxSerial) 
        ;
}


void ArduinoPlatform::closeUart()
{
    _knxSerial->end();
}


int ArduinoPlatform::uartAvailable()
{
    return _knxSerial->available();
}


size_t ArduinoPlatform::writeUart_data(const uint8_t data)
{
    //printHex("<p", &data, 1);
    return _knxSerial->write(data);
}


size_t ArduinoPlatform::writeUart_buffer(const uint8_t *buffer, size_t size)
{
    //printHex("<p", buffer, size);
    return _knxSerial->write(buffer, size);
}


int ArduinoPlatform::readUart()
{
    int val = _knxSerial->read();
    //if(val > 0)
    //    printHex("p>", (uint8_t*)&val, 1);
    return val;
}


size_t ArduinoPlatform::readBytesUart(uint8_t *buffer, size_t length)
{
    size_t toRead = length;
    uint8_t* pos = buffer;
    while (toRead > 0)
    {
        size_t val = _knxSerial->readBytes(pos, toRead);
        pos += val;
        toRead -= val;
    }
    //printHex("p>", buffer, length);
    return length;
}

/*#ifndef KNX_NO_SPI

void ArduinoPlatform::setupSpi()
{
    SPI.begin();
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
}

void ArduinoPlatform::closeSpi()
{
    SPI.endTransaction();
    SPI.end();
}

int ArduinoPlatform::readWriteSpi(uint8_t *data, size_t len)
{
    SPI.transfer(data, len);
    return 0;
}
#endif*/

//#ifndef KNX_NO_PRINT
void printUint64(uint64_t value, int base = DEC)
  {
  #ifndef KNX_PRINT_DISABLE
    char buf[8 * sizeof(uint64_t) + 1];
    char* str = &buf[sizeof(buf) - 1];
    *str = '\0';

    uint64_t n = value;
    do {
      char c = n % base;
      n /= base;

      *--str = c < 10 ? c + '0' : c + 'A' - 10;
    } while (n > 0);

     print(str);
	#endif 
}

void print(const char* s)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(s);
	#endif 
}
void print(char c)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(c);
	#endif 
}

void print(unsigned char num)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(num);
	#endif 
}

void print(unsigned char num, int base)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(num, base);
	#endif
}

void print(int num)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(num);
	#endif
}

void print(int num, int base)
{
	#ifndef  KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(num, base);
	#endif 
}

void print(unsigned int num)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(num);
	#endif 
}

void print(unsigned int num, int base)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(num, base);
	#endif 
}

void print(long num)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(num);
	#endif 
}

void print(long num, int base)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(num, base);
	#endif
}

void print(unsigned long num)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(num);
	#endif 
}

void print(unsigned long num, int base)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(num, base);
	#endif 
}

void print(unsigned long long num)
{
	#ifndef KNX_PRINT_DISABLE
    printUint64(num);
	#endif 
}

void print(unsigned long long num, int base)
{
	#ifndef KNX_PRINT_DISABLE
    printUint64(num, base);
	#endif 
}

void print(double num)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->print(num);
	#endif
}

void println(const char* s)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(s);
	#endif
}

void println(char c)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(c);
	#endif 
}

void println(unsigned char num)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(num);
	#endif 
}

void println(unsigned char num, int base)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(num, base);
	#endif 
}

void println(int num)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(num);
	#endif
}

void println(int num, int base)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(num, base);
	#endif
}

void println(unsigned int num)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(num);
	#endif
}

void println(unsigned int num, int base)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(num, base);
	#endif 
}

void println(long num)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(num);
	#endif 
}

void println(long num, int base)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(num, base);
	#endif 
}

void println(unsigned long num)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(num);
	#endif 
}

void println(unsigned long num, int base)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(num, base);
	#endif 
}

void println(unsigned long long num)
{
	#ifndef KNX_PRINT_DISABLE
    printUint64(num);
    println("");
	#endif 
}

void println(unsigned long long num, int base)
{
	#ifndef KNX_PRINT_DISABLE
    printUint64(num, base);
    println("");
	#endif 
}

void println(double num)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println(num);
	#endif 
}

void println(void)
{
	#ifndef KNX_PRINT_DISABLE
    ArduinoPlatform::SerialDebug->println();
	#endif 
}

//#endif 

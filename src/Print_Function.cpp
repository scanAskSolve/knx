#include "Print_Function.h"


#ifndef KNX_NO_PRINT

Stream* SerialDebug;

void Print_init(HardwareSerial* port){
    SerialDebug = port;
}

void printHex(const char* suffix, const uint8_t *data, size_t length, bool newline)
{
    print(suffix);
    for (size_t i = 0; i < length; i++) {
        if (data[i] < 0x10) { print("0"); }
        print(data[i], HEX);
        print(" ");
    }
    if (newline)
    {
        println();
    }
} 

void printUint64(uint64_t value, int base = DEC)
{
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
}

void print(const char* s)
{
    SerialDebug->print(s);
}
void print(char c)
{
    SerialDebug->print(c);
}

void print(unsigned char num)
{
    SerialDebug->print(num);
}

void print(unsigned char num, int base)
{
    SerialDebug->print(num, base);
}

void print(int num)
{
    SerialDebug->print(num);
}

void print(int num, int base)
{
    SerialDebug->print(num, base);
}

void print(unsigned int num)
{
    SerialDebug->print(num);
}

void print(unsigned int num, int base)
{
    SerialDebug->print(num, base);
}

void print(long num)
{
    SerialDebug->print(num);
}

void print(long num, int base)
{
    SerialDebug->print(num, base);
}

void print(unsigned long num)
{
    SerialDebug->print(num);
}

void print(unsigned long num, int base)
{
    SerialDebug->print(num, base);
}

void print(unsigned long long num)
{
    printUint64(num);
}

void print(unsigned long long num, int base)
{
    printUint64(num, base);
}

void print(double num)
{
    SerialDebug->print(num);
}

void println(const char* s)
{
    SerialDebug->println(s);
}

void println(char c)
{
    SerialDebug->println(c);
}

void println(unsigned char num)
{
    SerialDebug->println(num);
}

void println(unsigned char num, int base)
{
    SerialDebug->println(num, base);
}

void println(int num)
{
    SerialDebug->println(num);
}

void println(int num, int base)
{
    SerialDebug->println(num, base);
}

void println(unsigned int num)
{
    SerialDebug->println(num);
}

void println(unsigned int num, int base)
{
    SerialDebug->println(num, base);
}

void println(long num)
{
    SerialDebug->println(num);
}

void println(long num, int base)
{
    SerialDebug->println(num, base);
}

void println(unsigned long num)
{
    SerialDebug->println(num);
}

void println(unsigned long num, int base)
{
    SerialDebug->println(num, base);
}

void println(unsigned long long num)
{
    printUint64(num);
    println("");
}

void println(unsigned long long num, int base)
{
    printUint64(num, base);
    println("");
}

void println(double num)
{
    SerialDebug->println(num);
}

void println(void)
{
    SerialDebug->println();
}
#endif // KNX_NO_PRINT

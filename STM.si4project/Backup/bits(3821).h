#pragma  once

//#include <cstddef>
//#include <cstdint>

#include <stddef.h>
#include <stdint.h>


#if defined(ARDUINO_ARCH_STM32)
#define getbyte(x,n) (*(((uint8_t*)&(x))+n))
#define htons(x)  ( (getbyte(x,0)<<8) | getbyte(x,1) ) 
#define htonl(x) ( (getbyte(x,0)<<24) | (getbyte(x,1)<<16) | (getbyte(x,2)<<8) | getbyte(x,3) )
#define ntohs(x) htons(x)
#define ntohl(x) htonl(x)
#endif

#ifndef MIN
#define MIN(a, b) ((a < b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a > b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(x)    ((x > 0) ? (x) : (-x))
#endif
#include <Arduino.h>

//#ifndef KNX_NO_PRINT
void print(const char[]);
void print(char);
void print(unsigned char, int = DEC);
void print(int, int = DEC);
void print(unsigned int, int = DEC);
void print(long, int = DEC);
void print(unsigned long, int = DEC);
void print(long long, int = DEC);
void print(unsigned long long, int = DEC);
void print(double);

void println(const char[]);
void println(char);
void println(unsigned char, int = DEC);
void println(int, int = DEC);
void println(unsigned int, int = DEC);
void println(long, int = DEC);
void println(unsigned long, int = DEC);
void println(long long, int = DEC);
void println(unsigned long long, int = DEC);
void println(double);
void println(void);

void printHex(const char* suffix, const uint8_t *data, size_t length, bool newline = true);
//#endif

const uint8_t* popByte(uint8_t& b, const uint8_t* data);
const uint8_t* popWord(uint16_t& w, const uint8_t* data);
const uint8_t* popInt(uint32_t& i, const uint8_t* data);
const uint8_t* popByteArray(uint8_t* dst, uint32_t size, const uint8_t* data);
uint8_t* pushByte(uint8_t b, uint8_t* data);
uint8_t* pushWord(uint16_t w, uint8_t* data);
uint8_t* pushInt(uint32_t i, uint8_t* data);
uint8_t* pushByteArray(const uint8_t* src, uint32_t size, uint8_t* data);
uint16_t getWord(const uint8_t* data);
uint32_t getInt(const uint8_t* data);

void sixBytesFromUInt64(uint64_t num, uint8_t* toByteArray);
uint64_t sixBytesToUInt64(uint8_t* data);

uint16_t crc16Ccitt(uint8_t* input, uint16_t length);
uint16_t crc16Dnp(uint8_t* input, uint16_t length);

enum ParameterFloatEncodings
{
    Float_Enc_DPT9 = 0,          // 2 Byte. See Chapter 3.7.2 section 3.10 (Datapoint Types 2-Octet Float Value)
    Float_Enc_IEEE754Single = 1, // 4 Byte. C++ float
    Float_Enc_IEEE754Double = 2, // 8 Byte. C++ double
};


//#if defined(ARDUINO_ARCH_SAMD)
// temporary undef until framework-arduino-samd > 1.8.9 is released. See https://github.com/arduino/ArduinoCore-samd/pull/399 for a PR should will probably address this
//#undef max
//#undef min
// end of temporary undef
//#endif
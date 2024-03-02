#pragma  once


#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif


#define getbyte(x,n) (*(((uint8_t*)&(x))+n))
#define htons(x)  ( (getbyte(x,0)<<8) | getbyte(x,1) ) 
#define htonl(x) ( (getbyte(x,0)<<24) | (getbyte(x,1)<<16) | (getbyte(x,2)<<8) | getbyte(x,3) )
#define ntohs(x) htons(x)
#define ntohl(x) htonl(x)

#ifndef MIN
#define MIN(a, b) ((a < b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a > b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(x)    ((x > 0) ? (x) : (-x))
#endif

const uint8_t* popByte(uint8_t* b, const uint8_t* data);
const uint8_t* popWord(uint16_t* w, const uint8_t* data);
const uint8_t* popInt(uint32_t* i, const uint8_t* data);
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
#ifdef __cplusplus
}
#endif



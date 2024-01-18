#pragma once
#include "Arduino.h"

#ifndef KNX_NO_PRINT
    void Print_init(HardwareSerial* Uart_port); 

    void STM_Print_init(UART_HandleTypeDef *hPort); 

    //static Stream* SerialDebug;
//---------------------------------------------
    void print(const char[]);
    void print(char);
    void print(unsigned char, int = DEC);
//---------------------------------------------
//---------------------------------------------
    void print(int, int = DEC);
    void print(unsigned int, int = DEC);
//---------------------------------------------
//---------------------------------------------
    void print(long, int = DEC);
    void print(unsigned long, int = DEC);
    void print(long long, int = DEC);
    void print(unsigned long long, int = DEC);
//---------------------------------------------
//---------------------------------------------
    void print(double);
//---------------------------------------------
//---------------------------------------------
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
    //void printUint64(uint64_t value, int base = DEC);
#endif

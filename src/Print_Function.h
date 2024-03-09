#pragma once
#include "Arduino.h"
#ifndef KNX_NO_PRINT

void STM_Print_init(UART_HandleTypeDef *hPort);

int Cal_Real_length(char data);
//---------------------------------------------
void print(const char *s);
void print(char c);
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
void printHex(const char *suffix, const uint8_t *data, size_t length, bool newline = true);

#endif

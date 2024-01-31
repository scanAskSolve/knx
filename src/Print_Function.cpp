#include "Print_Function.h"


#ifndef KNX_NO_PRINT

UART_HandleTypeDef *Port;

void STM_Print_init(UART_HandleTypeDef *hPort){
    Port = hPort;
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
        HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    }
} 

int Cal_Real_length(char *data){
    int cnt = 0;
    while(data[cnt] != 0x00){
        cnt++;
    }

    return cnt;
}
//---------------------------------------------
void print(const char* s)
{
    HAL_UART_Transmit(Port,(uint8_t *)s ,strlen(s),1000);
}
void print(char c)
{
    char array[4] = {0};
    sprintf(array, "%c", c);
    HAL_UART_Transmit(Port,(uint8_t *) array ,Cal_Real_length(array),1000);
}

void print(unsigned char num)
{   
    char array[4] = {0};
    sprintf(array, "%c", num);
    HAL_UART_Transmit(Port,(uint8_t *) array ,Cal_Real_length(array),1000);
}

void print(unsigned char num, int base)
{
    char array[10] = {0}; 

    if (base == HEX)
    {
        sprintf(array, "%x", num);
    }
    else
    {
        sprintf(array, "%d", num);
    }
    HAL_UART_Transmit(Port, (uint8_t*)array, Cal_Real_length(array),1000);
}
//---------------------------------------------
//---------------------------------------------
void print(int num)
{
    char array[4] = {0};
    sprintf(array, "%d", num);
    HAL_UART_Transmit(Port, (uint8_t*)array, Cal_Real_length(array),1000);
}

void print(int num, int base)
{
    char array[12] = {0}; 

    if (base == HEX)
    {
        sprintf(array, "%x", num);
    }
    else
    {
        sprintf(array, "%d", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, Cal_Real_length(array),1000);
}

void print(unsigned int num)
{
    char array[4] = {0};
    sprintf(array, "%d", num);
    HAL_UART_Transmit(Port, (uint8_t*)array, Cal_Real_length(array),1000);
}

void print(unsigned int num, int base)
{
    char array[12] = {0}; 

    if (base == HEX)
    {
        sprintf(array, "%x", num);
    }
    else
    {
        sprintf(array, "%d", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, Cal_Real_length(array),1000);
}
//---------------------------------------------
//---------------------------------------------
void print(long num)
{
    char array[12] = {0}; 
    sprintf(array, "%lx", num);
    HAL_UART_Transmit(Port, (uint8_t*)array, Cal_Real_length(array),1000);
}

void print(long num, int base)
{
    char array[12]; 

    if (base == HEX)
    {
        sprintf(array, "%lx", num);
    }
    else
    {
        sprintf(array, "%ld", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, Cal_Real_length(array), 1000);
}

void print(unsigned long num)
{
    char array[40] = {0}; 
    sprintf(array, "%ld", num);
    HAL_UART_Transmit(Port, (uint8_t*)array, Cal_Real_length(array),1000);
}

void print(unsigned long num, int base)
{
    char array[12];
    if (base == HEX)
    {
        sprintf(array, "%lx", num);
    }
    else
    {
        sprintf(array, "%ld", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, Cal_Real_length(array), 1000);
}

void print(unsigned long long num)
{
    char array[40] = {0};
    sprintf(array, "%llu", num);
    HAL_UART_Transmit(Port, (uint8_t*)array, Cal_Real_length(array),1000);
}

void print(unsigned long long num, int base)
{
    char array[20] = {0}; 

    if (base == HEX)
    {
        sprintf(array, "%llx", num);
    }
    else
    {
        sprintf(array, "%llu", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, Cal_Real_length(array), 1000);
}
//---------------------------------------------
//---------------------------------------------
void print(double num)
{
    char array[20] = {0}; 
    sprintf(array, "%f", num);
    HAL_UART_Transmit(Port, (uint8_t*)array, Cal_Real_length(array), 1000);
}

#endif // KNX_NO_PRINT

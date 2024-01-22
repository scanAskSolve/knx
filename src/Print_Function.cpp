#include "Print_Function.h"


#ifndef KNX_NO_PRINT

Stream* SerialDebug;
UART_HandleTypeDef *Port;

void Print_init(HardwareSerial* Uart_port){
    SerialDebug = Uart_port;
}

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


 /*MX_USART1_UART_Init();
  MX_USART2_UART_Init();

  while(1){
    HAL_UART_Transmit(&huart1,(uint8_t *)"TEST",sizeof("TEST"),1000);
    HAL_Delay(1000);
  }*/
//---------------------------------------------
void print(const char* s)
{
    uint8_t buffer[256]; // Adjust the size based on the maximum expected string length
    strncpy((char*)buffer, s, sizeof(buffer));
    HAL_UART_Transmit(Port,buffer ,strlen((char*)buffer),1000);
    // HAL_Delay(1000);
    // SerialDebug->print(s);
}
void print(char c)
{
    HAL_UART_Transmit(Port,(uint8_t *) c ,sizeof(c),1000);
    // SerialDebug->print(c);
}

void print(unsigned char num)
{
    HAL_UART_Transmit(Port,(uint8_t *) &num ,sizeof(num),1000);
    // SerialDebug->print(num);
}

void print(unsigned char num, int base)
{
    // SerialDebug->print(num, base);
    char array[12]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    if (base == 16)
    {
        sprintf(array, "%x", num);
    }
    else
    {
        sprintf(array, "%d", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array),1000);
}
//---------------------------------------------
//---------------------------------------------
void print(int num)
{
    uint8_t buffer[4]; // Assuming a 32-bit integer, adjust size accordingly
    for (int i = 0; i < sizeof(buffer); ++i) {
        buffer[i] = (num >> (8 * i)) & 0xFF;
    }

    HAL_UART_Transmit(Port, buffer, sizeof(buffer),1000);
    // SerialDebug->print(num);
}

void print(int num, int base)
{
    char array[12]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    if (base == 16)
    {
        sprintf(array, "%x", num);
    }
    else
    {
        sprintf(array, "%d", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array),1000);
    // SerialDebug->print(num, base);
}

void print(unsigned int num)
{
    uint8_t buffer[4]; // Assuming a 32-bit uint32_t, adjust size accordingly
    for (int i = 0; i < sizeof(buffer); ++i) {
        buffer[i] = (num >> (8 * i)) & 0xFF;
    }

    HAL_UART_Transmit(Port, buffer, sizeof(buffer),1000);
    // SerialDebug->print(num);
}

void print(unsigned int num, int base)
{
    char array[12]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    if (base == 16)
    {
        sprintf(array, "%x", num);
    }
    else
    {
        sprintf(array, "%d", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array),1000);
    // SerialDebug->print(num, base);
}
//---------------------------------------------
//---------------------------------------------
void print(long num)
{
    uint8_t buffer[sizeof(long)]; // Adjust size accordingly
    for (int i = 0; i < sizeof(buffer); ++i) {
        buffer[i] = (num >> (8 * i)) & 0xFF;
    }
    HAL_UART_Transmit(Port, buffer, sizeof(buffer),1000);
    // SerialDebug->print(num);
}

void print(long num, int base)
{
    char array[12]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    if (base == 16)
    {
        sprintf(array, "%lx", num);
    }
    else
    {
        sprintf(array, "%ld", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array), 1000);
    // SerialDebug->print(num, base);
}

void print(unsigned long num)
{
    uint8_t buffer[sizeof(long)]; // Adjust size accordingly
    for (int i = 0; i < sizeof(buffer); ++i) {
        buffer[i] = (num >> (8 * i)) & 0xFF;
    }
    HAL_UART_Transmit(Port, buffer, sizeof(buffer),1000);
    // SerialDebug->print(num);
}

void print(unsigned long num, int base)
{
    char array[12]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    if (base == 16)
    {
        sprintf(array, "%lx", num);
    }
    else
    {
        sprintf(array, "%ld", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array), 1000);
    // SerialDebug->print(num, base);
}

void print(unsigned long long num)
{
    uint8_t buffer[sizeof(long long)]; // Adjust size accordingly
    for (int i = 0; i < sizeof(buffer); ++i) {
        buffer[i] = (num >> (8 * i)) & 0xFF;
    }
    HAL_UART_Transmit(Port, buffer, sizeof(buffer),1000);
    // printUint64(num);
}

void print(unsigned long long num, int base)
{
    // printUint64(num, base);
    char array[20]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    if (base == 16)
    {
        sprintf(array, "%llx", num);
    }
    else
    {
        sprintf(array, "%llu", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array), 1000);
    // SerialDebug->print(num, base);
}
//---------------------------------------------
//---------------------------------------------
void print(double num)
{
    char array[20]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    sprintf(array, "%f", num);

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array), 1000);
    // SerialDebug->print(num);
}
//---------------------------------------------
//---------------------------------------------
void println(const char* s)
{
    uint8_t buffer[256]; // Adjust the size based on the maximum expected string length
    strncpy((char*)buffer, s, sizeof(buffer));
    HAL_UART_Transmit(Port,buffer ,strlen((char*)buffer),1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // HAL_Delay(1000);
    // SerialDebug->print(s);
}

void println(char c)
{
    HAL_UART_Transmit(Port,(uint8_t *) c ,sizeof(c),1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // SerialDebug->print(c);
}

void println(unsigned char num)
{
    HAL_UART_Transmit(Port,(uint8_t *) &num ,sizeof(num),1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // SerialDebug->print(num);
}

void println(unsigned char num, int base)
{
    // SerialDebug->print(num, base);
    char array[12]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    if (base == 16)
    {
        sprintf(array, "%x", num);
    }
    else
    {
        sprintf(array, "%d", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array),1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
}

void println(int num)
{
    uint8_t buffer[4]; // Assuming a 32-bit integer, adjust size accordingly
    for (int i = 0; i < sizeof(buffer); ++i) {
        buffer[i] = (num >> (8 * i)) & 0xFF;
    }

    HAL_UART_Transmit(Port, buffer, sizeof(buffer),1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // SerialDebug->print(num);
}

void println(int num, int base)
{
    char array[12]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    if (base == 16)
    {
        sprintf(array, "%x", num);
    }
    else
    {
        sprintf(array, "%d", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array),1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // SerialDebug->print(num, base);
}

void println(unsigned int num)
{
    uint8_t buffer[4]; // Assuming a 32-bit uint32_t, adjust size accordingly
    for (int i = 0; i < sizeof(buffer); ++i) {
        buffer[i] = (num >> (8 * i)) & 0xFF;
    }

    HAL_UART_Transmit(Port, buffer, sizeof(buffer),1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // SerialDebug->print(num);
}

void println(unsigned int num, int base)
{
    char array[12]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    if (base == 16)
    {
        sprintf(array, "%x", num);
    }
    else
    {
        sprintf(array, "%d", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array),1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // SerialDebug->print(num, base);
}

void println(long num)
{
    uint8_t buffer[sizeof(long)]; // Adjust size accordingly
    for (int i = 0; i < sizeof(buffer); ++i) {
        buffer[i] = (num >> (8 * i)) & 0xFF;
    }
    HAL_UART_Transmit(Port, buffer, sizeof(buffer),1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // SerialDebug->print(num);
}

void println(long num, int base)
{
    char array[12]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    if (base == 16)
    {
        sprintf(array, "%lx", num);
    }
    else
    {
        sprintf(array, "%ld", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array), 1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // SerialDebug->print(num, base);
}

void println(unsigned long num)
{
    uint8_t buffer[sizeof(long)]; // Adjust size accordingly
    for (int i = 0; i < sizeof(buffer); ++i) {
        buffer[i] = (num >> (8 * i)) & 0xFF;
    }
    HAL_UART_Transmit(Port, buffer, sizeof(buffer),1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // SerialDebug->print(num);
}

void println(unsigned long num, int base)
{
    char array[12]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    if (base == 16)
    {
        sprintf(array, "%lx", num);
    }
    else
    {
        sprintf(array, "%ld", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array), 1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // SerialDebug->print(num, base);
}

void println(unsigned long long num)
{
    uint8_t buffer[sizeof(long long)]; // Adjust size accordingly
    for (int i = 0; i < sizeof(buffer); ++i) {
        buffer[i] = (num >> (8 * i)) & 0xFF;
    }
    HAL_UART_Transmit(Port, buffer, sizeof(buffer),1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // printUint64(num);
}

void println(unsigned long long num, int base)
{
    // printUint64(num, base);
    char array[20]; // Adjust the size based on the maximum expected string length
    array[0] = '\0'; // Ensure the string is initially empty

    if (base == 16)
    {
        sprintf(array, "%llx", num);
    }
    else
    {
        sprintf(array, "%llu", num);
    }

    HAL_UART_Transmit(Port, (uint8_t*)array, sizeof(array), 1000);
    HAL_UART_Transmit(Port, (uint8_t*)"\n\r", strlen("\n\r"), 1000);
    // SerialDebug->print(num, base);
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

#pragma once
#include ".\knx\stm32_eeprom.h"
#include "Arduino.h"

#ifndef KNX_FLASH_CALLBACK

#ifndef KNX_FLASH_SIZE
#define KNX_FLASH_SIZE 1024
#endif
#endif

#ifndef KNX_MV_MEMORY
#define KNX_MV_MEMORY 1
enum NvMemoryType
{
    Eeprom,
    Flash,
    Callback
};
#endif

void KNX_UART_Init(HardwareSerial *knxSerial);

// uart
void knxUart(HardwareSerial *serial);
HardwareSerial *knxUart();
void setupUart();
void closeUart();
int uartAvailable();
size_t writeUart(const uint8_t data);
size_t writeUart(const uint8_t *buffer, size_t size);
int readUart();
size_t readBytesUart(uint8_t *buffer, size_t length);

// basic stuff
void fatalError();
void restart();

uint32_t uniqueSerialNumber();
uint8_t *getNonVolatileMemoryStart();
// start of user flash aligned to start of an erase block
uint8_t *userFlashStart();
uint8_t *getEepromBuffer(uint32_t size);
size_t getNonVolatileMemorySize();
// size of the user flash in EraseBlocks
size_t userFlashSizeEraseBlocks();
// size of one EraseBlock in pages
size_t flashEraseBlockSize();
// size of one flash page in bytes
size_t flashPageSize();

// address is relative to start of nonvolatile memory
uint32_t writeNonVolatileMemory(uint32_t relativeAddress, uint8_t *buffer, size_t size);
uint32_t writeNonVolatileMemory(uint32_t relativeAddress, uint8_t value, size_t repeat);
// spi
void commitNonVolatileMemory();
// copies a EraseBlock into the _eraseblockBuffer
void bufferEraseBlock(int32_t eraseBlockNumber);
void loadEraseblockContaining(uint32_t relativeAddress);
int32_t getEraseBlockNumberOf(uint32_t relativeAddress);
// writes _eraseblockBuffer to flash
void writeBufferedEraseBlock();
// relativ to userFlashStart
void flashErase(uint16_t eraseBlockNum);
void flashWritePage(uint16_t pageNumber, uint8_t *data);
void commitToEeprom();

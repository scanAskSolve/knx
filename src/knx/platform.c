#include "bits.h"

#include "platform.h"



//#include <cstring>
#include "string.h"

//#include <cstdlib>
#include "stdlib.h"

#include <stddef.h>
#include <stdbool.h>


NvMemoryType _memoryType;

// in theory we would have to use this buffer for memory reads too,
// but because ets always restarts the device after programming it
// we can ignore this issue
uint8_t* _eraseblockBuffer;
int32_t _bufferedEraseblockNumber;
bool _bufferedEraseblockDirty;



NvMemoryType Get_NonVolatileMemoryType()
{
    return _memoryType;
}

void Set_NonVolatileMemoryType(NvMemoryType type)
{
    _memoryType = type;
}
/*
void Platform_setupSpi()
{}

void Platform_closeSpi()
{}

int Platform_readWriteSpi(uint8_t *data, size_t len)
{
    return 0;
}*/

size_t readBytesUart(uint8_t *buffer, size_t length)
{
    return 0;
}

int readUart()
{
    return -1;
}

size_t writeUart_buffer(const uint8_t *buffer, size_t size)
{
    return 0;
}

size_t writeUart_data(const uint8_t data)
{
    return 0;
}

int uartAvailable()
{
    return 0;
}

void closeUart()
{}

void setupUart()
{}

/*uint32_t Platform_currentIpAddress()
{
    return 0x01020304;
}

uint32_t Platform_currentSubnetMask()
{
    return 0;
}

uint32_t Platform_currentDefaultGateway()
{
    return 0;
}

void Platform_macAddress(uint8_t *data)
{}*/

uint32_t uniqueSerialNumber()
{
    return 0x01020304;
}

void setupMultiCast(uint32_t addr, uint16_t port)
{}

void closeMultiCast()
{}

bool sendBytesMultiCast(uint8_t *buffer, uint16_t len)
{
    return false;
}

bool sendBytesUniCast(uint32_t addr, uint16_t port, uint8_t* buffer, uint16_t len)
{
    return false;
}

int readBytesMultiCast(uint8_t *buffer, uint16_t maxLen)
{
    return 0;
}

size_t flashEraseBlockSize()
{
    return 0;
}

size_t flashPageSize()
{
    // align to 32bit as default for Eeprom Emulation plattforms
    return 4;
}

uint8_t * userFlashStart()
{
    return NULL;
}

size_t userFlashSizeEraseBlocks()
{
    return 0;
}

void flashErase(uint16_t eraseBlockNum)
{}

void flashWritePage(uint16_t pageNumber, uint8_t* data)
{}

uint8_t * getEepromBuffer(uint32_t size)
{
    return NULL;
}

void commitToEeprom()
{}

uint8_t* getNonVolatileMemoryStart()
{
    if(_memoryType == Flash)
        return userFlashStart();
    else
        return getEepromBuffer(KNX_FLASH_SIZE);
}

size_t getNonVolatileMemorySize()
{
    if(_memoryType == Flash)
        return userFlashSizeEraseBlocks() * flashEraseBlockSize() * flashPageSize();
    else
        return KNX_FLASH_SIZE;
}

void commitNonVolatileMemory()
{
    if(_memoryType == Flash)
    {
        if(_bufferedEraseblockNumber > -1 && _bufferedEraseblockDirty)
        {
            writeBufferedEraseBlock();
            
            free(_eraseblockBuffer);
            _eraseblockBuffer = NULL;
            _bufferedEraseblockNumber = -1;  // does that make sense?
        }
    }
    else
    {
        commitToEeprom();
    }
}

uint32_t writeNonVolatileMemory_Arrary(uint32_t relativeAddress, uint8_t* buffer, size_t size)
{
    if(_memoryType == Flash)
    {
        while (size > 0)
        {
            loadEraseblockContaining(relativeAddress);
            uint32_t start = _bufferedEraseblockNumber * (flashEraseBlockSize() * flashPageSize());
            uint32_t end = start +  (flashEraseBlockSize() * flashPageSize());

            uint32_t offset = relativeAddress - start;
            uint32_t length = end - relativeAddress;
            if(length > size)
                length = size;
            memcpy(_eraseblockBuffer + offset, buffer, length);
            _bufferedEraseblockDirty = true;

            relativeAddress += length;
            buffer += length;
            size -= length;
        }
        return relativeAddress;
    }
    else
    {
        memcpy(getEepromBuffer(KNX_FLASH_SIZE)+relativeAddress, buffer, size);
        return relativeAddress+size;
    }
}

// writes value repeat times into flash starting at relativeAddress
// returns next free relativeAddress
uint32_t writeNonVolatileMemory_value(uint32_t relativeAddress, uint8_t value, size_t repeat)
{
    if(_memoryType == Flash)
    {
        while (repeat > 0)
        {
            loadEraseblockContaining(relativeAddress);
            uint32_t start = _bufferedEraseblockNumber * (flashEraseBlockSize() * flashPageSize());
            uint32_t end = start +  (flashEraseBlockSize() * flashPageSize());

            uint32_t offset = relativeAddress - start;
            uint32_t length = end - relativeAddress;
            if(length > repeat)
                length = repeat;
            memset(_eraseblockBuffer + offset, value, length);
            _bufferedEraseblockDirty = true;

            relativeAddress += length;
            repeat -= length;
        }
        return relativeAddress;
    }
    else
    {
        memset(getEepromBuffer(KNX_FLASH_SIZE)+relativeAddress, value, repeat);
        return relativeAddress+repeat;
    }
}

void loadEraseblockContaining(uint32_t relativeAddress)
{
    int32_t blockNum = getEraseBlockNumberOf(relativeAddress);
    if (blockNum < 0)
    {
        println("loadEraseblockContaining could not get valid eraseblock number");
        fatalError();
    }

    if (blockNum != _bufferedEraseblockNumber && _bufferedEraseblockNumber >= 0)
        writeBufferedEraseBlock();

    bufferEraseBlock(blockNum);
}

uint32_t getEraseBlockNumberOf(uint32_t relativeAddress)
{
    return relativeAddress / (flashEraseBlockSize() * flashPageSize());
}


void writeBufferedEraseBlock()
{
    if(_bufferedEraseblockNumber > -1 && _bufferedEraseblockDirty)
    {
        flashErase(_bufferedEraseblockNumber);
        for(uint32_t i = 0; i < flashEraseBlockSize(); i++)
        {   
            int32_t pageNumber = _bufferedEraseblockNumber * flashEraseBlockSize() + i;
            uint8_t *data = _eraseblockBuffer + flashPageSize() * i;
            flashWritePage(pageNumber, data);
        }
        _bufferedEraseblockDirty = false;
    }
}


void bufferEraseBlock(int32_t eraseBlockNumber)
{
    if(_bufferedEraseblockNumber == eraseBlockNumber)
        return;
    
    if(_eraseblockBuffer == NULL)
    {
        _eraseblockBuffer = (uint8_t*)malloc(flashEraseBlockSize() * flashPageSize());
    }
    memcpy(_eraseblockBuffer, userFlashStart() + eraseBlockNumber * flashEraseBlockSize() * flashPageSize(), flashEraseBlockSize() * flashPageSize());

    _bufferedEraseblockNumber = eraseBlockNumber;
    _bufferedEraseblockDirty = false;
}

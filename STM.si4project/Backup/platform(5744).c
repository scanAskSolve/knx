#include "platform.h"

#include "bits.h"

//#include <cstring>
#include "string.h"

//#include <cstdlib>
#include "stdlib.h"


NvMemoryType Platform_NonVolatileMemoryType(struct Platform* self())
{
    return _memoryType;
}

void Platform_NonVolatileMemoryType(struct Platform* self(NvMemoryType type))
{
    _memoryType = type;
}
/*
void Platform_setupSpi(struct Platform* self())
{}

void Platform_closeSpi(struct Platform* self())
{}

int Platform_readWriteSpi(struct Platform* self(uint8_t *data, size_t len))
{
    return 0;
}*/

size_t Platform_readBytesUart(struct Platform* self(uint8_t *buffer, size_t length))
{
    return 0;
}

int Platform_readUart(struct Platform* self())
{
    return -1;
}

size_t Platform_writeUart(struct Platform* self(const uint8_t *buffer, size_t size))
{
    return 0;
}

size_t Platform_writeUart(struct Platform* self(const uint8_t data))
{
    return 0;
}

int Platform_uartAvailable(struct Platform* self())
{
    return 0;
}

void Platform_closeUart(struct Platform* self())
{}

void Platform_setupUart(struct Platform* self())
{}

/*uint32_t Platform_currentIpAddress(struct Platform* self())
{
    return 0x01020304;
}

uint32_t Platform_currentSubnetMask(struct Platform* self())
{
    return 0;
}

uint32_t Platform_currentDefaultGateway(struct Platform* self())
{
    return 0;
}

void Platform_macAddress(struct Platform* self(uint8_t *data))
{}*/

uint32_t uniqueSerialNumber(struct Platform* self())
{
    return 0x01020304;
}

void setupMultiCast(struct Platform* self(uint32_t addr, uint16_t port))
{}

void closeMultiCast(struct Platform* self())
{}

bool sendBytesMultiCast(struct Platform* self(uint8_t *buffer, uint16_t len))
{
    return false;
}

bool sendBytesUniCast(struct Platform* self(uint32_t addr, uint16_t port, uint8_t* buffer, uint16_t len))
{
    return false;
}

int readBytesMultiCast(struct Platform* self(uint8_t *buffer, uint16_t maxLen))
{
    return 0;
}

size_t flashEraseBlockSize(struct Platform* self())
{
    return 0;
}

size_t flashPageSize(struct Platform* self())
{
    // align to 32bit as default for Eeprom Emulation plattforms
    return 4;
}

uint8_t * userFlashStart(struct Platform* self())
{
    return NULL;
}

size_t userFlashSizeEraseBlocks(struct Platform* self())
{
    return 0;
}

void flashErase(struct Platform* self(uint16_t eraseBlockNum))
{}

void flashWritePage(struct Platform* self(uint16_t pageNumber, uint8_t* data))
{}

uint8_t * getEepromBuffer(struct Platform* self(uint32_t size))
{
    return NULL;
}

void commitToEeprom(struct Platform* self())
{}

uint8_t* getNonVolatileMemoryStart(struct Platform* self())
{
    if(_memoryType == Flash)
        return userFlashStart();
    else
        return getEepromBuffer(KNX_FLASH_SIZE);
}

size_t getNonVolatileMemorySize(struct Platform* self())
{
    if(_memoryType == Flash)
        return userFlashSizeEraseBlocks() * flashEraseBlockSize() * flashPageSize();
    else
        return KNX_FLASH_SIZE;
}

void commitNonVolatileMemory(struct Platform* self())
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

uint32_t writeNonVolatileMemory(struct Platform* self(uint32_t relativeAddress, uint8_t* buffer, size_t size))
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
uint32_t writeNonVolatileMemory(struct Platform* self(uint32_t relativeAddress, uint8_t value, size_t repeat))
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

void loadEraseblockContaining(struct Platform* self(uint32_t relativeAddress))
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

int32_t getEraseBlockNumberOf(struct Platform* self(uint32_t relativeAddress))
{
    return relativeAddress / (flashEraseBlockSize() * flashPageSize());
}


void writeBufferedEraseBlock(struct Platform* self())
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


void bufferEraseBlock(struct Platform* self(int32_t eraseBlockNumber))
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

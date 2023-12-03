#include "arduino_platform.h"
#include "knx/bits.h"
#include <EEPROM.h>

#include <Arduino.h>

#ifndef KNX_NO_PRINT
Stream* ArduinoPlatform::SerialDebug = &KNX_DEBUG_SERIAL;
#endif

ArduinoPlatform::ArduinoPlatform() : _knxSerial(nullptr)
{
}

ArduinoPlatform::ArduinoPlatform(HardwareSerial* knxSerial) : _knxSerial(knxSerial)
{
}
/*ArduinoPlatform::~ArduinoPlatform()
{
        //delete [] _eepromPtr;
	free(_eepromPtr);	
}*/
void ArduinoPlatform::fatalError()
{
    while (true)
    {
#ifdef KNX_LED
        static const long LED_BLINK_PERIOD = 200;

        if ((HAL_GetTick() % LED_BLINK_PERIOD) > (LED_BLINK_PERIOD / 2))
            digitalWrite(KNX_LED, HIGH);
        else
            digitalWrite(KNX_LED, LOW);
#endif
    }
}

void ArduinoPlatform::knxUart( HardwareSerial* serial )
{
    if (_knxSerial)
        closeUart();
    _knxSerial = serial;
    setupUart();
}

HardwareSerial* ArduinoPlatform::knxUart()
{
    return _knxSerial;
}

void ArduinoPlatform::setupUart()
{
    _knxSerial->begin(19200, SERIAL_8E1);
    while (!_knxSerial) 
        ;
}


void ArduinoPlatform::closeUart()
{
    _knxSerial->end();
}


int ArduinoPlatform::uartAvailable()
{
    return _knxSerial->available();
}


size_t ArduinoPlatform::writeUart(const uint8_t data)
{
    //printHex("<p", &data, 1);
    return _knxSerial->write(data);
}


size_t ArduinoPlatform::writeUart(const uint8_t *buffer, size_t size)
{
    //printHex("<p", buffer, size);
    return _knxSerial->write(buffer, size);
}


int ArduinoPlatform::readUart()
{
    int val = _knxSerial->read();
    //if(val > 0)
    //    printHex("p>", (uint8_t*)&val, 1);
    return val;
}


size_t ArduinoPlatform::readBytesUart(uint8_t *buffer, size_t length)
{
    size_t toRead = length;
    uint8_t* pos = buffer;
    while (toRead > 0)
    {
        size_t val = _knxSerial->readBytes(pos, toRead);
        pos += val;
        toRead -= val;
    }
    //printHex("p>", buffer, length);
    return length;
}

uint32_t ArduinoPlatform::uniqueSerialNumber()
{
    //return 0x01020304;
    return HAL_GetUIDw0() ^ HAL_GetUIDw1() ^ HAL_GetUIDw2();
}

uint8_t* ArduinoPlatform::getNonVolatileMemoryStart()
{
    if(_memoryType == Flash)
        return userFlashStart();
    else
        return getEepromBuffer(KNX_FLASH_SIZE);
}

uint8_t *ArduinoPlatform::userFlashStart()
{
    return nullptr;
}

uint8_t * ArduinoPlatform::getEepromBuffer(uint32_t size)
{
    //return nullptr;


    // check if the buffer already exists
    if (_eepromPtr == nullptr) // we need to initialize the buffer first
    {
        if (size > E2END + 1)
        {
            fatalError();
        }

        _eepromSize = size;
        _eepromPtr = new uint8_t[size];
        eeprom_buffer_fill();
        for (uint16_t i = 0; i < size; ++i)
            _eepromPtr[i] = eeprom_buffered_read_byte(i);
    }
    
    return _eepromPtr;
}

size_t ArduinoPlatform::getNonVolatileMemorySize()
{
    if(_memoryType == Flash)
        return userFlashSizeEraseBlocks() * flashEraseBlockSize() * flashPageSize();
    else
        return KNX_FLASH_SIZE;
}

size_t ArduinoPlatform::userFlashSizeEraseBlocks()
{
    return 0;
}

size_t ArduinoPlatform::flashEraseBlockSize()
{
    return 0;
}

size_t ArduinoPlatform::flashPageSize()
{
    // align to 32bit as default for Eeprom Emulation plattforms
    return 4;
}

uint32_t ArduinoPlatform::writeNonVolatileMemory(uint32_t relativeAddress, uint8_t* buffer, size_t size)
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
uint32_t ArduinoPlatform::writeNonVolatileMemory(uint32_t relativeAddress, uint8_t value, size_t repeat)
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

void ArduinoPlatform::loadEraseblockContaining(uint32_t relativeAddress)
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

int32_t ArduinoPlatform::getEraseBlockNumberOf(uint32_t relativeAddress)
{
    return relativeAddress / (flashEraseBlockSize() * flashPageSize());
}

void ArduinoPlatform::writeBufferedEraseBlock()
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

void ArduinoPlatform::bufferEraseBlock(int32_t eraseBlockNumber)
{
    if(_bufferedEraseblockNumber == eraseBlockNumber)
        return;
    
    if(_eraseblockBuffer == nullptr)
    {
        _eraseblockBuffer = (uint8_t*)malloc(flashEraseBlockSize() * flashPageSize());
    }
    memcpy(_eraseblockBuffer, userFlashStart() + eraseBlockNumber * flashEraseBlockSize() * flashPageSize(), flashEraseBlockSize() * flashPageSize());

    _bufferedEraseblockNumber = eraseBlockNumber;
    _bufferedEraseblockDirty = false;
}

void ArduinoPlatform::flashErase(uint16_t eraseBlockNum)
{}

void ArduinoPlatform::flashWritePage(uint16_t pageNumber, uint8_t* data)
{}

void ArduinoPlatform::commitNonVolatileMemory()
{
    if(_memoryType == Flash)
    {
        if(_bufferedEraseblockNumber > -1 && _bufferedEraseblockDirty)
        {
            writeBufferedEraseBlock();
            
            free(_eraseblockBuffer);
            _eraseblockBuffer = nullptr;
            _bufferedEraseblockNumber = -1;  // does that make sense?
        }
    }
    else
    {
        commitToEeprom();
    }
}

void ArduinoPlatform::commitToEeprom()
{
    if(_eepromPtr == nullptr || _eepromSize == 0)
        return;
    for (uint16_t i = 0; i < _eepromSize; ++i)
        eeprom_buffered_write_byte(i, _eepromPtr[i]);
    // For some GD32 chips, the flash needs to be unlocked twice
    // and the first call will fail. If the first call is
    // successful, the second one (inside eeprom_buffer_flush)
    // does nothing.
    HAL_FLASH_Unlock();
    eeprom_buffer_flush();
}
/*#ifndef KNX_NO_SPI

void ArduinoPlatform::setupSpi()
{
    SPI.begin();
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
}

void ArduinoPlatform::closeSpi()
{
    SPI.endTransaction();
    SPI.end();
}

int ArduinoPlatform::readWriteSpi(uint8_t *data, size_t len)
{
    SPI.transfer(data, len);
    return 0;
}
#endif*/

void ArduinoPlatform::restart()
{
    NVIC_SystemReset();
}

#ifndef KNX_NO_PRINT

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
    ArduinoPlatform::SerialDebug->print(s);
}
void print(char c)
{
    ArduinoPlatform::SerialDebug->print(c);
}

void print(unsigned char num)
{
    ArduinoPlatform::SerialDebug->print(num);
}

void print(unsigned char num, int base)
{
    ArduinoPlatform::SerialDebug->print(num, base);
}

void print(int num)
{
    ArduinoPlatform::SerialDebug->print(num);
}

void print(int num, int base)
{
    ArduinoPlatform::SerialDebug->print(num, base);
}

void print(unsigned int num)
{
    ArduinoPlatform::SerialDebug->print(num);
}

void print(unsigned int num, int base)
{
    ArduinoPlatform::SerialDebug->print(num, base);
}

void print(long num)
{
    ArduinoPlatform::SerialDebug->print(num);
}

void print(long num, int base)
{
    ArduinoPlatform::SerialDebug->print(num, base);
}

void print(unsigned long num)
{
    ArduinoPlatform::SerialDebug->print(num);
}

void print(unsigned long num, int base)
{
    ArduinoPlatform::SerialDebug->print(num, base);
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
    ArduinoPlatform::SerialDebug->print(num);
}

void println(const char* s)
{
    ArduinoPlatform::SerialDebug->println(s);
}

void println(char c)
{
    ArduinoPlatform::SerialDebug->println(c);
}

void println(unsigned char num)
{
    ArduinoPlatform::SerialDebug->println(num);
}

void println(unsigned char num, int base)
{
    ArduinoPlatform::SerialDebug->println(num, base);
}

void println(int num)
{
    ArduinoPlatform::SerialDebug->println(num);
}

void println(int num, int base)
{
    ArduinoPlatform::SerialDebug->println(num, base);
}

void println(unsigned int num)
{
    ArduinoPlatform::SerialDebug->println(num);
}

void println(unsigned int num, int base)
{
    ArduinoPlatform::SerialDebug->println(num, base);
}

void println(long num)
{
    ArduinoPlatform::SerialDebug->println(num);
}

void println(long num, int base)
{
    ArduinoPlatform::SerialDebug->println(num, base);
}

void println(unsigned long num)
{
    ArduinoPlatform::SerialDebug->println(num);
}

void println(unsigned long num, int base)
{
    ArduinoPlatform::SerialDebug->println(num, base);
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
    ArduinoPlatform::SerialDebug->println(num);
}

void println(void)
{
    ArduinoPlatform::SerialDebug->println();
}
#endif // KNX_NO_PRINT

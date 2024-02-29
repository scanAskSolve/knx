#include "arduino_platform.h"
#include "Print_Function.h"
#include "knx/bits.h"
//#include <EEPROM.h>
//#include "stm32_eeprom.h"


NvMemoryType _memoryType = Eeprom;
int32_t _bufferedEraseblockNumber = -1;
uint8_t* _eraseblockBuffer = nullptr;
bool _bufferedEraseblockDirty = false;
uint8_t *_eepromPtr = nullptr;
uint16_t _eepromSize = 0;

HardwareSerial *_knxUart;

void KNX_UART_Init(HardwareSerial* knxSerial){
    _knxUart = knxSerial;
}

void fatalError()
{
    while (true)
    {
        //EDA Not fix 
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(200);
    }
}

void knxUart( HardwareSerial* serial )
{
    if (_knxUart)
        closeUart();
    _knxUart = serial;
    setupUart();
}

HardwareSerial* knxUart()
{
    return _knxUart;
}

void setupUart()
{
    _knxUart->begin(19200, SERIAL_8E1);
    //_knxUart->begin(38400, SERIAL_8N1);
    while (!_knxUart) 
        ;
}


void closeUart()
{
    _knxUart->end();
}


int uartAvailable()
{
    return _knxUart->available();
}


size_t writeUart(const uint8_t data)
{
    //printHex("<p", &data, 1);
    return _knxUart->write(data);
}


size_t writeUart(const uint8_t *buffer, size_t size)
{
    //printHex("<p", buffer, size);
    return _knxUart->write(buffer, size);
}


int readUart()
{
    int val = _knxUart->read();
    //if(val > 0)
    //    printHex("p>", (uint8_t*)&val, 1);
    return val;
}


size_t readBytesUart(uint8_t *buffer, size_t length)
{
    size_t toRead = length;
    uint8_t* pos = buffer;
    while (toRead > 0)
    {
        size_t val = _knxUart->readBytes(pos, toRead);
        pos += val;
        toRead -= val;
    }
    //printHex("p>", buffer, length);
    return length;
}

uint32_t uniqueSerialNumber()
{
    //return 0x01020304;
    return HAL_GetUIDw0() ^ HAL_GetUIDw1() ^ HAL_GetUIDw2();
}

uint8_t* getNonVolatileMemoryStart()
{
    if(_memoryType == Flash)
        return userFlashStart();
#ifdef KNX_FLASH_CALLBACK
    else if(_memoryType == Callback)
        return _callbackFlashRead();
#endif
    else
        return getEepromBuffer(KNX_FLASH_SIZE);
}

uint8_t *userFlashStart()
{
    return nullptr;
}

uint8_t * getEepromBuffer(uint32_t size)
{
    //return nullptr;


    // check if the buffer already exists
    if (_eepromPtr == nullptr) // we need to initialize the buffer first
    {
        if (size > FLASH_PAGE_SIZE_END + 1)
        {
            fatalError();
        }

        _eepromSize = size;
        _eepromPtr = new uint8_t[size];
        eeprom_memery_read_to_buffer();
        for (uint16_t i = 0; i < size; ++i)
            _eepromPtr[i] = eeprom_buffered_read_one_byte(i);
    }
    
    return _eepromPtr;
}

size_t getNonVolatileMemorySize()
{
    if(_memoryType == Flash)
        return userFlashSizeEraseBlocks() * flashEraseBlockSize() * flashPageSize();
#ifdef KNX_FLASH_CALLBACK
    else if(_memoryType == Callback)
        return _callbackFlashSize();
#endif
    else
        return KNX_FLASH_SIZE;
}

size_t userFlashSizeEraseBlocks()
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

uint32_t writeNonVolatileMemory(uint32_t relativeAddress, uint8_t* buffer, size_t size)
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
#ifdef KNX_FLASH_CALLBACK
    else if(_memoryType == Callback)
        return _callbackFlashWrite(relativeAddress, buffer, size);
#endif
    else
    {
        memcpy(getEepromBuffer(KNX_FLASH_SIZE)+relativeAddress, buffer, size);
        return relativeAddress+size;
    }
}

// writes value repeat times into flash starting at relativeAddress
// returns next free relativeAddress
uint32_t writeNonVolatileMemory(uint32_t relativeAddress, uint8_t value, size_t repeat)
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
        print("loadEraseblockContaining could not get valid eraseblock number\r\n");
        fatalError();
    }

    if (blockNum != _bufferedEraseblockNumber && _bufferedEraseblockNumber >= 0)
        writeBufferedEraseBlock();

    bufferEraseBlock(blockNum);
}

int32_t getEraseBlockNumberOf(uint32_t relativeAddress)
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
    
    if(_eraseblockBuffer == nullptr)
    {
        _eraseblockBuffer = (uint8_t*)malloc(flashEraseBlockSize() * flashPageSize());
    }
    memcpy(_eraseblockBuffer, userFlashStart() + eraseBlockNumber * flashEraseBlockSize() * flashPageSize(), flashEraseBlockSize() * flashPageSize());

    _bufferedEraseblockNumber = eraseBlockNumber;
    _bufferedEraseblockDirty = false;
}

#ifdef KNX_FLASH_CALLBACK
void ArduinoPlatform::registerFlashCallbacks(
    FlashCallbackSize callbackFlashSize,
    FlashCallbackRead callbackFlashRead,
    FlashCallbackWrite callbackFlashWrite,
    FlashCallbackCommit callbackFlashCommit)
    {
        print("Set Callback\r\n");
        _memoryType = Callback;
        _callbackFlashSize = callbackFlashSize;
        _callbackFlashRead = callbackFlashRead;
        _callbackFlashWrite = callbackFlashWrite;
        _callbackFlashCommit = callbackFlashCommit;
        _callbackFlashSize();
    }

FlashCallbackSize ArduinoPlatform::callbackFlashSize()
{
   return _callbackFlashSize;
}
FlashCallbackRead ArduinoPlatform::callbackFlashRead()
{
   return _callbackFlashRead;
}
FlashCallbackWrite ArduinoPlatform::callbackFlashWrite()
{
   return _callbackFlashWrite;
}
FlashCallbackCommit ArduinoPlatform::callbackFlashCommit()
{
   return _callbackFlashCommit;
}
#endif

void flashErase(uint16_t eraseBlockNum)
{}

void flashWritePage(uint16_t pageNumber, uint8_t* data)
{}

void commitNonVolatileMemory()
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
#ifdef KNX_FLASH_CALLBACK
    else if(_memoryType == Callback)
        return _callbackFlashCommit();
#endif
    else
    {
        commitToEeprom();
    }
}

void commitToEeprom()
{
    if(_eepromPtr == nullptr || _eepromSize == 0)
        return;
    for (uint16_t i = 0; i < _eepromSize; ++i)
        eeprom_buffered_write_one_byte(i, _eepromPtr[i]);
    // For some GD32 chips, the flash needs to be unlocked twice
    // and the first call will fail. If the first call is
    // successful, the second one (inside eeprom_buffer_Write_to_memery)
    // does nothing.
    HAL_FLASH_Unlock();
    eeprom_buffer_Write_to_memery();
}


void restart()
{
    NVIC_SystemReset();
}


// #include "knx/platform.h"
#pragma once
#include "Arduino.h"
#include "knx/save_restore.h"
#ifndef KNX_DEBUG_SERIAL
#define KNX_DEBUG_SERIAL Serial
#endif

#ifndef KNX_FLASH_SIZE
#define KNX_FLASH_SIZE 1024
#pragma warning "KNX_FLASH_SIZE not defined, using 1024"
#endif

#ifndef KNX_MV_MEMORY
#define KNX_MV_MEMORY 1
enum NvMemoryType
{
    Eeprom,
    Flash
};
#endif
class ArduinoPlatform
{
  public:
    ArduinoPlatform();
    ArduinoPlatform(HardwareSerial* knxSerial);

    // basic stuff
    void fatalError();

    //uart
    virtual void knxUart( HardwareSerial* serial);
    virtual HardwareSerial* knxUart();
    virtual void setupUart();
    virtual void closeUart();
    virtual int uartAvailable();
    virtual size_t writeUart(const uint8_t data);
    virtual size_t writeUart(const uint8_t* buffer, size_t size);
    virtual int readUart();
    virtual size_t readBytesUart(uint8_t* buffer, size_t length);

    virtual uint32_t uniqueSerialNumber();
    virtual uint8_t* getNonVolatileMemoryStart();
        // start of user flash aligned to start of an erase block
    virtual uint8_t* userFlashStart();
    virtual uint8_t* getEepromBuffer(uint32_t size);
    virtual size_t getNonVolatileMemorySize();
        // size of the user flash in EraseBlocks
    virtual size_t userFlashSizeEraseBlocks();
    // size of one EraseBlock in pages
    virtual size_t flashEraseBlockSize();
    // size of one flash page in bytes
    virtual size_t flashPageSize();

    // address is relative to start of nonvolatile memory
    virtual uint32_t writeNonVolatileMemory(uint32_t relativeAddress, uint8_t* buffer, size_t size);
    virtual uint32_t writeNonVolatileMemory(uint32_t relativeAddress, uint8_t value, size_t repeat);
    //spi
    virtual void commitNonVolatileMemory();
    // copies a EraseBlock into the _eraseblockBuffer
    void bufferEraseBlock(int32_t eraseBlockNumber);
    void loadEraseblockContaining(uint32_t relativeAddress);
    int32_t getEraseBlockNumberOf(uint32_t relativeAddress);
    // writes _eraseblockBuffer to flash
    virtual void writeBufferedEraseBlock();
    //relativ to userFlashStart
    virtual void flashErase(uint16_t eraseBlockNum);
    virtual void flashWritePage(uint16_t pageNumber, uint8_t* data); 
    virtual void commitToEeprom();

    virtual void restart() = 0;
/*#ifndef KNX_NO_SPI

    void setupSpi() override;
    void closeSpi() override;
    int readWriteSpi (uint8_t *data, size_t len) override;
    
#endif*/
#ifndef KNX_NO_PRINT
    static Stream* SerialDebug;

#endif

  protected:
    HardwareSerial* _knxSerial;
    NvMemoryType _memoryType = Eeprom;
    int32_t _bufferedEraseblockNumber = -1;
    uint8_t* _eraseblockBuffer = nullptr;
    bool _bufferedEraseblockDirty = false;
};

#ifndef KNX_NO_PRINT
    void print(const char[]);
    void print(char);
    void print(unsigned char, int = DEC);
    void print(int, int = DEC);
    void print(unsigned int, int = DEC);
    void print(long, int = DEC);
    void print(unsigned long, int = DEC);
    void print(long long, int = DEC);
    void print(unsigned long long, int = DEC);
    void print(double);

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
#endif

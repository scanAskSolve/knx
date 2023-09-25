#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stddef.h>

#ifndef KNX_FLASH_SIZE
#define KNX_FLASH_SIZE 1024
#pragma warning "KNX_FLASH_SIZE not defined, using 1024"
#endif

enum NvMemoryType
{
    Eeprom,
    Flash
};

typedef struct Platform
{
  
     
    // ip config
    /* uint32_t currentIpAddress();
     uint32_t currentSubnetMask();
     uint32_t currentDefaultGateway();
     void macAddress(uint8_t* data);*/

    // unique serial number
     uint32_t uniqueSerialNumber();

    // basic stuff
     void restart();
     void fatalError();

    //multicast socket
     void setupMultiCast(uint32_t addr, uint16_t port);
     void closeMultiCast();
     bool sendBytesMultiCast(uint8_t* buffer, uint16_t len);
     int readBytesMultiCast(uint8_t* buffer, uint16_t maxLen);

    //unicast socket
     bool sendBytesUniCast(uint32_t addr, uint16_t port, uint8_t* buffer, uint16_t len);
    
    //UART
     void setupUart();
     void closeUart();
     int uartAvailable();
     size_t writeUart(const uint8_t data);
     size_t writeUart(const uint8_t* buffer, size_t size);
     int readUart();
     size_t readBytesUart(uint8_t* buffer, size_t length);

    // SPI
    /* void setupSpi();
     void closeSpi();
     int readWriteSpi(uint8_t *data, size_t len);*/

    //Memory

    // --- Overwrite these methods in the device-plattform to use the EEPROM Emulation API for UserMemory ----
    //
    // --- changes to the UserMemory are written directly into the address space starting at getEepromBuffer
    // --- commitToEeprom must save this to a non-volatile area if neccessary
     uint8_t* getEepromBuffer(uint32_t size);
     void commitToEeprom();
    // -------------------------------------------------------------------------------------------------------

     uint8_t* getNonVolatileMemoryStart();
     size_t getNonVolatileMemorySize();
     void commitNonVolatileMemory();
    // address is relative to start of nonvolatile memory
     uint32_t writeNonVolatileMemory(uint32_t relativeAddress, uint8_t* buffer, size_t size);
     uint32_t writeNonVolatileMemory(uint32_t relativeAddress, uint8_t value, size_t repeat);

    NvMemoryType NonVolatileMemoryType();
    void NonVolatileMemoryType(NvMemoryType type);

  // --- Overwrite these methods in the device-plattform to use flash memory handling by the knx stack ---
  // --- also set _memoryType;

  
    // size of one EraseBlock in pages
     size_t flashEraseBlockSize();
    // start of user flash aligned to start of an erase block
     uint8_t* userFlashStart();
    // size of the user flash in EraseBlocks
     size_t userFlashSizeEraseBlocks();
    //relativ to userFlashStart
     void flashErase(uint16_t eraseBlockNum);
    //write a single page to flash (pageNumber relative to userFashStart
     void flashWritePage(uint16_t pageNumber, uint8_t* data); 


  // -------------------------------------------------------------------------------------------------------

    
    NvMemoryType _memoryType;

    void loadEraseblockContaining(uint32_t relativeAddress);
    int32_t getEraseBlockNumberOf(uint32_t relativeAddress);
    // writes _eraseblockBuffer to flash
     void writeBufferedEraseBlock();
    // copies a EraseBlock into the _eraseblockBuffer
    void bufferEraseBlock(int32_t eraseBlockNumber);

    // in theory we would have to use this buffer for memory reads too,
    // but because ets always restarts the device after programming it
    // we can ignore this issue
    uint8_t* _eraseblockBuffer;
    int32_t _bufferedEraseblockNumber;
    bool _bufferedEraseblockDirty;
};
#endif // PLATFORM_H

 uint32_t currentIpAddress();

     uint32_t currentSubnetMask();

     uint32_t currentDefaultGateway();

     void macAddress(uint8_t* data);
 unique serial number
     uint32_t uniqueSerialNumber();
multicast socket
     void setupMultiCast(uint32_t addr, uint16_t port);

     void closeMultiCast();

     bool sendBytesMultiCast(uint8_t* buffer, uint16_t len);

     int readBytesMultiCast(uint8_t* buffer, uint16_t maxLen);
unicast socket
     bool sendBytesUniCast(uint32_t addr, uint16_t port, uint8_t* buffer, uint16_t len);
UART
     void setupUart();

     void closeUart();

     int uartAvailable();

     size_t writeUart(const uint8_t data);

     size_t writeUart(const uint8_t* buffer, size_t size);

     int readUart();

     size_t readBytesUart(uint8_t* buffer, size_t length);
 void setupSpi();

     void closeSpi();

     int readWriteSpi(uint8_t *data, size_t len);
volatile area if neccessary
     uint8_t* getEepromBuffer(uint32_t size);

     void commitToEeprom();


     uint8_t* getNonVolatileMemoryStart();

     size_t getNonVolatileMemorySize();

     void commitNonVolatileMemory();
 address is relative to start of nonvolatile memory
     uint32_t writeNonVolatileMemory(uint32_t relativeAddress, uint8_t* buffer, size_t size);

     uint32_t writeNonVolatileMemory(uint32_t relativeAddress, uint8_t value, size_t repeat);


    NvMemoryType NonVolatileMemoryType();

    void NonVolatileMemoryType(NvMemoryType type);
 size of one flash page in bytes
     size_t flashPageSize();
 size of one EraseBlock in pages
     size_t flashEraseBlockSize();
 start of user flash aligned to start of an erase block
     uint8_t* userFlashStart();
 size of the user flash in EraseBlocks
     size_t userFlashSizeEraseBlocks();
relativ to userFlashStart
     void flashErase(uint16_t eraseBlockNum);
write a single page to flash (pageNumber relative to userFashStart
     void flashWritePage(uint16_t pageNumber, uint8_t* data);
 Eeprom;


    void loadEraseblockContaining(uint32_t relativeAddress);

    int32_t getEraseBlockNumberOf(uint32_t relativeAddress);
 writes _eraseblockBuffer to flash
     void writeBufferedEraseBlock();
 copies a EraseBlock into the _eraseblockBuffer
    void bufferEraseBlock(int32_t eraseBlockNumber);
 nullptr;
 false;
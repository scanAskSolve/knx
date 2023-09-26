#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


#ifndef KNX_FLASH_SIZE
#define KNX_FLASH_SIZE 1024
#pragma warning "KNX_FLASH_SIZE not defined, using 1024"
#endif


typedef enum 
{
    Eeprom,
    Flash
}NvMemoryType;

typedef struct Platform
{
  
    // unique serial number
     uint32_t (*uniqueSerialNumber)();

    // basic stuff
     void (*restart)();
     void (*fatalError)();

    //multicast socket
     void (*setupMultiCast)(uint32_t addr, uint16_t port);
     void (*closeMultiCast)();
     bool (*sendBytesMultiCast)(uint8_t* buffer, uint16_t len);
     int (*readBytesMultiCast)(uint8_t* buffer, uint16_t maxLen);

    //unicast socket
     bool (*sendBytesUniCast)(uint32_t addr, uint16_t port, uint8_t* buffer, uint16_t len);
    
    //UART
     void (*setupUart)();
     void (*closeUart)();
     int (*uartAvailable)();
     size_t (*writeUart_data)(const uint8_t data);
     size_t (*writeUart_buffer)(const uint8_t* buffer, size_t size);
     int (*readUart)();
     size_t (*readBytesUart)(uint8_t* buffer, size_t length);

    // SPI
    /* void setupSpi();
     void closeSpi();
     int readWriteSpi(uint8_t *data, size_t len);*/

    //Memory

    // --- Overwrite these methods in the device-plattform to use the EEPROM Emulation API for UserMemory ----
    //
    // --- changes to the UserMemory are written directly into the address space starting at getEepromBuffer
    // --- commitToEeprom must save this to a non-volatile area if neccessary
     uint8_t* (*getEepromBuffer)(uint32_t size);
     void (*commitToEeprom)();
    // -------------------------------------------------------------------------------------------------------

     uint8_t* (*getNonVolatileMemoryStart)();
     size_t (*getNonVolatileMemorySize)();
     void (*commitNonVolatileMemory)();
    // address is relative to start of nonvolatile memory
     uint32_t (*writeNonVolatileMemory_Arrary)(uint32_t relativeAddress, uint8_t* buffer, size_t size);
     uint32_t (*writeNonVolatileMemory_value)(uint32_t relativeAddress, uint8_t value, size_t repeat);

    NvMemoryType (*Get_NonVolatileMemoryType)();
    void (*Set_NonVolatileMemoryType)(NvMemoryType type);

   // --- Overwrite these methods in the device-plattform to use flash memory handling by the knx stack ---
   // --- also set _memoryType = Flash in the device-plattform's contructor
   // --- optional: overwrite writeBufferedEraseBlock() in the device-plattform to reduce overhead when flashing multiple pages


  
    // size of one EraseBlock in pages
     size_t (*flashEraseBlockSize)();
    // start of user flash aligned to start of an erase block
     uint8_t* (*userFlashStart)();
    // size of the user flash in EraseBlocks
     size_t (*userFlashSizeEraseBlocks)();
    //relativ to userFlashStart
     void (*flashErase)(uint16_t eraseBlockNum);
    //write a single page to flash (pageNumber relative to userFashStart
     void (*flashWritePage)(uint16_t pageNumber, uint8_t* data); 


  // -------------------------------------------------------------------------------------------------------
	
    void (*loadEraseblockContaining)(uint32_t relativeAddress);
    uint32_t (*getEraseBlockNumberOf)(uint32_t relativeAddress);
    // writes _eraseblockBuffer to flash
     void (*writeBufferedEraseBlock)();
    // copies a EraseBlock into the _eraseblockBuffer
    void (*bufferEraseBlock)(int32_t eraseBlockNumber);


};
#endif // PLATFORM_H


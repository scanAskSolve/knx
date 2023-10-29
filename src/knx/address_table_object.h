#pragma once

// #include "table_object.h"
#include "interface_object.h"
#include "memory.h"

/**
 * This class represents the group address table. It provides a mapping between transport layer 
 * service access points (TSAP) and group addresses. The TSAP can be imagined as an index to the array 
 * of group addresses.
 * 
 * See section 4.10 of @cite knx:3/5/1 for further details.
 * It implements realisation type 7 (see section 4.10.7 of @cite knx:3/5/1). 
 */
// class Memory;

typedef void (*BeforeTablesUnloadCallback)();

class AddressTableObject : public InterfaceObject
{
  public:
    /**
     * The constructor.
     * 
     * @param memory This parameter is only passed to the constructor of TableObject and is not used by this class.
     */
    AddressTableObject(Memory& memory);
    const uint8_t* restore(const uint8_t* buffer);

    /**
     * returns the number of group addresses of the object.
     */
    uint16_t entryCount();
    /**
     * Get the group address mapped to a TSAP.
     * 
     * @param tsap The TSAP of which to get the group address for.
     * 
     * @return the groupAddress if found or zero if no group address was found.
     */
    uint16_t getGroupAddress(uint16_t tsap);
    /**
     * Get the TSAP mapped to a group address.
     * 
     * @param groupAddress the group address of which to get the TSAP for.
     * 
     * @return the TSAP if found or zero if no tsap was found.
     */
    uint16_t getTsap(uint16_t groupAddress);
    /**
     * Check if the address table contains a group address.
     * 
     * @param groupAddress the group address to check
     * 
     * @return true if the address table contains the group address, false otherwise
     */
    bool contains(uint16_t groupAddress);

  protected:
    void beforeStateChange(LoadState& newState);

  private:
    uint16_t* _groupAddresses = 0;

  public:
    LoadState loadState();
    void loadState(LoadState newState);
    uint8_t* save(uint8_t* buffer);
    // const uint8_t* restore(const uint8_t* buffer);
    uint16_t saveSize();

    static void beforeTablesUnloadCallback(BeforeTablesUnloadCallback func);
    static BeforeTablesUnloadCallback beforeTablesUnloadCallback();

    uint8_t* data();
    /**
     * Set the reason for a state change failure.
     */
    void errorCode(ErrorCode errorCode);

    // void initializeProperties(size_t propertiesSize, Property** properties) override;

    static BeforeTablesUnloadCallback _beforeTablesUnload;

    LoadState _state = LS_UNLOADED;
    Memory& _memory;
    uint8_t *_data = 0;
    static uint8_t _tableUnloadCount;

    uint32_t _size = 0;

    uint32_t tableReference();
    bool allocTable(uint32_t size, bool doFill, uint8_t fillByte);
    void loadEvent(const uint8_t* data);
    void loadEventUnloaded(const uint8_t* data);
    void loadEventLoading(const uint8_t* data);
    void loadEventLoaded(const uint8_t* data);
    void loadEventError(const uint8_t* data);
    void additionalLoadControls(const uint8_t* data);

};

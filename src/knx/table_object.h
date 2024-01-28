#pragma once

#include "interface_object.h"

class Memory;

typedef void (*BeforeTablesUnloadCallback)();

/**
 * This class provides common functionality for interface objects that are configured by ETS with MemorWrite.
 */
// class TableObject: public InterfaceObject
class TableObject
{
    //friend class Memory;

  public:
    /**
     * The constuctor.
     * @param memory The instance of the memory management class to use.
     */
    TableObject(Memory& memory);

    /**
     * The destructor.
     */
    virtual ~TableObject();
    /**
     * This method returns the ::LoadState of the interface object.
     */
    LoadState loadState();
    uint8_t* save(uint8_t* buffer) ;
    const uint8_t* restore(const uint8_t* buffer) ;
    uint16_t saveSize() ;

    static void beforeTablesUnloadCallback(BeforeTablesUnloadCallback func);
    static BeforeTablesUnloadCallback beforeTablesUnloadCallback();

  //protected:
    /**
     * This method is called before the interface object enters a new ::LoadState.
     * If there is a error changing the state newState should be set to ::LS_ERROR and errorCode() 
     * to a reason for the failure.
     */
    virtual void beforeStateChange(LoadState& newState);

    /**
     * returns the internal data of the interface object. This pointer belongs to the TableObject class and 
     * must not be freed.
     */
    uint8_t* data();
    /**
     * Set the reason for a state change failure.
     */
    void errorCode(ErrorCode errorCode);

    void initializeProperties(size_t propertiesSize, Property** properties) ;

    static BeforeTablesUnloadCallback _beforeTablesUnload;

  //private:
    uint32_t tableReference();
    bool allocTable(uint32_t size, bool doFill, uint8_t fillByte);
    void loadEvent(const uint8_t* data);
    void loadEventUnloaded(const uint8_t* data);
    void loadEventLoading(const uint8_t* data);
    void loadEventLoaded(const uint8_t* data);
    void loadEventError(const uint8_t* data);
    void additionalLoadControls(const uint8_t* data);
    /**
     * set the ::LoadState of the interface object.
     * 
     * Calls beforeStateChange().
     * 
     * @param newState the new ::LoadState 
     */
    void loadState(LoadState newState);
    LoadState _state = LS_UNLOADED;
    Memory& _memory;
    uint8_t *_data = 0;
    static uint8_t _tableUnloadCount;

    /**
     * used to store size of data() in allocTable(), needed for calculation of crc in PID_MCB_TABLE.
     * This value is also saved and restored.
     * The size of the memory block cannot be used because it is changed during alignment to page size.
     */
    uint32_t _size = 0;

  public:
    void readProperty(PropertyID id, uint16_t start, uint8_t& count, uint8_t* data);
    void writeProperty(PropertyID id, uint16_t start, uint8_t* data, uint8_t& count);
    uint8_t propertySize(PropertyID id);
    void command(PropertyID id, uint8_t* data, uint8_t length, uint8_t* resultData, uint8_t &resultLength);
    void state(PropertyID id, uint8_t* data, uint8_t length, uint8_t* resultData, uint8_t &resultLength);
    void readPropertyDescription(uint8_t& propertyId, uint8_t& propertyIndex, bool& writeEnable, uint8_t& type, uint16_t& numberOfElements, uint8_t& access);

    void masterReset(EraseCode eraseCode, uint8_t channel);
    Property *property(PropertyID id);
    const Property *property(PropertyID id) const;

    virtual uint8_t *interfaceObjectSave(uint8_t *buffer);
    virtual const uint8_t *interfaceObjectRestore(const uint8_t *buffer);
    virtual uint16_t interfaceObjectSaveSize();

    void interfaceObject_initializeProperties(size_t propertiesSize, Property **properties);

    Property **_properties = nullptr;
    uint8_t _propertyCount = 0;
};

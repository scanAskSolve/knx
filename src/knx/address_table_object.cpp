//#include <cstring>
#include "string.h"

#include "address_table_object.h"
#include "bits.h"
#include "data_property.h"
#include "callback_property.h"

using namespace std;

AddressTableObject::AddressTableObject(Memory& memory)
    : _memory(memory)
{
    Property* properties[] =
    {
        new DataProperty(PID_OBJECT_TYPE, false, PDT_UNSIGNED_INT, 1, ReadLv3 | WriteLv0, (uint16_t)OT_ADDR_TABLE)
    };

    Property* ownProperties[] =
    {
        new CallbackProperty<AddressTableObject>(this, PID_LOAD_STATE_CONTROL, true, PDT_CONTROL, 1, ReadLv3 | WriteLv3,
            [](AddressTableObject* obj, uint16_t start, uint8_t count, uint8_t* data) -> uint8_t {
                if(start == 0)
                {
                    uint16_t currentNoOfElements = 1;
                    pushWord(currentNoOfElements, data);
                    return 1;
                }

                data[0] = obj->_state;
                return 1;
            },
            [](AddressTableObject* obj, uint16_t start, uint8_t count, const uint8_t* data) -> uint8_t {
                obj->loadEvent(data);
                return 1;
            }),
        new CallbackProperty<AddressTableObject>(this, PID_TABLE_REFERENCE, false, PDT_UNSIGNED_LONG, 1, ReadLv3 | WriteLv0,
            [](AddressTableObject* obj, uint16_t start, uint8_t count, uint8_t* data) -> uint8_t {
                if(start == 0)
                {
                    uint16_t currentNoOfElements = 1;
                    pushWord(currentNoOfElements, data);
                    return 1;
                }

                if (obj->_state == LS_UNLOADED)
                    pushInt(0, data);
                else
                    pushInt(obj->tableReference(), data);
                return 1;
            }),
        new CallbackProperty<AddressTableObject>(this, PID_MCB_TABLE, false, PDT_GENERIC_08, 1, ReadLv3 | WriteLv0,
            [](AddressTableObject* obj, uint16_t start, uint8_t count, uint8_t* data) -> uint8_t {
                if (obj->_state != LS_LOADED)
                    return 0; // need to check return code for invalid
                
                uint32_t segmentSize = obj->_size;
                uint16_t crc16 = crc16Ccitt(obj->data(), segmentSize); 

                pushInt(segmentSize, data);     // Segment size
                pushByte(0x00, data + 4);       // CRC control byte -> 0: always valid
                pushByte(0xFF, data + 5);       // Read access 4 bits + Write access 4 bits
                pushWord(crc16, data + 6);      // CRC-16 CCITT of data
    
                return 1;
            }),
        new DataProperty(PID_ERROR_CODE, false, PDT_ENUM8, 1, ReadLv3 | WriteLv0, (uint8_t)E_NO_FAULT)
     };
    //TODO: missing

    //      23 PID_TABLE 3 / (3)

    uint8_t ownPropertiesCount = sizeof(ownProperties) / sizeof(Property*);

    uint8_t propertyCount = sizeof(properties) / sizeof(Property*);
    uint8_t allPropertiesCount = propertyCount + ownPropertiesCount;

    Property* allProperties[allPropertiesCount];
    memcpy(allProperties, properties, sizeof(properties));
    memcpy(allProperties + propertyCount, ownProperties, sizeof(ownProperties));

    InterfaceObject::initializeProperties(sizeof(allProperties), allProperties);
}

uint16_t AddressTableObject::entryCount()
{
    // after programming without GA the module hangs
    if (loadState() != LS_LOADED || _groupAddresses[0] == 0xFFFF)
        return 0;

    return ntohs(_groupAddresses[0]);
}

uint16_t AddressTableObject::getGroupAddress(uint16_t tsap)
{
    if (loadState() != LS_LOADED || tsap > entryCount() )
        return 0;

    return ntohs(_groupAddresses[tsap]);
}

uint16_t AddressTableObject::getTsap(uint16_t addr)
{
    uint16_t size = entryCount();
    #ifdef USE_BINSEARCH

    uint16_t low,high,i;
    low = 1;
    high = size;

    while(low <= high)
    {
        i = (low+high)/2;
        uint16_t ga = ntohs(_groupAddresses[i]);
        if (ga == addr)
            return i;
        if(addr < ga)
            high = i - 1;
        else
            low = i + 1 ;
    }
    #else
    for (uint16_t i = 1; i <= size; i++)
        if (ntohs(_groupAddresses[i]) == addr)
            return i;
    #endif
    return 0;
}

#pragma region SaveRestore

const uint8_t* AddressTableObject::restore(const uint8_t* buffer)
{
    uint8_t state = 0;
    buffer = popByte(state, buffer);
    _state = (LoadState)state;

    buffer = popInt(_size, buffer);

    uint32_t relativeAddress = 0;
    buffer = popInt(relativeAddress, buffer);

    if (relativeAddress != 0)
        _data = _memory.toAbsolute(relativeAddress);
    else
        _data = 0;

    _groupAddresses = (uint16_t*)data();

    return buffer;
}

#pragma endregion

bool AddressTableObject::contains(uint16_t addr)
{
    return (getTsap(addr) > 0);
}

uint8_t AddressTableObject::_tableUnloadCount = 0;

void AddressTableObject::beforeStateChange(LoadState& newState)
{
     if (newState == LS_LOADED && _tableUnloadCount > 0)
        _tableUnloadCount--;
    if (_tableUnloadCount > 0)
        return;
    if (newState == LS_UNLOADED) {
        _tableUnloadCount++;
        if (_beforeTablesUnload != 0)
            _beforeTablesUnload();
    }
    
    if (newState != LS_LOADED)
        return;

    _groupAddresses = (uint16_t*)data();
}
BeforeTablesUnloadCallback AddressTableObject::_beforeTablesUnload = 0;

void AddressTableObject::beforeTablesUnloadCallback(BeforeTablesUnloadCallback func)
{
    _beforeTablesUnload = func;
}

BeforeTablesUnloadCallback AddressTableObject::beforeTablesUnloadCallback()
{
    return _beforeTablesUnload;
}

LoadState AddressTableObject::loadState()
{
    return _state;
}

void AddressTableObject::loadState(LoadState newState)
{
    if (newState == _state)
        return;
    beforeStateChange(newState);
    _state = newState;
}


uint8_t* AddressTableObject::save(uint8_t* buffer)
{
    buffer = pushByte(_state, buffer);

    buffer = pushInt(_size, buffer);

    if (_data)
        buffer = pushInt(_memory.toRelative(_data), buffer);
    else
        buffer = pushInt(0, buffer);

    return buffer;
}


uint16_t AddressTableObject::saveSize()
{
    return 5 + InterfaceObject::saveSize() + sizeof(_size);
}

void AddressTableObject::additionalLoadControls(const uint8_t* data)
{
    if (data[1] != 0x0B) // Data Relative Allocation
    {
        loadState(LS_ERROR);
        errorCode(E_INVALID_OPCODE);
        return;
    }

    size_t size = ((data[2] << 24) | (data[3] << 16) | (data[4] << 8) | data[5]);
    bool doFill = data[6] == 0x1;
    uint8_t fillByte = data[7];
    if (!allocTable(size, doFill, fillByte))
    {
        loadState(LS_ERROR);
        errorCode(E_MAX_TABLE_LENGTH_EXEEDED);
    }
}

uint8_t* AddressTableObject::data()
{
    return _data;
}

void AddressTableObject::errorCode(ErrorCode errorCode)
{
    uint8_t data = errorCode;
    Property* prop = property(PID_ERROR_CODE);
    prop->write(data);
}

uint32_t AddressTableObject::tableReference()
{
    return (uint32_t)_memory.toRelative(_data);
}

bool AddressTableObject::allocTable(uint32_t size, bool doFill, uint8_t fillByte)
{
    if (_data)
    {
        _memory.freeMemory(_data);
        _data = 0;
    }

    if (size == 0)
        return true;

    _data = _memory.allocMemory(size);
    if (!_data)
        return false;

    if (doFill)
    {
        uint32_t addr = _memory.toRelative(_data);
        for(uint32_t i = 0; i < size;i++)
            _memory.writeMemory(addr+i, 1, &fillByte);
    }

    _size = size;

    return true;
}

void AddressTableObject::loadEvent(const uint8_t* data)
{
    switch (_state)
    {
        case LS_UNLOADED:
            loadEventUnloaded(data);
            break;
        case LS_LOADING:
            loadEventLoading(data);
            break;
        case LS_LOADED:
            loadEventLoaded(data);
            break;
        case LS_ERROR:
            loadEventError(data);
            break;
        default:
            /* do nothing */
            break;
    }
}

void AddressTableObject::loadEventUnloaded(const uint8_t* data)
{
    uint8_t event = data[0];
    switch (event)
    {
        case LE_NOOP:
        case LE_LOAD_COMPLETED:
        case LE_ADDITIONAL_LOAD_CONTROLS:
        case LE_UNLOAD:
            break;
        case LE_START_LOADING:
            loadState(LS_LOADING);
            break;
        default:
            loadState(LS_ERROR);
            errorCode(E_GOT_UNDEF_LOAD_CMD);
    }
}

void AddressTableObject::loadEventLoading(const uint8_t* data)
{
    uint8_t event = data[0];
    switch (event)
    {
        case LE_NOOP:
        case LE_START_LOADING:
            break;
        case LE_LOAD_COMPLETED:
            _memory.saveMemory();
            loadState(LS_LOADED);
            break;
        case LE_UNLOAD:
            loadState(LS_UNLOADED);
            break;
        case LE_ADDITIONAL_LOAD_CONTROLS:
            additionalLoadControls(data);
            break;
        default:
            loadState(LS_ERROR);
            errorCode(E_GOT_UNDEF_LOAD_CMD);
    }
}

void AddressTableObject::loadEventLoaded(const uint8_t* data)
{
    uint8_t event = data[0];
    switch (event)
    {
        case LE_NOOP:
        case LE_LOAD_COMPLETED:
            break;
        case LE_START_LOADING:
            loadState(LS_LOADING);
            break;
        case LE_UNLOAD:
            loadState(LS_UNLOADED);
            //free nv memory
            if (_data)
            {
                _memory.freeMemory(_data);
                _data = 0;
            }
            break;
        case LE_ADDITIONAL_LOAD_CONTROLS:
            loadState(LS_ERROR);
            errorCode(E_INVALID_OPCODE);
            break;
        default:
            loadState(LS_ERROR);
            errorCode(E_GOT_UNDEF_LOAD_CMD);
    }
}

void AddressTableObject::loadEventError(const uint8_t* data)
{
    uint8_t event = data[0];
    switch (event)
    {
        case LE_NOOP:
        case LE_LOAD_COMPLETED:
        case LE_ADDITIONAL_LOAD_CONTROLS:
        case LE_START_LOADING:
            break;
        case LE_UNLOAD:
            loadState(LS_UNLOADED);
            break;
        default:
            loadState(LS_ERROR);
            errorCode(E_GOT_UNDEF_LOAD_CMD);
    }
}

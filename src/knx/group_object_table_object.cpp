// #include <cstring>
#include "string.h"

#include "group_object_table_object.h"
#include "group_object.h"
#include "bits.h"
#include "property.h"

#include "memory.h"

GroupObjectTableObject::GroupObjectTableObject(Memory &memory) : _memory(memory)
{
    Property *properties[]{
        new Property(PID_OBJECT_TYPE, false, PDT_UNSIGNED_INT, 1, ReadLv3 | WriteLv0, (uint16_t)OT_GRP_OBJ_TABLE)};
    // todo TableObject::initializeProperties
    // TableObject::initializeProperties(sizeof(properties), properties);
}

GroupObjectTableObject::~GroupObjectTableObject()
{
    freeGroupObjects();
}

uint16_t GroupObjectTableObject::entryCount()
{
    if (loadState() != LS_LOADED)
        return 0;

    return ntohs(_tableData[0]);
}

GroupObject &GroupObjectTableObject::get(uint16_t asap)
{
    return _groupObjects[asap - 1];
}

const uint8_t *GroupObjectTableObject::restore(const uint8_t *buffer)
{

    buffer = tableObjectRestore(buffer);

    _tableData = (uint16_t *)data();
    initGroupObjects();

    return buffer;
}

GroupObject &GroupObjectTableObject::nextUpdatedObject(bool &valid)
{
    static uint16_t startIdx = 1;

    uint16_t objCount = entryCount();

    for (uint16_t asap = startIdx; asap <= objCount; asap++)
    {
        GroupObject &go = get(asap);

        if (go.commFlag() == Updated)
        {
            go.commFlag(Ok);
            startIdx = asap + 1;
            valid = true;
            return go;
        }
    }

    startIdx = 1;
    valid = false;
    return get(1);
}

void GroupObjectTableObject::groupObjects(GroupObject *objs, uint16_t size)
{
    freeGroupObjects();
    _groupObjects = objs;
    _groupObjectCount = size;
    initGroupObjects();
}

void GroupObjectTableObject::beforeStateChange(LoadState &newState)
{

    tableObjectBeforeStateChange(newState);
    if (newState != LS_LOADED)
        return;

    _tableData = (uint16_t *)data();

    if (!initGroupObjects())
    {
        newState = LS_ERROR;

        errorCode(E_SOFTWARE_FAULT);
    }
}

bool GroupObjectTableObject::initGroupObjects()
{
    if (!_tableData)
        return false;

    freeGroupObjects();

    uint16_t goCount = ntohs(_tableData[0]);

    _groupObjects = new GroupObject[goCount];
    _groupObjectCount = goCount;

    for (uint16_t asap = 1; asap <= goCount; asap++)
    {
        GroupObject &go = _groupObjects[asap - 1];
        go._asap = asap;
        go._table = this;

        go._dataLength = go.goSize();
        go._data = new uint8_t[go._dataLength];
        memset(go._data, 0, go._dataLength);

        if (go.valueReadOnInit())
            go.requestObjectRead();
    }

    return true;
}

void GroupObjectTableObject::freeGroupObjects()
{
    if (_groupObjects)
        delete[] _groupObjects;

    _groupObjectCount = 0;
    _groupObjects = 0;
}
void GroupObjectTableObject::readProperty(PropertyID id, uint16_t start, uint8_t &count, uint8_t *data)
{
    Property *prop = property(id);
    if (prop == nullptr)
    {
        count = 0;
        return;
    }

    count = prop->read(start, count, data);
}

void GroupObjectTableObject::writeProperty(PropertyID id, uint16_t start, uint8_t *data, uint8_t &count)
{
    Property *prop = property(id);
    if (prop == nullptr)
    {
        count = 0;
        return;
    }

    count = prop->write(start, count, data);
}
void GroupObjectTableObject::readPropertyDescription(uint8_t &propertyId, uint8_t &propertyIndex, bool &writeEnable, uint8_t &type, uint16_t &numberOfElements, uint8_t &access)
{
    uint8_t count = _propertyCount;

    numberOfElements = 0;
    if (_properties == nullptr || count == 0)
        return;

    Property *prop = nullptr;

    // from KNX spec. 03.03.07 Application Layer (page 56) - 3.4.3.3  A_PropertyDescription_Read-service
    // Summary: either propertyId OR propertyIndex, but not both at the same time
    if (propertyId != 0)
    {
        for (uint8_t i = 0; i < count; i++)
        {
            Property *p = _properties[i];
            if (p->Id() != propertyId)
                continue;

            prop = p;
            propertyIndex = i;
            break;
        }
    }
    else
    {
        // If propertyId is zero, propertyIndex shall be used.
        // Response: propertyIndex of received A_PropertyDescription_Read
        if (propertyIndex < count)
        {
            prop = _properties[propertyIndex];
        }
    }

    if (prop != nullptr)
    {
        propertyId = prop->Id();
        writeEnable = prop->WriteEnable();
        type = prop->Type();
        numberOfElements = prop->MaxElements();
        access = prop->Access();
    }
}
uint8_t GroupObjectTableObject::propertySize(PropertyID id)
{
    Property *prop = property(id);
    if (prop == nullptr)
    {
        return 0;
    }

    return prop->ElementSize();
}
void GroupObjectTableObject::command(PropertyID id, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength)
{
    Property *prop = property(id);
    if (prop == nullptr)
    {
        resultLength = 0;
        return;
        ;
    }

    prop->command(data, length, resultData, resultLength);
}

void GroupObjectTableObject::state(PropertyID id, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength)
{
    Property *prop = property(id);
    if (prop == nullptr)
    {
        resultLength = 0;
        return;
        ;
    }

    prop->state(data, length, resultData, resultLength);
}
Property *GroupObjectTableObject::property(PropertyID id)
{
    for (int i = 0; i < _propertyCount; i++)
        if (_properties[i]->Id() == id)
            return _properties[i];

    return nullptr;
}
void GroupObjectTableObject::masterReset(EraseCode eraseCode, uint8_t channel)
{
    // every interface object shall implement this
    // However, for the time being we provide an empty default implementation
}
uint8_t *GroupObjectTableObject::data()
{
    return _data;
}

LoadState GroupObjectTableObject::loadState()
{
    return _state;
}

uint8_t *GroupObjectTableObject::save(uint8_t *buffer)
{
    buffer = pushByte(_state, buffer);

    buffer = pushInt(_size, buffer);

    if (_data)
        buffer = pushInt(_memory.toRelative(_data), buffer);
    else
        buffer = pushInt(0, buffer);

    return buffer;
}

const uint8_t *GroupObjectTableObject::tableObjectRestore(const uint8_t *buffer)
{
    uint8_t state = 0;
    buffer = popByte(&state, buffer);
    _state = (LoadState)state;

    buffer = popInt(&_size, buffer);

    uint32_t relativeAddress = 0;
    buffer = popInt(&relativeAddress, buffer);

    if (relativeAddress != 0)
        _data = _memory.toAbsolute(relativeAddress);
    else
        _data = 0;

    return buffer;
}

uint16_t GroupObjectTableObject::saveSize()
{
    return 5 + interfaceSaveSize() + sizeof(_size);
}

uint16_t GroupObjectTableObject::interfaceSaveSize()
{
    uint16_t size = 0;

    for (int i = 0; i < _propertyCount; i++)
    {
        Property *prop = _properties[i];
        if (!prop->WriteEnable())
            continue;

        size += prop->saveSize();
    }
    return size;
}
void GroupObjectTableObject::tableObjectBeforeStateChange(LoadState &newState)
{
    if (newState == LS_LOADED && _tableUnloadCount > 0)
        _tableUnloadCount--;
    if (_tableUnloadCount > 0)
        return;
    if (newState == LS_UNLOADED)
    {
        _tableUnloadCount++;
    }
}
void GroupObjectTableObject::errorCode(ErrorCode errorCode)
{
    uint8_t data = errorCode;
    Property *prop = property(PID_ERROR_CODE);
    prop->write(data);
}

void GroupObjectTableObject::initializeProperties(size_t propertiesSize, Property **properties)
{
    Property *ownProperties[] =
        {
            new Property(
                this, PID_LOAD_STATE_CONTROL, true, PDT_CONTROL, 1, ReadLv3 | WriteLv3,
                [](GroupObjectTableObject *obj, uint16_t start, uint8_t count, uint8_t *data) -> uint8_t
                {
                    if (start == 0)
                    {
                        uint16_t currentNoOfElements = 1;
                        pushWord(currentNoOfElements, data);
                        return 1;
                    }

                    data[0] = obj->_state;
                    return 1;
                },
                [](GroupObjectTableObject *obj, uint16_t start, uint8_t count, const uint8_t *data) -> uint8_t
                {
                    obj->loadEvent(data);
                    return 1;
                }),
            new Property(this, PID_TABLE_REFERENCE, false, PDT_UNSIGNED_LONG, 1, ReadLv3 | WriteLv0,
                         [](GroupObjectTableObject *obj, uint16_t start, uint8_t count, uint8_t *data) -> uint8_t
                         {
                             if (start == 0)
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
            new Property(this, PID_MCB_TABLE, false, PDT_GENERIC_08, 1, ReadLv3 | WriteLv0,
                         [](GroupObjectTableObject *obj, uint16_t start, uint8_t count, uint8_t *data) -> uint8_t
                         {
                             if (obj->_state != LS_LOADED)
                                 return 0; // need to check return code for invalid

                             uint32_t segmentSize = obj->_size;
                             uint16_t crc16 = crc16Ccitt(obj->data(), segmentSize);

                             pushInt(segmentSize, data); // Segment size
                             pushByte(0x00, data + 4);   // CRC control byte -> 0: always valid
                             pushByte(0xFF, data + 5);   // Read access 4 bits + Write access 4 bits
                             pushWord(crc16, data + 6);  // CRC-16 CCITT of data

                             return 1;
                         }),
            new Property(PID_ERROR_CODE, false, PDT_ENUM8, 1, ReadLv3 | WriteLv0, (uint8_t)E_NO_FAULT)};
    // TODO: missing

    //      23 PID_TABLE 3 / (3)

    uint8_t ownPropertiesCount = sizeof(ownProperties) / sizeof(Property *);

    uint8_t propertyCount = propertiesSize / sizeof(Property *);
    uint8_t allPropertiesCount = propertyCount + ownPropertiesCount;

    Property *allProperties[allPropertiesCount];
    memcpy(allProperties, properties, propertiesSize);
    memcpy(allProperties + propertyCount, ownProperties, sizeof(ownProperties));

    interfaceInitializeProperties(sizeof(allProperties), allProperties);
}

uint32_t GroupObjectTableObject::tableReference()
{
    return (uint32_t)_memory.toRelative(_data);
}
void GroupObjectTableObject::interfaceInitializeProperties(size_t propertiesSize, Property **properties)
{
    _propertyCount = propertiesSize / sizeof(Property *);
    _properties = new Property *[_propertyCount];
    memcpy(_properties, properties, propertiesSize);
}
void GroupObjectTableObject::loadEvent(const uint8_t *data)
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
void GroupObjectTableObject::loadEventUnloaded(const uint8_t *data)
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

void GroupObjectTableObject::loadEventLoading(const uint8_t *data)
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

void GroupObjectTableObject::loadEventLoaded(const uint8_t *data)
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
        // free nv memory
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

void GroupObjectTableObject::loadEventError(const uint8_t *data)
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
void GroupObjectTableObject::loadState(LoadState newState)
{
    if (newState == _state)
        return;
    beforeStateChange(newState);
    _state = newState;
}
void GroupObjectTableObject::additionalLoadControls(const uint8_t *data)
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
bool GroupObjectTableObject::allocTable(uint32_t size, bool doFill, uint8_t fillByte)
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
        for (uint32_t i = 0; i < size; i++)
            _memory.writeMemory(addr + i, 1, &fillByte);
    }

    _size = size;

    return true;
}
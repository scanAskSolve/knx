// #include <cstring>
#include "string.h"

#include "address_table_object.h"
#include "bits.h"
#include "property.h"

using namespace std;

AddressTableObject::AddressTableObject(Memory &memory)
    : TableObject(memory)
{
    Property *properties[] =
        {
            new Property(PID_OBJECT_TYPE, false, PDT_UNSIGNED_INT, 1, ReadLv3 | WriteLv0, (uint16_t)OT_ADDR_TABLE)};

    TableObject::initializeProperties(sizeof(properties), properties);
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
    if (loadState() != LS_LOADED || tsap > entryCount())
        return 0;

    return ntohs(_groupAddresses[tsap]);
}

uint16_t AddressTableObject::getTsap(uint16_t addr)
{
    uint16_t size = entryCount();
#ifdef USE_BINSEARCH

    uint16_t low, high, i;
    low = 1;
    high = size;

    while (low <= high)
    {
        i = (low + high) / 2;
        uint16_t ga = ntohs(_groupAddresses[i]);
        if (ga == addr)
            return i;
        if (addr < ga)
            high = i - 1;
        else
            low = i + 1;
    }
#else
    for (uint16_t i = 1; i <= size; i++)
        if (ntohs(_groupAddresses[i]) == addr)
            return i;
#endif
    return 0;
}

#pragma region SaveRestore

const uint8_t *AddressTableObject::restore(const uint8_t *buffer)
{
    buffer = TableObject::restore(buffer);

    _groupAddresses = (uint16_t *)data();

    return buffer;
}

#pragma endregion

bool AddressTableObject::contains(uint16_t addr)
{
    return (getTsap(addr) > 0);
}

void AddressTableObject::beforeStateChange(LoadState &newState)
{
    TableObject::beforeStateChange(newState);
    if (newState != LS_LOADED)
        return;

    _groupAddresses = (uint16_t *)data();
}
void AddressTableObject::readProperty(PropertyID id, uint16_t start, uint8_t &count, uint8_t *data)
{
    Property *prop = property(id);
    if (prop == nullptr)
    {
        count = 0;
        return;
    }

    count = prop->read(start, count, data);
}

void AddressTableObject::writeProperty(PropertyID id, uint16_t start, uint8_t *data, uint8_t &count)
{
    Property *prop = property(id);
    if (prop == nullptr)
    {
        count = 0;
        return;
    }

    count = prop->write(start, count, data);
}
void AddressTableObject::readPropertyDescription(uint8_t& propertyId, uint8_t& propertyIndex, bool& writeEnable, uint8_t& type, uint16_t& numberOfElements, uint8_t& access)
{
    uint8_t count = _propertyCount;

    numberOfElements = 0;
    if (_properties == nullptr || count == 0)
        return;

    Property* prop = nullptr;

    // from KNX spec. 03.03.07 Application Layer (page 56) - 3.4.3.3  A_PropertyDescription_Read-service
    // Summary: either propertyId OR propertyIndex, but not both at the same time
    if (propertyId != 0)
    {
        for (uint8_t i = 0; i < count; i++)
        {
            Property* p = _properties[i];
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
// #include <cstring>
#include "string.h"

#include "device_object.h"
#include "bits.h"
#include "property.h"
#include "config.h"

#define LEN_KNX_SERIAL 6

DeviceObject::DeviceObject()
{
    uint8_t serialNumber[] = {0x00, 0xFA, 0x01, 0x02, 0x03, 0x04};
    uint8_t hardwareType[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    Property *properties[] =
        {
            new Property(PID_OBJECT_TYPE, false, PDT_UNSIGNED_INT, 1, ReadLv3 | WriteLv0, (uint16_t)OT_DEVICE),
            new Property(PID_SERIAL_NUMBER, false, PDT_GENERIC_06, 1, ReadLv3 | WriteLv0, serialNumber),
            new Property(this, PID_MANUFACTURER_ID, false, PDT_UNSIGNED_INT, 1, ReadLv3 | WriteLv0,
                         [](DeviceObject *io, uint16_t start, uint8_t count, uint8_t *data) -> uint8_t
                         {
                             if (start == 0)
                             {
                                 uint16_t currentNoOfElements = 1;
                                 pushWord(currentNoOfElements, data);
                                 return 1;
                             }

                             pushByteArray(io->propertyData(PID_SERIAL_NUMBER), 2, data);
                             return 1;
                         }),
            new Property(PID_DEVICE_CONTROL, true, PDT_BITSET8, 1, ReadLv3 | WriteLv3, (uint8_t)0),
            new Property(PID_ORDER_INFO, false, PDT_GENERIC_10, 1, ReadLv3 | WriteLv0),
            new Property(PID_VERSION, false, PDT_VERSION, 1, ReadLv3 | WriteLv0, (uint16_t)3),
            new Property(PID_ROUTING_COUNT, true, PDT_UNSIGNED_CHAR, 1, ReadLv3 | WriteLv3, (uint8_t)(6 << 4)),
            new Property(
                this, PID_PROG_MODE, true, PDT_BITSET8, 1, ReadLv3 | WriteLv3,
                [](DeviceObject *io, uint16_t start, uint8_t count, uint8_t *data) -> uint8_t
                {
                    if (start == 0)
                    {
                        uint16_t currentNoOfElements = 1;
                        pushWord(currentNoOfElements, data);
                        return 1;
                    }

                    *data = io->_prgMode;
                    return 1;
                },
                [](DeviceObject *io, uint16_t start, uint8_t count, const uint8_t *data) -> uint8_t
                {
                    if (start == 0)
                        return 1;

                    io->_prgMode = *data;
                    return 1;
                }),
            new Property(PID_MAX_APDU_LENGTH, false, PDT_UNSIGNED_INT, 1, ReadLv3 | WriteLv0, (uint16_t)254),
            new Property(this, PID_SUBNET_ADDR, false, PDT_UNSIGNED_CHAR, 1, ReadLv3 | WriteLv0,
                         [](DeviceObject *io, uint16_t start, uint8_t count, uint8_t *data) -> uint8_t
                         {
                             if (start == 0)
                             {
                                 uint16_t currentNoOfElements = 1;
                                 pushWord(currentNoOfElements, data);
                                 return 1;
                             }

                             *data = ((io->_ownAddress >> 8) & 0xff);

                             return 1;
                         }),
            new Property(this, PID_DEVICE_ADDR, false, PDT_UNSIGNED_CHAR, 1, ReadLv3 | WriteLv0,
                         [](DeviceObject *io, uint16_t start, uint8_t count, uint8_t *data) -> uint8_t
                         {
                             if (start == 0)
                             {
                                 uint16_t currentNoOfElements = 1;
                                 pushWord(currentNoOfElements, data);
                                 return 1;
                             }

                             *data = (io->_ownAddress & 0xff);
                             return 1;
                         }),
            new Property(PID_IO_LIST, false, PDT_UNSIGNED_INT, 8, ReadLv3 | WriteLv0),
            new Property(PID_HARDWARE_TYPE, true, PDT_GENERIC_06, 1, ReadLv3 | WriteLv3, hardwareType),
            new Property(PID_DEVICE_DESCRIPTOR, false, PDT_GENERIC_02, 1, ReadLv3 | WriteLv0),
#ifdef USE_RF
            new Property(PID_RF_DOMAIN_ADDRESS_CEMI_SERVER, true, PDT_GENERIC_06, 1, ReadLv3 | WriteLv3),
#endif
        };
    _propertyCount =  sizeof(properties)/ sizeof(Property*);
    _properties = new Property*[_propertyCount];
    memcpy(_properties, properties, sizeof(properties));
}

uint8_t *DeviceObject::save(uint8_t *buffer)
{
    buffer = pushWord(_ownAddress, buffer);
    for (int i = 0; i < _propertyCount; i++)
    {
        Property* prop = _properties[i];
        if (!prop->WriteEnable())
            continue;
        
        buffer = prop->save(buffer);
    }
    return buffer;
}

const uint8_t *DeviceObject::restore(const uint8_t *buffer)
{
    buffer = popWord(_ownAddress, buffer);
    for (int i = 0; i < _propertyCount; i++)
    {
        Property* prop = _properties[i];
        if (!prop->WriteEnable())
            continue;

        buffer = prop->restore(buffer);
    }
    return buffer;
}

uint16_t DeviceObject::saveSize()
{
    uint16_t size = 0;

    for (int i = 0; i < _propertyCount; i++)
    {
        Property* prop = _properties[i];
        if (!prop->WriteEnable())
            continue;

        size += prop->saveSize();
    }
    return 2 + size;
}

uint16_t DeviceObject::individualAddress()
{
    return _ownAddress;
}

void DeviceObject::individualAddress(uint16_t value)
{
    _ownAddress = value;
}

#define USER_STOPPED 0x1
#define OWN_ADDR_DUPL 0x2
#define VERIFY_MODE 0x4
#define SAFE_STATE 0x8

void DeviceObject::individualAddressDuplication(bool value)
{
    Property *prop = property(PID_DEVICE_CONTROL);
    uint8_t data;
    prop->read(data);

    if (value)
        data |= OWN_ADDR_DUPL;
    else
        data &= ~OWN_ADDR_DUPL;
    prop->write(data);
}

bool DeviceObject::verifyMode()
{
    Property *prop = property(PID_DEVICE_CONTROL);
    uint8_t data;
    prop->read(data);
    return (data & VERIFY_MODE) > 0;
}

void DeviceObject::verifyMode(bool value)
{
    Property *prop = property(PID_DEVICE_CONTROL);
    uint8_t data;
    prop->read(data);

    if (value)
        data |= VERIFY_MODE;
    else
        data &= ~VERIFY_MODE;
    prop->write(data);
}

bool DeviceObject::progMode()
{
    return _prgMode == 1;
}

void DeviceObject::progMode(bool value)
{
    if (value)
        _prgMode = 1;
    else
        _prgMode = 0;
}

uint16_t DeviceObject::manufacturerId()
{
    uint16_t manufacturerId;
    popWord(manufacturerId, propertyData(PID_SERIAL_NUMBER));
    return manufacturerId;
}

void DeviceObject::manufacturerId(uint16_t value)
{
    uint8_t data[LEN_KNX_SERIAL];
    memcpy(data, propertyData(PID_SERIAL_NUMBER), LEN_KNX_SERIAL);
    pushWord(value, data);
    propertyValue(PID_SERIAL_NUMBER, data);
}

uint32_t DeviceObject::bauNumber()
{
    uint32_t bauNumber;
    popInt(bauNumber, propertyData(PID_SERIAL_NUMBER) + 2);
    return bauNumber;
}

void DeviceObject::bauNumber(uint32_t value)
{
    uint8_t data[LEN_KNX_SERIAL];
    memcpy(data, propertyData(PID_SERIAL_NUMBER), LEN_KNX_SERIAL);
    pushInt(value, data + 2);
    propertyValue(PID_SERIAL_NUMBER, data);
}

const uint8_t *DeviceObject::orderNumber()
{
    Property *prop = property(PID_ORDER_INFO);
    return prop->data();
}

void DeviceObject::orderNumber(const uint8_t *value)
{
    Property *prop = property(PID_ORDER_INFO);
    prop->write(value);
}

const uint8_t *DeviceObject::hardwareType()
{
    Property *prop = property(PID_HARDWARE_TYPE);
    return prop->data();
}

void DeviceObject::hardwareType(const uint8_t *value)
{
    Property *prop = property(PID_HARDWARE_TYPE);
    prop->write(value);
}

uint16_t DeviceObject::version()
{
    Property *prop = property(PID_VERSION);
    uint16_t value;
    prop->read(value);
    return value;
}

void DeviceObject::version(uint16_t value)
{
    Property *prop = property(PID_VERSION);
    prop->write(value);
}

uint16_t DeviceObject::maskVersion()
{
    Property *prop = property(PID_DEVICE_DESCRIPTOR);
    uint16_t value;
    prop->read(value);
    return value;
}

void DeviceObject::maskVersion(uint16_t value)
{
    Property *prop = property(PID_DEVICE_DESCRIPTOR);
    prop->write(value);
}

uint16_t DeviceObject::maxApduLength()
{
    Property *prop = property(PID_MAX_APDU_LENGTH);
    uint16_t value;
    prop->read(value);
    return value;
}

void DeviceObject::maxApduLength(uint16_t value)
{
    Property *prop = property(PID_MAX_APDU_LENGTH);
    prop->write(value);
}

const uint8_t *DeviceObject::rfDomainAddress()
{
    Property *prop = property(PID_RF_DOMAIN_ADDRESS_CEMI_SERVER);
    return prop->data();
}

void DeviceObject::rfDomainAddress(uint8_t *value)
{
    Property *prop = property(PID_RF_DOMAIN_ADDRESS_CEMI_SERVER);
    prop->write(value);
}

uint8_t DeviceObject::defaultHopCount()
{
    Property *prop = property(PID_ROUTING_COUNT);
    uint8_t value;
    prop->read(value);
    return (value >> 4) & 0x07;
}

void DeviceObject::propertyValue(PropertyID id, uint8_t *value)
{
    Property *prop = property(id);
    prop->write(value);
}
const uint8_t *DeviceObject::propertyData(PropertyID id)
{
    Property *prop = property(id);
    return prop->data();
}

const uint8_t *DeviceObject::propertyData(PropertyID id, uint16_t elementIndex)
{
    Property *prop = property(id);
    return prop->data(elementIndex);
}
void DeviceObject::readProperty(PropertyID id, uint16_t start, uint8_t &count, uint8_t *data)
{
    Property *prop = property(id);
    if (prop == nullptr)
    {
        count = 0;
        return;
    }

    count = prop->read(start, count, data);
}

void DeviceObject::writeProperty(PropertyID id, uint16_t start, uint8_t *data, uint8_t &count)
{
    Property *prop = property(id);
    if (prop == nullptr)
    {
        count = 0;
        return;
    }

    count = prop->write(start, count, data);
}
void DeviceObject::readPropertyDescription(uint8_t& propertyId, uint8_t& propertyIndex, bool& writeEnable, uint8_t& type, uint16_t& numberOfElements, uint8_t& access)
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
uint8_t DeviceObject::propertySize(PropertyID id)
{
    Property* prop = property(id);
    if (prop == nullptr)
    {
        return 0;
    }

    return prop->ElementSize();
}
void DeviceObject::command(PropertyID id, uint8_t* data, uint8_t length, uint8_t* resultData, uint8_t& resultLength)
{
    Property* prop = property(id);
    if (prop == nullptr)
    {
        resultLength = 0;
        return;;
    }

    prop->command(data, length, resultData, resultLength);
}

void DeviceObject::state(PropertyID id, uint8_t* data, uint8_t length, uint8_t* resultData, uint8_t& resultLength)
{
    Property* prop = property(id);
    if (prop == nullptr)
    {
        resultLength = 0;
        return;;
    }

    prop->state(data, length, resultData, resultLength);
}
Property* DeviceObject::property(PropertyID id)
{
    for (int i = 0; i < _propertyCount; i++)
        if (_properties[i]->Id() == id)
            return _properties[i];

    return nullptr;
}
void DeviceObject::masterReset(EraseCode eraseCode, uint8_t channel)
{
    // every interface object shall implement this
    // However, for the time being we provide an empty default implementation
}
void DeviceObject::initializeProperties(size_t propertiesSize, Property** properties)
{
    _propertyCount = propertiesSize / sizeof(Property*);
    _properties = new Property*[_propertyCount];
    memcpy(_properties, properties, propertiesSize);
}
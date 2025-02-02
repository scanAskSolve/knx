#include "application_program_object.h"
#include "bits.h"
#include "property.h"
#include "dptconvert.h"
// #include <cstring>
#include "string.h"

ApplicationProgramObject::ApplicationProgramObject(Memory &memory)
    : TableObject(memory)
{
    Property *properties[] =
        {
            new Property(PID_OBJECT_TYPE, false, PDT_UNSIGNED_INT, 1, ReadLv3 | WriteLv0, (uint16_t)OT_APPLICATION_PROG),
            new Property(PID_PROG_VERSION, true, PDT_GENERIC_05, 1, ReadLv3 | WriteLv3),
            new Property(this, PID_PEI_TYPE, false, PDT_UNSIGNED_CHAR, 1, ReadLv3 | WriteLv0,
                         [](ApplicationProgramObject *io, uint16_t start, uint8_t count, uint8_t *data) -> uint8_t
                         {
                             if (start == 0)
                             {
                                 uint16_t currentNoOfElements = 1;
                                 pushWord(currentNoOfElements, data);
                                 return 1;
                             }

                             data[0] = 0;
                             return 1;
                         })};

    TableObject::initializeProperties(sizeof(properties), properties);
}

uint8_t *ApplicationProgramObject::save(uint8_t *buffer)
{
    uint8_t programVersion[5];
    property(PID_PROG_VERSION)->read(programVersion);
    buffer = pushByteArray(programVersion, 5, buffer);

    return TableObject::save(buffer);
}

const uint8_t *ApplicationProgramObject::restore(const uint8_t *buffer)
{
    uint8_t programVersion[5];
    buffer = popByteArray(programVersion, 5, buffer);
    property(PID_PROG_VERSION)->write(programVersion);

    return TableObject::restore(buffer);
}

uint16_t ApplicationProgramObject::saveSize()
{
    return TableObject::saveSize() + 5; // sizeof(programVersion)
}

uint8_t *ApplicationProgramObject::data(uint32_t addr)
{
    return TableObject::data() + addr;
}

uint8_t ApplicationProgramObject::getByte(uint32_t addr)
{
    return *(TableObject::data() + addr);
}

uint16_t ApplicationProgramObject::getWord(uint32_t addr)
{
    return ::getWord(TableObject::data() + addr);
}

uint32_t ApplicationProgramObject::getInt(uint32_t addr)
{
    return ::getInt(TableObject::data() + addr);
}

double ApplicationProgramObject::getFloat(uint32_t addr, ParameterFloatEncodings encoding)
{
    switch (encoding)
    {
    case Float_Enc_DPT9:
        return float16FromPayload(TableObject::data() + addr, 0);
        break;
    case Float_Enc_IEEE754Single:
        return float32FromPayload(TableObject::data() + addr, 0);
        break;
    case Float_Enc_IEEE754Double:
        return float64FromPayload(TableObject::data() + addr, 0);
        break;
    default:
        return 0;
        break;
    }
}
void ApplicationProgramObject::readProperty(PropertyID id, uint16_t start, uint8_t &count, uint8_t *data)
{
    Property *prop = property(id);
    if (prop == nullptr)
    {
        count = 0;
        return;
    }

    count = prop->read(start, count, data);
}

void ApplicationProgramObject::writeProperty(PropertyID id, uint16_t start, uint8_t *data, uint8_t &count)
{
    Property *prop = property(id);
    if (prop == nullptr)
    {
        count = 0;
        return;
    }

    count = prop->write(start, count, data);
}
void ApplicationProgramObject::readPropertyDescription(uint8_t &propertyId, uint8_t &propertyIndex, bool &writeEnable, uint8_t &type, uint16_t &numberOfElements, uint8_t &access)
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
uint8_t ApplicationProgramObject::propertySize(PropertyID id)
{
    Property *prop = property(id);
    if (prop == nullptr)
    {
        return 0;
    }

    return prop->ElementSize();
}
void ApplicationProgramObject::command(PropertyID id, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength)
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

void ApplicationProgramObject::state(PropertyID id, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength)
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
Property *ApplicationProgramObject::property(PropertyID id)
{
    for (int i = 0; i < _propertyCount; i++)
        if (_properties[i]->Id() == id)
            return _properties[i];

    return nullptr;
}
void ApplicationProgramObject::masterReset(EraseCode eraseCode, uint8_t channel)
{
    // every interface object shall implement this
    // However, for the time being we provide an empty default implementation
}
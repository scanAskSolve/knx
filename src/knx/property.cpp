#include "property.h"
#include "bits.h"

//#include <cstring>
#include "string.h"


PropertyID Property::Id() const
{
    return _id;
}

bool Property::WriteEnable() const
{
    return _writeEnable;
}

PropertyDataType Property::Type() const
{
    return _type;
}

uint16_t Property::MaxElements() const
{
    return _maxElements;
}

uint8_t Property::Access() const
{
    return _access;
}

uint8_t Property::ElementSize() const
{
    switch (_type)
    {
        case PDT_CHAR:
        case PDT_CONTROL: // is actually 10 if written, but this is always handled with a callback
        case PDT_GENERIC_01:
        case PDT_UNSIGNED_CHAR:
        case PDT_BITSET8:
        case PDT_BINARY_INFORMATION: // only 1 bit really
        case PDT_ENUM8:
        case PDT_SCALING:
            return 1;
        case PDT_GENERIC_02:
        case PDT_INT:
        case PDT_KNX_FLOAT:
        case PDT_UNSIGNED_INT:
        case PDT_VERSION:
        case PDT_BITSET16:
            return 2;
        case PDT_DATE:
        case PDT_ESCAPE:
        case PDT_FUNCTION:
        case PDT_GENERIC_03:
        case PDT_NE_FL:
        case PDT_NE_VL:
        case PDT_POLL_GROUP_SETTING:
        case PDT_TIME:
        case PDT_UTF8:
            return 3;
        case PDT_FLOAT:
        case PDT_GENERIC_04:
        case PDT_LONG:
        case PDT_UNSIGNED_LONG:
            return 4;
        case PDT_GENERIC_05:
        case PDT_SHORT_CHAR_BLOCK:
            return 5;
        case PDT_GENERIC_06:
        case PDT_ALARM_INFO:
            return 6;
        case PDT_GENERIC_07:
            return 7;
        case PDT_DATE_TIME:
        case PDT_DOUBLE:
        case PDT_GENERIC_08:
            return 8;
        case PDT_GENERIC_09:
            return 9;
        case PDT_CHAR_BLOCK:
        case PDT_GENERIC_10:
            return 10;
        case PDT_GENERIC_11:
            return 11;
        case PDT_GENERIC_12:
            return 12;
        case PDT_GENERIC_13:
            return 13;
        case PDT_GENERIC_14:
            return 14;
        case PDT_GENERIC_15:
            return 15;
        case PDT_GENERIC_16:
            return 16;
        case PDT_GENERIC_17:
            return 17;
        case PDT_GENERIC_18:
            return 18;
        case PDT_GENERIC_19:
            return 19;
        case PDT_GENERIC_20:
            return 20;
        default:
            return 0;
    }
}

Property::Property(PropertyID id, bool writeEnable, PropertyDataType type,
                   uint16_t maxElements, uint8_t access)
    : _id(id), _writeEnable(writeEnable), _type(type), _maxElements(maxElements), _access(access)
{}
/*

Property::Property(PropertyID id, bool writeEnable, PropertyDataType type,
                           uint16_t maxElements, uint8_t access, uint16_t value)
    : _id(id), _writeEnable(writeEnable), _type(type), _maxElements(maxElements), _access(access)
{
	write(value);
}

Property::Property(PropertyID id, bool writeEnable, PropertyDataType type, 
                           uint16_t maxElements, uint8_t access, uint32_t value)
    : _id(id), _writeEnable(writeEnable), _type(type), _maxElements(maxElements), _access(access)
{
	write(value);
}

Property::Property(PropertyID id, bool writeEnable, PropertyDataType type,
                           uint16_t maxElements, uint8_t access, uint8_t value)
    : _id(id), _writeEnable(writeEnable), _type(type), _maxElements(maxElements), _access(access)
{
	write(value);
}

Property::Property(PropertyID id, bool writeEnable, PropertyDataType type,
                           uint16_t maxElements, uint8_t access, const uint8_t* value)
    : _id(id), _writeEnable(writeEnable), _type(type), _maxElements(maxElements), _access(access)
{	
	write(value);
}
*/

Property::Property(PropertyID id, bool writeEnable, PropertyDataType type,
                           uint16_t maxElements, uint8_t access, uint16_t value)
    : Property(id, writeEnable, type, maxElements, access)
{
    Property::write(value);
}

Property::Property(PropertyID id, bool writeEnable, PropertyDataType type, 
                           uint16_t maxElements, uint8_t access, uint32_t value)
    : Property(id, writeEnable, type, maxElements, access)
{
    Property::write(value);
}

Property::Property(PropertyID id, bool writeEnable, PropertyDataType type,
                           uint16_t maxElements, uint8_t access, uint8_t value)
    : Property(id, writeEnable, type, maxElements, access)
{
    Property::write(value);
}

Property::Property(PropertyID id, bool writeEnable, PropertyDataType type,
                           uint16_t maxElements, uint8_t access, const uint8_t* value)
    : Property(id, writeEnable, type, maxElements, access)
{
    Property::write(value);
}

Property::~Property()
{
	    if (_data)
        delete[] _data;
}


uint8_t Property::read(uint8_t& value) const
{
    if (ElementSize() != 1)
        return 0;
    
    return read(1, 1, &value);
}


uint8_t Property::read(uint16_t& value) const
{
    if (ElementSize() != 2)
        return 0;

    uint8_t data[2];
    uint8_t count = read(1, 1, data);
    if (count > 0)
    {
        popWord(value, data);
    }
    return count;
}


uint8_t Property::read(uint32_t& value) const
{
    if (ElementSize() != 4)
        return 0;

    uint8_t data[4];
    uint8_t count = read(1, 1, data);
    if (count > 0)
    {
        popInt(value, data);
    }
    return count;
}

uint8_t Property::read(uint8_t* value) const
{
    return read(1, 1, value);
}

uint8_t Property::write(uint8_t value)
{
    if (ElementSize() != 1)
        return 0;

    return write(1, 1, &value);
}


uint8_t Property::write(uint16_t value)
{
    if (ElementSize() != 2)
        return 0;

    uint8_t data[2];
    pushWord(value, data);
    return write(1, 1, data);
}


uint8_t Property::write(uint32_t value)
{
    if (ElementSize() != 4)
        return 0;

    uint8_t data[4];
    pushInt(value, data);
    return write(1, 1, data);
}


uint8_t Property::write(const uint8_t* value)
{
    return write(1, 1, value);
}


uint8_t Property::write(uint16_t position, uint16_t value)
{
    if (ElementSize() != 2)
        return 0;

    uint8_t data[2];
    pushWord(value, data);
    return write(position, 1, data);
}

void Property::command(uint8_t* data, uint8_t length, uint8_t* resultData, uint8_t& resultLength)
{
    (void)data;
    (void)length;
    (void)resultData;
    resultLength = 0;
}

void Property::state(uint8_t* data, uint8_t length, uint8_t* resultData, uint8_t &resultLength)
{
    (void)data;
    (void)length;
    (void)resultData;
    resultLength = 0;
}

/*
uint8_t* Property::save(uint8_t* buffer)
{
	return buffer;
}
const uint8_t* Property::restore(const uint8_t* buffer)
{
	return buffer;
}


uint16_t Property::saveSize()
{
	return 0;
}
*/

//-----------------------------------------------------------------------

uint8_t Property::read(uint16_t start, uint8_t count, uint8_t* data) const 
{
    if (start == 0)
    {
        pushWord(_currentElements, data);
        return 1;
    }

    if (count == 0 || _currentElements == 0 || start > _currentElements || count > _currentElements - start + 1)
        return 0;


    // we start counting with zero
    start -= 1;

    // data is already big enough to hold the data
    memcpy(data, _data + (start * ElementSize()), count * ElementSize()); 

    return count;
}

uint8_t Property::write(uint16_t start, uint8_t count, const uint8_t* data)
{
    if (count == 0 || start > _maxElements || start + count > _maxElements + 1)
        return 0;

    if (start == 0)
    {
        if (count == 1 && data[0] == 0 && data[1] == 0)
        {
            // reset _data
            _currentElements = 0;
            if (_data)
            {
                delete[] _data;
                _data = nullptr;
            }
            return 1;
        }
        else
            return 0;
    }

    // we start counting with zero
    start -= 1;
    if (start + count > _currentElements)
    {
        // reallocate memory for _data
        uint8_t* oldData = _data;
        size_t oldDataSize = _currentElements * ElementSize();

        size_t newDataSize = (start + count) * ElementSize();
        _data = new uint8_t[newDataSize];
        memset(_data, 0, newDataSize);

        if (oldData != nullptr)
        {
            memcpy(_data, oldData, oldDataSize);
            delete[] oldData;
        }

        _currentElements = start + count;
    }

    memcpy(_data + (start * ElementSize()), data, count * ElementSize());

    return count;
}

/*
Property* Property::DataProperty(PropertyID id, bool writeEnable, PropertyDataType type,
                           uint16_t maxElements, uint8_t access)
{
	Property* prop = new Property(id, writeEnable, type, maxElements, access);
	return prop;
}
	

Property* Property::DataProperty(PropertyID id, bool writeEnable, PropertyDataType type,
                           uint16_t maxElements, uint8_t access, uint16_t value)
{
	Property* prop = new Property(id, writeEnable, type, maxElements, access);
	prop->write(value);
	return prop;
}

Property* Property::DataProperty(PropertyID id, bool writeEnable, PropertyDataType type, 
                           uint16_t maxElements, uint8_t access, uint32_t value)
{
	Property* prop = new Property(id, writeEnable, type, maxElements, access);
	prop->write(value);
	return prop;

}

Property* Property::DataProperty(PropertyID id, bool writeEnable, PropertyDataType type,
                           uint16_t maxElements, uint8_t access, uint8_t value)
{
	Property* prop = new Property(id, writeEnable, type, maxElements, access);
	prop->write(value);
	return prop;

}

Property* Property::DataProperty(PropertyID id, bool writeEnable, PropertyDataType type,
                           uint16_t maxElements, uint8_t access, const uint8_t* value)
{	
	Property* prop = new Property(id, writeEnable, type, maxElements, access);
	prop->write(value);
	return prop;
}
*/
uint16_t Property::saveSize()
{
    return sizeof(_currentElements) + _maxElements * ElementSize();
}


const uint8_t* Property::restore(const uint8_t* buffer)
{
    uint16_t elements = 0;
    buffer = popWord(elements, buffer);

    if (elements != _currentElements)
    {
        if (_data != nullptr)
            delete[] _data;
        
        _data = new uint8_t[elements * ElementSize()];
        _currentElements = elements;
    }

    if (elements > 0)
        buffer = popByteArray(_data, elements * ElementSize(), buffer);

    return buffer;
}


uint8_t* Property::save(uint8_t* buffer)
{
    buffer = pushWord(_currentElements, buffer);
    if (_currentElements > 0)
        buffer = pushByteArray(_data, _currentElements * ElementSize(), buffer);

    return buffer;
}
const uint8_t* Property::data()
{
    return _data;
}

const uint8_t* Property::data(uint16_t elementIndex)
{
    if ((elementIndex == 0) || (elementIndex > _currentElements))
        return nullptr;

    elementIndex -= 1; // Starting from 0
    uint16_t offset = elementIndex * ElementSize();
    return _data + offset;
}




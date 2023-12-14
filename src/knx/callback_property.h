#pragma once

#include "property.h"

class InterfaceObject;
class ApplicationProgramObject;
class DeviceObject;
class TableObject;

class CallbackProperty : public Property
{
  public:
    CallbackProperty(ApplicationProgramObject* io, PropertyID id, bool writeEnable, PropertyDataType type, uint16_t maxElements,
                     uint8_t access, uint8_t (*readCallbackApplication)(ApplicationProgramObject*, uint16_t, uint8_t, uint8_t*),
                     uint8_t (*writeCallbackApplication)(ApplicationProgramObject*, uint16_t, uint8_t, const uint8_t*))
        : Property(id, writeEnable, type, maxElements, access),
          _interfaceObjectApplication(io), _readCallbackApplication(readCallbackApplication), _writeCallbackApplication(writeCallbackApplication)
    {}
    CallbackProperty(ApplicationProgramObject* io, PropertyID id, bool writeEnable, PropertyDataType type, uint16_t maxElements,
                     uint8_t access, uint8_t (*readCallbackApplication)(ApplicationProgramObject*, uint16_t, uint8_t, uint8_t*))
        : Property(id, writeEnable, type, maxElements, access), _interfaceObjectApplication(io), _readCallbackApplication(readCallbackApplication)
    {}
    CallbackProperty(DeviceObject* io, PropertyID id, bool writeEnable, PropertyDataType type, uint16_t maxElements,
                     uint8_t access, uint8_t (*readCallbackDevice)(DeviceObject*, uint16_t, uint8_t, uint8_t*),
                     uint8_t (*writeCallbackDevice)(DeviceObject*, uint16_t, uint8_t, const uint8_t*))
        : Property(id, writeEnable, type, maxElements, access),
          _interfaceObjectDevice(io), _readCallbackDevice(readCallbackDevice), _writeCallbackDevice(writeCallbackDevice)
    {}
    CallbackProperty(DeviceObject* io, PropertyID id, bool writeEnable, PropertyDataType type, uint16_t maxElements,
                     uint8_t access, uint8_t (*readCallbackDevice)(DeviceObject*, uint16_t, uint8_t, uint8_t*))
        : Property(id, writeEnable, type, maxElements, access), _interfaceObjectDevice(io), _readCallbackDevice(readCallbackDevice)
    {}
    CallbackProperty(TableObject* io, PropertyID id, bool writeEnable, PropertyDataType type, uint16_t maxElements,
                     uint8_t access, uint8_t (*readCallbackTable)(TableObject*, uint16_t, uint8_t, uint8_t*),
                     uint8_t (*writeCallbackTable)(TableObject*, uint16_t, uint8_t, const uint8_t*))
        : Property(id, writeEnable, type, maxElements, access),
          _interfaceObjectTable(io), _readCallbackTable(readCallbackTable), _writeCallbackTable(writeCallbackTable)
    {}
    CallbackProperty(TableObject* io, PropertyID id, bool writeEnable, PropertyDataType type, uint16_t maxElements,
                     uint8_t access, uint8_t (*readCallbackTable)(TableObject*, uint16_t, uint8_t, uint8_t*))
        : Property(id, writeEnable, type, maxElements, access), _interfaceObjectTable(io), _readCallbackTable(readCallbackTable)
    {}
    
    uint8_t read(uint16_t start, uint8_t count, uint8_t* data) const override
    {
        if (count == 0 || start > _maxElements || start + count > _maxElements + 1
        || _readCallbackApplication == nullptr|| _readCallbackDevice == nullptr|| _readCallbackTable == nullptr)
            return 0;
        if(_readCallbackApplication != nullptr){
          return _readCallbackApplication(_interfaceObjectApplication, start, count, data);
        }else if(_readCallbackDevice != nullptr){
          return _readCallbackDevice(_interfaceObjectDevice, start, count, data);
        }else if(_readCallbackTable != nullptr){
          return _readCallbackTable(_interfaceObjectTable, start, count, data);
        }
        // return _readCallback(_interfaceObject, start, count, data);
    }
    uint8_t write(uint16_t start, uint8_t count, const uint8_t* data) override
    {
        if (count == 0 || start > _maxElements || start + count > _maxElements + 1 
        || _writeCallbackApplication == nullptr|| _writeCallbackDevice == nullptr|| _writeCallbackTable == nullptr)
            return 0;
        if(_writeCallbackApplication != nullptr){
          return _writeCallbackApplication(_interfaceObjectApplication, start, count, data);
        }else if(_writeCallbackDevice != nullptr){
          return _writeCallbackDevice(_interfaceObjectDevice, start, count, data);
        }else if(_writeCallbackTable != nullptr){
          return _writeCallbackTable(_interfaceObjectTable, start, count, data);
        }
        // return _writeCallback(_interfaceObject, start, count, data);
    }
  private:
    ApplicationProgramObject* _interfaceObjectApplication = nullptr;
    uint8_t (*_readCallbackApplication)(ApplicationProgramObject*, uint16_t, uint8_t, uint8_t*) = nullptr;
    uint8_t (*_writeCallbackApplication)(ApplicationProgramObject*, uint16_t, uint8_t, const uint8_t*) = nullptr;

    DeviceObject* _interfaceObjectDevice = nullptr;
    uint8_t (*_readCallbackDevice)(DeviceObject*, uint16_t, uint8_t, uint8_t*) = nullptr;
    uint8_t (*_writeCallbackDevice)(DeviceObject*, uint16_t, uint8_t, const uint8_t*) = nullptr;

    TableObject* _interfaceObjectTable = nullptr;
    uint8_t (*_readCallbackTable)(TableObject*, uint16_t, uint8_t, uint8_t*) = nullptr;
    uint8_t (*_writeCallbackTable)(TableObject*, uint16_t, uint8_t, const uint8_t*) = nullptr;
};

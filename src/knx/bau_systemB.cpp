#include "bau_systemB.h"
#include "bits.h"
#include <string.h>
#include <stdio.h>

enum NmReadSerialNumberType
{
    NM_Read_SerialNumber_By_ProgrammingMode = 0x01,
    NM_Read_SerialNumber_By_ExFactoryState = 0x02,
    NM_Read_SerialNumber_By_PowerReset = 0x03,
    NM_Read_SerialNumber_By_ManufacturerSpecific = 0xFE,
};

static constexpr auto kFunctionPropertyResultBufferMaxSize = 0xFF; // EDA Fix V1.4
static constexpr auto kRestartProcessTime = 3;

BauSystemB::BauSystemB()
    : _memory(_deviceObj), _appProgram(_memory),
      _addrTable(_memory),
      _assocTable(_memory), _groupObjTable(_memory),
      _appLayer(*this),
      _transLayer(_appLayer), _netLayer(_deviceObj, _transLayer, LayerType::device),
      _dlLayer(_deviceObj, _netLayer.getInterface(), (ITpUartCallBacks &)*this)


{
    _memory.addSaveRestore(&_appProgram);

    _appLayer.transportLayer(_transLayer);
    _appLayer.associationTableObject(_assocTable);
    _transLayer.networkLayer(_netLayer);
    _transLayer.groupAddressTable(_addrTable);

    _memory.addSaveRestore(&_deviceObj);
    _memory.addSaveRestore(&_groupObjTable); // changed order for better memory management
    _memory.addSaveRestore(&_addrTable);
    _memory.addSaveRestore(&_assocTable);

    _netLayer.getInterface().dataLinkLayer(_dlLayer);

    // Set Mask Version in Device Object depending on the BAU
    _deviceObj.maskVersion(0x07B0);

    // Set which interface objects are available in the device object
    // This differs from BAU to BAU with different medium types.
    // See PID_IO_LIST
    Property *prop = _deviceObj.property(PID_IO_LIST);
    prop->write(1, (uint16_t)OT_DEVICE);
    prop->write(2, (uint16_t)OT_ADDR_TABLE);
    prop->write(3, (uint16_t)OT_ASSOC_TABLE);
    prop->write(4, (uint16_t)OT_GRP_OBJ_TABLE);
    prop->write(5, (uint16_t)OT_APPLICATION_PROG);
}

void BauSystemB::readMemory()
{
    _memory.readMemory();
}

void BauSystemB::writeMemory()
{
    _memory.writeMemory();
}

ApplicationProgramObject &BauSystemB::parameters()
{
    return _appProgram;
}

DeviceObject &BauSystemB::deviceObject()
{
    return _deviceObj;
}

uint8_t BauSystemB::checkmasterResetValidity(EraseCode eraseCode, uint8_t channel)
{
    static constexpr uint8_t successCode = 0x00;      // Where does this come from? It is the code for "success".
    static constexpr uint8_t invalidEraseCode = 0x02; // Where does this come from? It is the error code for "unspported erase code".

    switch (eraseCode)
    {
    case EraseCode::ConfirmedRestart:
    {
        print("Confirmed restart requested.\r\n");
        return successCode;
    }
    case EraseCode::ResetAP:
    {
        // TODO: increase download counter except for confirmed restart (PID_DOWNLOAD_COUNTER)
        print("ResetAP requested. Not implemented yet.\r\n");
        return successCode;
    }
    case EraseCode::ResetIA:
    {
        // TODO: increase download counter except for confirmed restart (PID_DOWNLOAD_COUNTER)
        print("ResetIA requested. Not implemented yet.\r\n");
        return successCode;
    }
    case EraseCode::ResetLinks:
    {
        // TODO: increase download counter except for confirmed restart (PID_DOWNLOAD_COUNTER)
        print("ResetLinks requested. Not implemented yet.\r\n");
        return successCode;
    }
    case EraseCode::ResetParam:
    {
        // TODO: increase download counter except for confirmed restart (PID_DOWNLOAD_COUNTER)
        print("ResetParam requested. Not implemented yet.\r\n");
        return successCode;
    }
    case EraseCode::FactoryReset:
    {
        // TODO: increase download counter except for confirmed restart (PID_DOWNLOAD_COUNTER)
        print("Factory reset requested. type: with IA\r\n");
        return successCode;
    }
    case EraseCode::FactoryResetWithoutIA:
    {
        // TODO: increase download counter except for confirmed restart (PID_DOWNLOAD_COUNTER)
        print("Factory reset requested. type: without IA\r\n");
        return successCode;
    }
    default:
    {
        print("Unhandled erase code: ");
        print(eraseCode, HEX);
        print("\r\n");
        return invalidEraseCode;
    }
    }
}

void BauSystemB::deviceDescriptorReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptorType)
{
    if (descriptorType != 0)
        descriptorType = 0x3f;

    uint8_t data[2];
    pushWord(_deviceObj.maskVersion(), data);
    applicationLayer().deviceDescriptorReadResponse(AckRequested, priority, hopType, asap, secCtrl, descriptorType, data);
}

void BauSystemB::memoryWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                       uint16_t memoryAddress, uint8_t *data)
{
    _memory.writeMemory(memoryAddress, number, data);
    if (_deviceObj.verifyMode())
        memoryReadIndication(priority, hopType, asap, secCtrl, number, memoryAddress, data);
}

void BauSystemB::memoryReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                      uint16_t memoryAddress, uint8_t *data)
{
    applicationLayer().memoryReadResponse(AckRequested, priority, hopType, asap, secCtrl, number, memoryAddress, data);
}

void BauSystemB::memoryReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                      uint16_t memoryAddress)
{
    applicationLayer().memoryReadResponse(AckRequested, priority, hopType, asap, secCtrl, number, memoryAddress,
                                          _memory.toAbsolute(memoryAddress));
}

void BauSystemB::memoryExtWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t *data)
{
    _memory.writeMemory(memoryAddress, number, data);

    applicationLayer().memoryExtWriteResponse(AckRequested, priority, hopType, asap, secCtrl, ReturnCodes::Success, number, memoryAddress, _memory.toAbsolute(memoryAddress));
}

void BauSystemB::memoryExtReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress)
{
    applicationLayer().memoryExtReadResponse(AckRequested, priority, hopType, asap, secCtrl, ReturnCodes::Success, number, memoryAddress, _memory.toAbsolute(memoryAddress));
}

void BauSystemB::doMasterReset(EraseCode eraseCode, uint8_t channel)
{
    _deviceObj.masterReset(eraseCode, channel);
    _appProgram.masterReset(eraseCode, channel);

    _addrTable.masterReset(eraseCode, channel);
    _assocTable.masterReset(eraseCode, channel);
    _groupObjTable.masterReset(eraseCode, channel);
}

void BauSystemB::restartRequestIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, RestartType restartType, EraseCode eraseCode, uint8_t channel)
{
    if (restartType == RestartType::BasicRestart)
    {
        print("Basic restart requested\r\n");
        if (_beforeRestart != 0)
            _beforeRestart();
    }
    else if (restartType == RestartType::MasterReset)
    {
        uint8_t errorCode = checkmasterResetValidity(eraseCode, channel);
        // We send the restart response now before actually applying the reset values
        // Processing time is kRestartProcessTime (example 3 seconds) that we require for the applying the master reset with restart
        applicationLayer().restartResponse(AckRequested, priority, hopType, secCtrl, errorCode, (errorCode == 0) ? kRestartProcessTime : 0);
        doMasterReset(eraseCode, channel);
    }
    else
    {
        // Cannot happen as restartType is just one bit
        print("Unhandled restart type.\r\n");
        fatalError();
    }

    // Flush the EEPROM before resetting
    _memory.writeMemory();
    restart();
}

void BauSystemB::authorizeIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint32_t key)
{
    applicationLayer().authorizeResponse(AckRequested, priority, hopType, asap, secCtrl, 0);
}

void BauSystemB::userMemoryReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress)
{
    applicationLayer().userMemoryReadResponse(AckRequested, priority, hopType, asap, secCtrl, number, memoryAddress,
                                              _memory.toAbsolute(memoryAddress));
}

void BauSystemB::userMemoryWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t *data)
{
    _memory.writeMemory(memoryAddress, number, data);

    if (_deviceObj.verifyMode())
        userMemoryReadIndication(priority, hopType, asap, secCtrl, number, memoryAddress);
}

void BauSystemB::propertyDescriptionReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                                   uint8_t propertyId, uint8_t propertyIndex)
{
    uint8_t pid = propertyId;
    bool writeEnable = false;
    uint8_t type = 0;
    uint16_t numberOfElements = 0;
    uint8_t access = 0;
    switch (objectIndex)
    {
    case 0:
        _deviceObj.readPropertyDescription(pid, propertyIndex, writeEnable, type, numberOfElements, access);
        break;
    case 1:
        _addrTable.readPropertyDescription(pid, propertyIndex, writeEnable, type, numberOfElements, access);
        break;
    case 2:
        _assocTable.readPropertyDescription(pid, propertyIndex, writeEnable, type, numberOfElements, access);
        break;
    case 3:
        _groupObjTable.readPropertyDescription(pid, propertyIndex, writeEnable, type, numberOfElements, access);
        break;
    case 4:
        _appProgram.readPropertyDescription(pid, propertyIndex, writeEnable, type, numberOfElements, access);
        break;
    case 5: // would be app_program 2
        nullptr;
    }

    applicationLayer().propertyDescriptionReadResponse(AckRequested, priority, hopType, asap, secCtrl, objectIndex, pid, propertyIndex,
                                                       writeEnable, type, numberOfElements, access);
}

void BauSystemB::propertyValueWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                              uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t *data, uint8_t length)
{
    // InterfaceObject *obj = getInterfaceObject(objectIndex);
    switch (objectIndex)
    {
    case 0:
        _deviceObj.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case 1:
        _addrTable.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case 2:
        _assocTable.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case 3:
        _groupObjTable.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case 4:
        _appProgram.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case 5: // would be app_program 2
        nullptr;
    }
    propertyValueReadIndication(priority, hopType, asap, secCtrl, objectIndex, propertyId, numberOfElements, startIndex);
}

void BauSystemB::propertyValueExtWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
                                                 uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t *data, uint8_t length, bool confirmed)
{
    uint8_t returnCode = ReturnCodes::Success;

    // InterfaceObject *obj = getInterfaceObject(objectType, objectInstance);
    switch (objectType)
    {
    case OT_DEVICE:
        _deviceObj.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case OT_ADDR_TABLE:
        _addrTable.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case OT_ASSOC_TABLE:
        _assocTable.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case OT_GRP_OBJ_TABLE:
        _groupObjTable.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case OT_APPLICATION_PROG:
        _appProgram.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    default:
        // numberOfElements = 0;
        returnCode = ReturnCodes::AddressVoid;
    }

    if (confirmed)
    {
        applicationLayer().propertyValueExtWriteConResponse(AckRequested, priority, hopType, asap, secCtrl, objectType, objectInstance, propertyId, numberOfElements, startIndex, returnCode);
    }
}

void BauSystemB::propertyValueReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                             uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex)
{
    uint8_t size = 0;
    uint8_t elementCount = numberOfElements;
    // InterfaceObject *obj = getInterfaceObject(objectIndex);
    switch (objectIndex)
    {
        uint8_t elementSize;
    case 0:
        elementSize = _deviceObj.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of property array entry 0 which contains the current number of elements

        break;
    case 1:
        elementSize = _addrTable.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of property array entry 0 which contains the current number of elements

        break;
    case 2:
        elementSize = _assocTable.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of property array entry 0 which contains the current number of elements

        break;
    case 3:
        elementSize = _groupObjTable.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of property array entry 0 which contains the current number of elements

        break;
    case 4:
        elementSize = _appProgram.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of property array entry 0 which contains the current number of elements

        break;
    case 5: // would be app_program 2
        nullptr;
    default:
        elementCount = 0;
    }
    uint8_t data[size];
    switch (objectIndex)
    {
    case 0:
        _deviceObj.readProperty((PropertyID)propertyId, startIndex, numberOfElements, data);
        break;
    case 1:
        _addrTable.readProperty((PropertyID)propertyId, startIndex, numberOfElements, data);
        break;
    case 2:
        _assocTable.readProperty((PropertyID)propertyId, startIndex, numberOfElements, data);
        break;
    case 3:
        _groupObjTable.readProperty((PropertyID)propertyId, startIndex, numberOfElements, data);
        break;
    case 4:
        _appProgram.readProperty((PropertyID)propertyId, startIndex, numberOfElements, data);
        break;
    case 5: // would be app_program 2
        nullptr;
    }
    if (elementCount == 0)
        size = 0;

    applicationLayer().propertyValueReadResponse(AckRequested, priority, hopType, asap, secCtrl, objectIndex, propertyId, elementCount,
                                                 startIndex, data, size);
}

void BauSystemB::propertyValueExtReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
                                                uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex)
{
    uint8_t size = 0;
    uint8_t elementCount = numberOfElements;
    uint8_t elementSize = 0;
    // InterfaceObject *obj = getInterfaceObject(objectType, objectInstance);
    switch (objectType)
    {
    case OT_DEVICE:
        elementSize = _deviceObj.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of propert array entry 0 which is the size
        break;

    case OT_ADDR_TABLE:
        elementSize = _addrTable.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of propert array entry 0 which is the size
        break;

    case OT_ASSOC_TABLE:
        elementSize = _assocTable.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of propert array entry 0 which is the size
        break;

    case OT_GRP_OBJ_TABLE:
        elementSize = _groupObjTable.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of propert array entry 0 which is the size
        break;

    case OT_APPLICATION_PROG:
        elementSize = _appProgram.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of propert array entry 0 which is the size
        break;


    default:
        elementCount = 0;
    }

    uint8_t data[size];
    switch (objectType)
    {
    case OT_DEVICE:
        _deviceObj.readProperty((PropertyID)propertyId, startIndex, elementCount, data);
        break;
    case OT_ADDR_TABLE:
        _addrTable.readProperty((PropertyID)propertyId, startIndex, elementCount, data);
        break;
    case OT_ASSOC_TABLE:
        _assocTable.readProperty((PropertyID)propertyId, startIndex, elementCount, data);
        break;
    case OT_GRP_OBJ_TABLE:
        _groupObjTable.readProperty((PropertyID)propertyId, startIndex, elementCount, data);
        break;
    case OT_APPLICATION_PROG:
        _appProgram.readProperty((PropertyID)propertyId, startIndex, elementCount, data);
        break;
    }

    if (elementCount == 0)
        size = 0;

    applicationLayer().propertyValueExtReadResponse(AckRequested, priority, hopType, asap, secCtrl, objectType, objectInstance, propertyId, elementCount,
                                                    startIndex, data, size);
}

void BauSystemB::functionPropertyCommandIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                                   uint8_t propertyId, uint8_t *data, uint8_t length)
{
    uint8_t resultData[kFunctionPropertyResultBufferMaxSize];
    uint8_t resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer

    bool handled = false;

    // InterfaceObject *obj = getInterfaceObject(objectIndex);
    switch (objectIndex)
    {
    case 0:
        if (_deviceObj.property((PropertyID)propertyId)->Type() == PDT_FUNCTION)
        {
            _deviceObj.command((PropertyID)propertyId, data, length, resultData, resultLength);
            handled = true;
        }
        else
        {
            if (_functionProperty != 0)
                if (_functionProperty(objectIndex, propertyId, length, data, resultData, resultLength))
                    handled = true;
        }
        break;
    case 1:
        if (_addrTable.property((PropertyID)propertyId)->Type() == PDT_FUNCTION)
        {
            _addrTable.command((PropertyID)propertyId, data, length, resultData, resultLength);
            handled = true;
        }
        else
        {
            if (_functionProperty != 0)
                if (_functionProperty(objectIndex, propertyId, length, data, resultData, resultLength))
                    handled = true;
        }
        break;
    case 2:
        if (_assocTable.property((PropertyID)propertyId)->Type() == PDT_FUNCTION)
        {
            _assocTable.command((PropertyID)propertyId, data, length, resultData, resultLength);
            handled = true;
        }
        else
        {
            if (_functionProperty != 0)
                if (_functionProperty(objectIndex, propertyId, length, data, resultData, resultLength))
                    handled = true;
        }
        break;
    case 3:
        if (_assocTable.property((PropertyID)propertyId)->Type() == PDT_FUNCTION)
        {
            _assocTable.command((PropertyID)propertyId, data, length, resultData, resultLength);
            handled = true;
        }
        else
        {
            if (_functionProperty != 0)
                if (_functionProperty(objectIndex, propertyId, length, data, resultData, resultLength))
                    handled = true;
        }
        break;
    case 4:
        if (_appProgram.property((PropertyID)propertyId)->Type() == PDT_FUNCTION)
        {
            _appProgram.command((PropertyID)propertyId, data, length, resultData, resultLength);
            handled = true;
        }
        else
        {
            if (_functionProperty != 0)
                if (_functionProperty(objectIndex, propertyId, length, data, resultData, resultLength))
                    handled = true;
        }
        break;
    case 5: // would be app_program 2
        nullptr;
    default:
        if (_functionProperty != 0)
            if (_functionProperty(objectIndex, propertyId, length, data, resultData, resultLength))
                handled = true;
    }

    // only return a value it was handled by a property or function
    if (handled)
        applicationLayer().functionPropertyStateResponse(AckRequested, priority, hopType, asap, secCtrl, objectIndex, propertyId, resultData, resultLength);
}

void BauSystemB::functionPropertyStateIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                                 uint8_t propertyId, uint8_t *data, uint8_t length)
{
    uint8_t resultData[kFunctionPropertyResultBufferMaxSize];
    uint8_t resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer

    bool handled = true;

    // InterfaceObject *obj = getInterfaceObject(objectIndex);
    switch (objectIndex)
    {
    case 0:
        if (_deviceObj.property((PropertyID)propertyId)->Type() == PDT_FUNCTION)
        {
            _deviceObj.state((PropertyID)propertyId, data, length, resultData, resultLength);
            handled = true;
        }
        else
        {
            if (_functionPropertyState != 0)
                if (_functionPropertyState(objectIndex, propertyId, length, data, resultData, resultLength))
                    handled = true;
        }
        break;
    case 1:
        if (_addrTable.property((PropertyID)propertyId)->Type() == PDT_FUNCTION)
        {
            _addrTable.state((PropertyID)propertyId, data, length, resultData, resultLength);
            handled = true;
        }
        else
        {
            if (_functionPropertyState != 0)
                if (_functionPropertyState(objectIndex, propertyId, length, data, resultData, resultLength))
                    handled = true;
        }
        break;
    case 2:
        if (_assocTable.property((PropertyID)propertyId)->Type() == PDT_FUNCTION)
        {
            _assocTable.state((PropertyID)propertyId, data, length, resultData, resultLength);
            handled = true;
        }
        else
        {
            if (_functionPropertyState != 0)
                if (_functionPropertyState(objectIndex, propertyId, length, data, resultData, resultLength))
                    handled = true;
        }
        break;
    case 3:
        if (_groupObjTable.property((PropertyID)propertyId)->Type() == PDT_FUNCTION)
        {
            _groupObjTable.state((PropertyID)propertyId, data, length, resultData, resultLength);
            handled = true;
        }
        else
        {
            if (_functionPropertyState != 0)
                if (_functionPropertyState(objectIndex, propertyId, length, data, resultData, resultLength))
                    handled = true;
        }
        break;
    case 4:
        if (_appProgram.property((PropertyID)propertyId)->Type() == PDT_FUNCTION)
        {
            _appProgram.state((PropertyID)propertyId, data, length, resultData, resultLength);
            handled = true;
        }
        else
        {
            if (_functionPropertyState != 0)
                if (_functionPropertyState(objectIndex, propertyId, length, data, resultData, resultLength))
                    handled = true;
        }
        break;
    case 5: // would be app_program 2
        nullptr;
    default:
        if (_functionPropertyState != 0)
            if (_functionPropertyState(objectIndex, propertyId, length, data, resultData, resultLength))
                handled = true;
    }

    // only return a value it was handled by a property or function
    if (handled)
        applicationLayer().functionPropertyStateResponse(AckRequested, priority, hopType, asap, secCtrl, objectIndex, propertyId, resultData, resultLength);
}

void BauSystemB::functionPropertyExtCommandIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
                                                      uint8_t propertyId, uint8_t *data, uint8_t length)
{
    uint8_t resultData[kFunctionPropertyResultBufferMaxSize];
    uint8_t resultLength = 1; // we always have to include the return code at least

    // InterfaceObject *obj = getInterfaceObject(objectType, objectInstance);
    switch (objectType)
    {
        PropertyDataType propType;
    case OT_DEVICE:
        propType = _deviceObj.property((PropertyID)propertyId)->Type();

        if (propType == PDT_FUNCTION)
        {
            // The first byte is reserved and 0 for PDT_FUNCTION
            uint8_t reservedByte = data[0];
            if (reservedByte != 0x00)
            {
                resultData[0] = ReturnCodes::DataVoid;
            }
            else
            {
                resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer
                _deviceObj.command((PropertyID)propertyId, data, length, resultData, resultLength);
                // resultLength was modified by the callee
            }
        }
        else if (propType == PDT_CONTROL)
        {
            uint8_t count = 1;
            // write the event
            _deviceObj.writeProperty((PropertyID)propertyId, 1, data, count);
            if (count == 1)
            {
                // Read the current state (one byte only) for the response
                _deviceObj.readProperty((PropertyID)propertyId, 1, count, &resultData[1]);
                resultLength = count ? 2 : 1;
                resultData[0] = count ? ReturnCodes::Success : ReturnCodes::DataVoid;
            }
            else
            {
                resultData[0] = ReturnCodes::AddressVoid;
            }
        }
        else
        {
            resultData[0] = ReturnCodes::DataTypeConflict;
        }
        break;
    case OT_ADDR_TABLE:
        propType = _addrTable.property((PropertyID)propertyId)->Type();

        if (propType == PDT_FUNCTION)
        {
            // The first byte is reserved and 0 for PDT_FUNCTION
            uint8_t reservedByte = data[0];
            if (reservedByte != 0x00)
            {
                resultData[0] = ReturnCodes::DataVoid;
            }
            else
            {
                resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer
                _addrTable.command((PropertyID)propertyId, data, length, resultData, resultLength);
                // resultLength was modified by the callee
            }
        }
        else if (propType == PDT_CONTROL)
        {
            uint8_t count = 1;
            // write the event
            _addrTable.writeProperty((PropertyID)propertyId, 1, data, count);
            if (count == 1)
            {
                // Read the current state (one byte only) for the response
                _addrTable.readProperty((PropertyID)propertyId, 1, count, &resultData[1]);
                resultLength = count ? 2 : 1;
                resultData[0] = count ? ReturnCodes::Success : ReturnCodes::DataVoid;
            }
            else
            {
                resultData[0] = ReturnCodes::AddressVoid;
            }
        }
        else
        {
            resultData[0] = ReturnCodes::DataTypeConflict;
        }
        break;
    case OT_ASSOC_TABLE:
        propType = _groupObjTable.property((PropertyID)propertyId)->Type();

        if (propType == PDT_FUNCTION)
        {
            // The first byte is reserved and 0 for PDT_FUNCTION
            uint8_t reservedByte = data[0];
            if (reservedByte != 0x00)
            {
                resultData[0] = ReturnCodes::DataVoid;
            }
            else
            {
                resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer
                _groupObjTable.command((PropertyID)propertyId, data, length, resultData, resultLength);
                // resultLength was modified by the callee
            }
        }
        else if (propType == PDT_CONTROL)
        {
            uint8_t count = 1;
            // write the event
            _groupObjTable.writeProperty((PropertyID)propertyId, 1, data, count);
            if (count == 1)
            {
                // Read the current state (one byte only) for the response
                _groupObjTable.readProperty((PropertyID)propertyId, 1, count, &resultData[1]);
                resultLength = count ? 2 : 1;
                resultData[0] = count ? ReturnCodes::Success : ReturnCodes::DataVoid;
            }
            else
            {
                resultData[0] = ReturnCodes::AddressVoid;
            }
        }
        else
        {
            resultData[0] = ReturnCodes::DataTypeConflict;
        }
        break;
    case OT_GRP_OBJ_TABLE:
        propType = _groupObjTable.property((PropertyID)propertyId)->Type();

        if (propType == PDT_FUNCTION)
        {
            // The first byte is reserved and 0 for PDT_FUNCTION
            uint8_t reservedByte = data[0];
            if (reservedByte != 0x00)
            {
                resultData[0] = ReturnCodes::DataVoid;
            }
            else
            {
                resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer
                _groupObjTable.command((PropertyID)propertyId, data, length, resultData, resultLength);
                // resultLength was modified by the callee
            }
        }
        else if (propType == PDT_CONTROL)
        {
            uint8_t count = 1;
            // write the event
            _groupObjTable.writeProperty((PropertyID)propertyId, 1, data, count);
            if (count == 1)
            {
                // Read the current state (one byte only) for the response
                _groupObjTable.readProperty((PropertyID)propertyId, 1, count, &resultData[1]);
                resultLength = count ? 2 : 1;
                resultData[0] = count ? ReturnCodes::Success : ReturnCodes::DataVoid;
            }
            else
            {
                resultData[0] = ReturnCodes::AddressVoid;
            }
        }
        else
        {
            resultData[0] = ReturnCodes::DataTypeConflict;
        }
        break;
    case OT_APPLICATION_PROG:
        propType = _appProgram.property((PropertyID)propertyId)->Type();

        if (propType == PDT_FUNCTION)
        {
            // The first byte is reserved and 0 for PDT_FUNCTION
            uint8_t reservedByte = data[0];
            if (reservedByte != 0x00)
            {
                resultData[0] = ReturnCodes::DataVoid;
            }
            else
            {
                resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer
                _appProgram.command((PropertyID)propertyId, data, length, resultData, resultLength);
                // resultLength was modified by the callee
            }
        }
        else if (propType == PDT_CONTROL)
        {
            uint8_t count = 1;
            // write the event
            _appProgram.writeProperty((PropertyID)propertyId, 1, data, count);
            if (count == 1)
            {
                // Read the current state (one byte only) for the response
                _appProgram.readProperty((PropertyID)propertyId, 1, count, &resultData[1]);
                resultLength = count ? 2 : 1;
                resultData[0] = count ? ReturnCodes::Success : ReturnCodes::DataVoid;
            }
            else
            {
                resultData[0] = ReturnCodes::AddressVoid;
            }
        }
        else
        {
            resultData[0] = ReturnCodes::DataTypeConflict;
        }
        break;
    default:
        resultData[0] = ReturnCodes::GenericError;
    }

    applicationLayer().functionPropertyExtStateResponse(AckRequested, priority, hopType, asap, secCtrl, objectType, objectInstance, propertyId, resultData, resultLength);
}

void BauSystemB::functionPropertyExtStateIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
                                                    uint8_t propertyId, uint8_t *data, uint8_t length)
{
    uint8_t resultData[kFunctionPropertyResultBufferMaxSize];
    uint8_t resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer
    PropertyDataType propType;
    // InterfaceObject *obj = getInterfaceObject(objectType, objectInstance);
    switch (objectType)
    {
    case OT_DEVICE:
        propType = _deviceObj.property((PropertyID)propertyId)->Type();

        if (propType == PDT_FUNCTION)
        {
            // The first byte is reserved and 0 for PDT_FUNCTION
            uint8_t reservedByte = data[0];
            if (reservedByte != 0x00)
            {
                resultData[0] = ReturnCodes::DataVoid;
            }
            else
            {
                resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer
                _deviceObj.state((PropertyID)propertyId, data, length, resultData, resultLength);
                // resultLength was modified by the callee
            }
        }
        else if (propType == PDT_CONTROL)
        {
            uint8_t count = 1;
            // Read the current state (one byte only) for the response
            _deviceObj.readProperty((PropertyID)propertyId, 1, count, &resultData[1]);
            resultLength = count ? 2 : 1;
            resultData[0] = count ? ReturnCodes::Success : ReturnCodes::DataVoid;
        }
        else
        {
            resultData[0] = ReturnCodes::DataTypeConflict;
        }
        break;

    case OT_ADDR_TABLE:
        propType = _addrTable.property((PropertyID)propertyId)->Type();

        if (propType == PDT_FUNCTION)
        {
            // The first byte is reserved and 0 for PDT_FUNCTION
            uint8_t reservedByte = data[0];
            if (reservedByte != 0x00)
            {
                resultData[0] = ReturnCodes::DataVoid;
            }
            else
            {
                resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer
                _addrTable.state((PropertyID)propertyId, data, length, resultData, resultLength);
                // resultLength was modified by the callee
            }
        }
        else if (propType == PDT_CONTROL)
        {
            uint8_t count = 1;
            // Read the current state (one byte only) for the response
            _addrTable.readProperty((PropertyID)propertyId, 1, count, &resultData[1]);
            resultLength = count ? 2 : 1;
            resultData[0] = count ? ReturnCodes::Success : ReturnCodes::DataVoid;
        }
        else
        {
            resultData[0] = ReturnCodes::DataTypeConflict;
        }
        break;

    case OT_ASSOC_TABLE:
        propType = _assocTable.property((PropertyID)propertyId)->Type();

        if (propType == PDT_FUNCTION)
        {
            // The first byte is reserved and 0 for PDT_FUNCTION
            uint8_t reservedByte = data[0];
            if (reservedByte != 0x00)
            {
                resultData[0] = ReturnCodes::DataVoid;
            }
            else
            {
                resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer
                _assocTable.state((PropertyID)propertyId, data, length, resultData, resultLength);
                // resultLength was modified by the callee
            }
        }
        else if (propType == PDT_CONTROL)
        {
            uint8_t count = 1;
            // Read the current state (one byte only) for the response
            _assocTable.readProperty((PropertyID)propertyId, 1, count, &resultData[1]);
            resultLength = count ? 2 : 1;
            resultData[0] = count ? ReturnCodes::Success : ReturnCodes::DataVoid;
        }
        else
        {
            resultData[0] = ReturnCodes::DataTypeConflict;
        }
        break;
    case OT_GRP_OBJ_TABLE:
        propType = _groupObjTable.property((PropertyID)propertyId)->Type();

        if (propType == PDT_FUNCTION)
        {
            // The first byte is reserved and 0 for PDT_FUNCTION
            uint8_t reservedByte = data[0];
            if (reservedByte != 0x00)
            {
                resultData[0] = ReturnCodes::DataVoid;
            }
            else
            {
                resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer
                _groupObjTable.state((PropertyID)propertyId, data, length, resultData, resultLength);
                // resultLength was modified by the callee
            }
        }
        else if (propType == PDT_CONTROL)
        {
            uint8_t count = 1;
            // Read the current state (one byte only) for the response
            _groupObjTable.readProperty((PropertyID)propertyId, 1, count, &resultData[1]);
            resultLength = count ? 2 : 1;
            resultData[0] = count ? ReturnCodes::Success : ReturnCodes::DataVoid;
        }
        else
        {
            resultData[0] = ReturnCodes::DataTypeConflict;
        }
        break;

    case OT_APPLICATION_PROG:
        propType = _appProgram.property((PropertyID)propertyId)->Type();

        if (propType == PDT_FUNCTION)
        {
            // The first byte is reserved and 0 for PDT_FUNCTION
            uint8_t reservedByte = data[0];
            if (reservedByte != 0x00)
            {
                resultData[0] = ReturnCodes::DataVoid;
            }
            else
            {
                resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer
                _appProgram.state((PropertyID)propertyId, data, length, resultData, resultLength);
                // resultLength was modified by the callee
            }
        }
        else if (propType == PDT_CONTROL)
        {
            uint8_t count = 1;
            // Read the current state (one byte only) for the response
            _appProgram.readProperty((PropertyID)propertyId, 1, count, &resultData[1]);
            resultLength = count ? 2 : 1;
            resultData[0] = count ? ReturnCodes::Success : ReturnCodes::DataVoid;
        }
        else
        {
            resultData[0] = ReturnCodes::DataTypeConflict;
        }
        break;

    default:
        resultData[0] = ReturnCodes::GenericError;
    }

    applicationLayer().functionPropertyExtStateResponse(AckRequested, priority, hopType, asap, secCtrl, objectType, objectInstance, propertyId, resultData, resultLength);
}

void BauSystemB::individualAddressReadIndication(HopCountType hopType, const SecurityControl &secCtrl)
{
    if (_deviceObj.progMode())
        applicationLayer().individualAddressReadResponse(AckRequested, hopType, secCtrl);
}

void BauSystemB::individualAddressWriteIndication(HopCountType hopType, const SecurityControl &secCtrl, uint16_t newaddress)
{
    if (_deviceObj.progMode())
        _deviceObj.individualAddress(newaddress);
}

void BauSystemB::individualAddressSerialNumberWriteIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint16_t newIndividualAddress,
                                                              uint8_t *knxSerialNumber)
{
    // If the received serial number matches our serial number
    // then store the received new individual address in the device object
    if (!memcmp(knxSerialNumber, _deviceObj.propertyData(PID_SERIAL_NUMBER), 6))
        _deviceObj.individualAddress(newIndividualAddress);
}

void BauSystemB::individualAddressSerialNumberReadIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t *knxSerialNumber)
{
    // If the received serial number matches our serial number
    // then send a response with the serial number. The domain address is set to 0 for closed media.
    // An open medium BAU has to override this method and provide a proper domain address.
    if (!memcmp(knxSerialNumber, _deviceObj.propertyData(PID_SERIAL_NUMBER), 6))
    {
        uint8_t emptyDomainAddress[6] = {0x00};
        applicationLayer().IndividualAddressSerialNumberReadResponse(priority, hopType, secCtrl, emptyDomainAddress, knxSerialNumber);
    }
}

void BauSystemB::addSaveRestore(DeviceObject *obj)
{
    _memory.addSaveRestore(obj);
}

bool BauSystemB::restartRequest(uint16_t asap, const SecurityControl secCtrl)
{
    if (applicationLayer().isConnected())
        return false;
    _restartState = Connecting; // order important, has to be set BEFORE connectRequest
    _restartSecurity = secCtrl;
    applicationLayer().connectRequest(asap, SystemPriority);
    applicationLayer().deviceDescriptorReadRequest(AckRequested, SystemPriority, NetworkLayerParameter, asap, secCtrl, 0);
    return true;
}

void BauSystemB::connectConfirm(uint16_t tsap)
{
    if (_restartState == Connecting)
    {
        /* restart connection is confirmed, go to the next state */
        _restartState = Connected;
        _restartDelay = HAL_GetTick();
    }
    else
    {
        _restartState = Idle;
    }
}

void BauSystemB::nextRestartState()
{
    switch (_restartState)
    {
    case Idle:
        /* inactive state, do nothing */
        break;
    case Connecting:
        /* wait for connection, we do nothing here */
        break;
    case Connected:
        /* connection confirmed, we send restartRequest, but we wait a moment (sending ACK etc)... */
        if (HAL_GetTick() - _restartDelay > 30)
        {
            applicationLayer().restartRequest(AckRequested, SystemPriority, NetworkLayerParameter, _restartSecurity);
            _restartState = Restarted;
            _restartDelay = HAL_GetTick();
        }
        break;
    case Restarted:
        /* restart is finished, we send a disconnect */
        if (HAL_GetTick() - _restartDelay > 30)
        {
            applicationLayer().disconnectRequest(SystemPriority);
            _restartState = Idle;
        }
    default:
        break;
    }
}

void BauSystemB::systemNetworkParameterReadIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint16_t objectType,
                                                      uint16_t propertyId, uint8_t *testInfo, uint16_t testInfoLength)
{
    uint8_t operand;

    popByte(&operand, testInfo + 1); // First byte (+ 0) contains only 4 reserved bits (0)

    // See KNX spec. 3.5.2 p.33 (Management Procedures: Procedures with A_SystemNetworkParameter_Read)
    switch ((NmReadSerialNumberType)operand)
    {
    case NM_Read_SerialNumber_By_ProgrammingMode: // NM_Read_SerialNumber_By_ProgrammingMode
        // Only send a reply if programming mode is on
        if (_deviceObj.progMode() && (objectType == OT_DEVICE) && (propertyId == PID_SERIAL_NUMBER))
        {
            // Send reply. testResult data is KNX serial number
            applicationLayer().systemNetworkParameterReadResponse(priority, hopType, secCtrl, objectType, propertyId,
                                                                  testInfo, testInfoLength, (uint8_t *)_deviceObj.propertyData(PID_SERIAL_NUMBER), 6);
        }
        break;

    case NM_Read_SerialNumber_By_ExFactoryState: // NM_Read_SerialNumber_By_ExFactoryState
        break;

    case NM_Read_SerialNumber_By_PowerReset: // NM_Read_SerialNumber_By_PowerReset
        break;

    case NM_Read_SerialNumber_By_ManufacturerSpecific: // Manufacturer specific use of A_SystemNetworkParameter_Read
        break;
    }
}

void BauSystemB::systemNetworkParameterReadLocalConfirm(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint16_t objectType,
                                                        uint16_t propertyId, uint8_t *testInfo, uint16_t testInfoLength, bool status)
{
}

void BauSystemB::propertyValueRead(ObjectType objectType, uint8_t objectInstance, uint8_t propertyId,
                                   uint8_t &numberOfElements, uint16_t startIndex,
                                   uint8_t **data, uint32_t &length)
{
    uint32_t size = 0;
    uint8_t elementCount = numberOfElements;

    // InterfaceObject *obj = getInterfaceObject(objectType, objectInstance);
    switch (objectType)
    {
        uint8_t elementSize;
    case OT_DEVICE:
        elementSize = _deviceObj.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of property array entry 0 which contains the current number of elements
        *data = new uint8_t[size];
        _deviceObj.readProperty((PropertyID)propertyId, startIndex, elementCount, *data);
        break;
    case OT_ADDR_TABLE:
        elementSize = _addrTable.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of property array entry 0 which contains the current number of elements
        *data = new uint8_t[size];
        _addrTable.readProperty((PropertyID)propertyId, startIndex, elementCount, *data);
        break;
    case OT_ASSOC_TABLE:
        elementSize = _assocTable.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of property array entry 0 which contains the current number of elements
        *data = new uint8_t[size];
        _assocTable.readProperty((PropertyID)propertyId, startIndex, elementCount, *data);
        break;
    case OT_GRP_OBJ_TABLE:
        elementSize = _groupObjTable.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of property array entry 0 which contains the current number of elements
        *data = new uint8_t[size];
        _groupObjTable.readProperty((PropertyID)propertyId, startIndex, elementCount, *data);
        break;
    case OT_APPLICATION_PROG:
        elementSize = _appProgram.propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of property array entry 0 which contains the current number of elements
        *data = new uint8_t[size];
        _appProgram.readProperty((PropertyID)propertyId, startIndex, elementCount, *data);
        break;
    default:
        elementCount = 0;
        *data = nullptr;
    }

    numberOfElements = elementCount;
    length = size;
}

void BauSystemB::propertyValueWrite(ObjectType objectType, uint8_t objectInstance, uint8_t propertyId,
                                    uint8_t &numberOfElements, uint16_t startIndex,
                                    uint8_t *data, uint32_t length)
{
    // InterfaceObject *obj = getInterfaceObject(objectType, objectInstance);

    switch (objectType)
    {
    case OT_DEVICE:
        _deviceObj.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case OT_ADDR_TABLE:
        _addrTable.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case OT_ASSOC_TABLE:
        _assocTable.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case OT_GRP_OBJ_TABLE:
        _groupObjTable.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    case OT_APPLICATION_PROG:
        _appProgram.writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
        break;
    default:
        numberOfElements = 0;
    }
}

Memory &BauSystemB::memory()
{
    return _memory;
}

void BauSystemB::versionCheckCallback(VersionCheckCallback func)
{
    _memory.versionCheckCallback(func);
}

VersionCheckCallback BauSystemB::versionCheckCallback()
{
    return _memory.versionCheckCallback();
}

void BauSystemB::beforeRestartCallback(BeforeRestartCallback func)
{
    _beforeRestart = func;
}

BeforeRestartCallback BauSystemB::beforeRestartCallback()
{
    return _beforeRestart;
}

void BauSystemB::functionPropertyCallback(FunctionPropertyCallback func)
{
    _functionProperty = func;
}

FunctionPropertyCallback BauSystemB::functionPropertyCallback()
{
    return _functionProperty;
}
void BauSystemB::functionPropertyStateCallback(FunctionPropertyCallback func)
{
    _functionPropertyState = func;
}

FunctionPropertyCallback BauSystemB::functionPropertyStateCallback()
{
    return _functionPropertyState;
}

void BauSystemB::groupValueReadLocalConfirm(AckType ack, uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl, bool status)
{
    GroupObject &go = _groupObjTable.get(asap);
    if (status)
        go.commFlag(Ok);
    else
        go.commFlag(Error);
}

void BauSystemB::groupValueReadIndication(uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl)
{
    GroupObject &go = _groupObjTable.get(asap);

    if (!go.communicationEnable() || !go.readEnable())
        return;

    uint8_t *data = go.valueRef();
    _appLayer.groupValueReadResponse(AckRequested, asap, priority, hopType, secCtrl, data, go.sizeInTelegram());
}

void BauSystemB::groupValueReadResponseConfirm(AckType ack, uint16_t asap, Priority priority, HopCountType hopTtype, const SecurityControl &secCtrl, uint8_t *data, uint8_t dataLength, bool status)
{
}

void BauSystemB::groupValueReadAppLayerConfirm(uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t *data, uint8_t dataLength)
{
    GroupObject &go = _groupObjTable.get(asap);

    if (!go.communicationEnable() || !go.responseUpdateEnable())
        return;

    updateGroupObject(go, data, dataLength);
}

void BauSystemB::groupValueWriteLocalConfirm(AckType ack, uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t *data, uint8_t dataLength, bool status)
{
    GroupObject &go = _groupObjTable.get(asap);
    if (status)
        go.commFlag(Ok);
    else
        go.commFlag(Error);
}

void BauSystemB::groupValueWriteIndication(uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t *data, uint8_t dataLength)
{
    GroupObject &go = _groupObjTable.get(asap);

    if (!go.communicationEnable() || !go.writeEnable())
        return;

    updateGroupObject(go, data, dataLength);
}

void BauSystemB::individualAddressWriteLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint16_t newaddress, bool status)
{
}

void BauSystemB::individualAddressReadLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, bool status)
{
}

void BauSystemB::individualAddressReadResponseConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, bool status)
{
}

void BauSystemB::individualAddressReadAppLayerConfirm(HopCountType hopType, const SecurityControl &secCtrl, uint16_t individualAddress)
{
}

void BauSystemB::individualAddressSerialNumberReadLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint8_t *serialNumber, bool status)
{
}

void BauSystemB::individualAddressSerialNumberReadResponseConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint8_t *serialNumber, uint16_t domainAddress, bool status)
{
}

void BauSystemB::individualAddressSerialNumberReadAppLayerConfirm(HopCountType hopType, const SecurityControl &secCtrl, uint8_t *serialNumber, uint16_t individualAddress, uint16_t domainAddress)
{
}

void BauSystemB::individualAddressSerialNumberWriteLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint8_t *serialNumber, uint16_t newaddress, bool status)
{
}

void BauSystemB::deviceDescriptorReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptorType, bool status)
{
}

void BauSystemB::deviceDescriptorReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptor_type,
                                                     uint8_t *device_descriptor, bool status)
{
}

void BauSystemB::deviceDescriptorReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptortype, uint8_t *deviceDescriptor)
{
}

void BauSystemB::restartRequestLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, bool status)
{
}

void BauSystemB::propertyValueReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, bool status)
{
}

void BauSystemB::propertyValueReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t *data, uint8_t length, bool status)
{
}

void BauSystemB::propertyValueReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t *data, uint8_t length)
{
}

void BauSystemB::propertyValueWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t *data, uint8_t length, bool status)
{
}

void BauSystemB::propertyDescriptionReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool status)
{
}

void BauSystemB::propertyDescriptionReadResponse(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool writeEnable, uint8_t type, uint16_t maxNumberOfElements, uint8_t access)
{
}

void BauSystemB::propertyDescriptionReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool writeEnable, uint8_t type, uint16_t maxNumberOfElements, uint8_t access, bool status)
{
}

void BauSystemB::propertyDescriptionReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool writeEnable, uint8_t type, uint16_t maxNumberOfElements, uint8_t access)
{
}

void BauSystemB::memoryReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint16_t memoryAddress, bool status)
{
}

void BauSystemB::memoryReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint16_t memoryAddress, uint8_t *data, bool status)
{
}

void BauSystemB::memoryReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint16_t memoryAddress, uint8_t *data)
{
}

void BauSystemB::memoryWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint16_t memoryAddress, uint8_t *data, bool status)
{
}

void BauSystemB::memoryExtReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, bool status)
{
}

void BauSystemB::memoryExtReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t *data, bool status)
{
}

void BauSystemB::memoryExtReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t *data)
{
}

void BauSystemB::memoryExtWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t *data, bool status)
{
}

void BauSystemB::memoryExtWriteResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t *data, bool status)
{
}

void BauSystemB::memoryExtWriteAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t *data)
{
}

void BauSystemB::userMemoryReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, bool status)
{
}

void BauSystemB::userMemoryReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t *memoryData, bool status)
{
}

void BauSystemB::userMemoryReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t *memoryData)
{
}

void BauSystemB::userMemoryWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t *memoryData, bool status)
{
}

void BauSystemB::userManufacturerInfoLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, bool status)
{
}

void BauSystemB::userManufacturerInfoIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl)
{
}

void BauSystemB::userManufacturerInfoResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t *info, bool status)
{
}

void BauSystemB::userManufacturerInfoAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t *info)
{
}

void BauSystemB::authorizeLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint32_t key, bool status)
{
}

void BauSystemB::authorizeResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level, bool status)
{
}

void BauSystemB::authorizeAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level)
{
}

void BauSystemB::keyWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level, uint32_t key, bool status)
{
}

void BauSystemB::keyWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level, uint32_t key)
{
}

void BauSystemB::keyWriteResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level, bool status)
{
}

void BauSystemB::keyWriteAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level)
{
}

void BauSystemB::domainAddressSerialNumberWriteIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t *rfDoA,
                                                          const uint8_t *knxSerialNumber)
{
}

void BauSystemB::domainAddressSerialNumberReadIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t *knxSerialNumber)
{
}

void BauSystemB::domainAddressSerialNumberWriteLocalConfirm(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t *rfDoA,
                                                            const uint8_t *knxSerialNumber, bool status)
{
}

void BauSystemB::domainAddressSerialNumberReadLocalConfirm(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t *knxSerialNumber, bool status)
{
}

void BauSystemB::updateGroupObject(GroupObject &go, uint8_t *data, uint8_t length)
{
    uint8_t *goData = go.valueRef();
    if (length != go.valueSize())
    {
        go.commFlag(Error);
        return;
    }

    memcpy(goData, data, length);

    if (go.commFlag() != WriteRequest)
    {
        go.commFlag(Updated);
#ifdef SMALL_GROUPOBJECT
        GroupObject::processClassCallback(go);
#else
        GroupObjectUpdatedHandler handler = go.callback();
        if (handler)
            handler(go);
#endif
    }
    else
    {
        go.commFlag(Updated);
    }
}

void BauSystemB::sendNextGroupTelegram()
{
    if (!configured())
        return;

    static uint16_t startIdx = 1;

    GroupObjectTableObject &table = _groupObjTable;
    uint16_t objCount = table.entryCount();

    for (uint16_t asap = startIdx; asap <= objCount; asap++)
    {
        GroupObject &go = table.get(asap);

        ComFlag flag = go.commFlag();
        if (flag != ReadRequest && flag != WriteRequest)
            continue;

        if (flag == WriteRequest)
        {
#ifdef SMALL_GROUPOBJECT
            GroupObject::processClassCallback(go);
#else
            GroupObjectUpdatedHandler handler = go.callback();
            if (handler)
                handler(go);
#endif
        }
        if (!go.communicationEnable())
        {
            go.commFlag(Ok);
            continue;
        }

        SecurityControl goSecurity;
        goSecurity.toolAccess = false; // Secured group communication never uses the toolkey. ETS knows all keys, also the group keys.

        goSecurity.dataSecurity = DataSecurity::None;

        if (flag == WriteRequest && go.transmitEnable())
        {
            uint8_t *data = go.valueRef();
            _appLayer.groupValueWriteRequest(AckRequested, asap, go.priority(), NetworkLayerParameter, goSecurity, data,
                                             go.sizeInTelegram());
        }
        else if (flag == ReadRequest)
        {
            _appLayer.groupValueReadRequest(AckRequested, asap, go.priority(), NetworkLayerParameter, goSecurity);
        }

        go.commFlag(Transmitting);

        startIdx = asap + 1;
        return;
    }

    startIdx = 1;
}

ApplicationLayer &BauSystemB::applicationLayer()
{
    return _appLayer;
}

GroupObjectTableObject &BauSystemB::groupObjectTable()
{
    return _groupObjTable;
}

bool BauSystemB::configured()
{
    // _configured is set to true initially, if the device was configured with ETS it will be set to true after restart

    if (!_configured)
        return false;

    _configured = _groupObjTable.loadState() == LS_LOADED && _addrTable.loadState() == LS_LOADED && _assocTable.loadState() == LS_LOADED && _appProgram.loadState() == LS_LOADED;

    return _configured;
}

void BauSystemB::loop()
{
    _dlLayer.loop();

    _transLayer.loop();
    sendNextGroupTelegram();
    nextRestartState();
}

bool BauSystemB::isAckRequired(uint16_t address, bool isGrpAddr)
{
    if (isGrpAddr)
    {
        // ACK for broadcasts
        if (address == 0)
            return true;
        // is group address in group address table? ACK if yes.
        return _addrTable.contains(address);
    }

    // Also ACK for our own individual address
    if (address == _deviceObj.individualAddress())
        return true;

    if (address == 0)
    {
        print("Invalid broadcast detected: destination address is 0, but address type is \"individual\"\r\n");
    }

    return false;
}

bool BauSystemB::enabled()
{
    return _dlLayer.enabled();
}

void BauSystemB::enabled(bool value)
{
    _dlLayer.enabled(value);
}
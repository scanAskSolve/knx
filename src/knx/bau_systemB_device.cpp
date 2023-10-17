#include "bau_systemB_device.h"
#include "bits.h"
#include <string.h>
#include <stdio.h>

BauSystemBDevice::BauSystemBDevice(ArduinoPlatform& platform) :
    _memory(platform, _deviceObj),
    _appProgram(_memory),
    _platform(platform),
    _addrTable(_memory),
    _assocTable(_memory), _groupObjTable(_memory),
#ifdef USE_DATASECURE
    _appLayer(_deviceObj, _secIfObj, *this),
#else
    _appLayer(*this),
#endif
    _transLayer(_appLayer), _netLayer(_deviceObj, _transLayer)
{
    
    _memory.addSaveRestore(&_appProgram);

    _appLayer.transportLayer(_transLayer);
    _appLayer.associationTableObject(_assocTable);
#ifdef USE_DATASECURE
    _appLayer.groupAddressTable(_addrTable);
#endif
    _transLayer.networkLayer(_netLayer);
    _transLayer.groupAddressTable(_addrTable);

    _memory.addSaveRestore(&_deviceObj);
    _memory.addSaveRestore(&_groupObjTable); // changed order for better memory management
    _memory.addSaveRestore(&_addrTable);
    _memory.addSaveRestore(&_assocTable);
#ifdef USE_DATASECURE
    _memory.addSaveRestore(&_secIfObj);
#endif
}

ApplicationLayer& BauSystemBDevice::applicationLayer()
{
    return _appLayer;
}

GroupObjectTableObject& BauSystemBDevice::groupObjectTable()
{
    return _groupObjTable;
}

void BauSystemBDevice::loop()
{
    _transLayer.loop();
    sendNextGroupTelegram();
    nextRestartState();
#ifdef USE_DATASECURE
    _appLayer.loop();
#endif
}

void BauSystemBDevice::sendNextGroupTelegram()
{
    if(!configured())
        return;
    
    static uint16_t startIdx = 1;

    GroupObjectTableObject& table = _groupObjTable;
    uint16_t objCount = table.entryCount();

    for (uint16_t asap = startIdx; asap <= objCount; asap++)
    {
        GroupObject& go = table.get(asap);

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

#ifdef USE_DATASECURE
        // Get security flags from Security Interface Object for this group object
        goSecurity.dataSecurity = _secIfObj.getGroupObjectSecurity(asap);
#else
        goSecurity.dataSecurity = DataSecurity::None;
#endif

        if (flag == WriteRequest && go.transmitEnable())
        {
            uint8_t* data = go.valueRef();
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

void BauSystemBDevice::updateGroupObject(GroupObject & go, uint8_t * data, uint8_t length)
{
    uint8_t* goData = go.valueRef();
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

bool BauSystemBDevice::configured()
{
    // _configured is set to true initially, if the device was configured with ETS it will be set to true after restart
    
    if (!_configured)
        return false;
    
    _configured = _groupObjTable.loadState() == LS_LOADED
        && _addrTable.loadState() == LS_LOADED
        && _assocTable.loadState() == LS_LOADED
        && _appProgram.loadState() == LS_LOADED;

#ifdef USE_DATASECURE
    _configured &= _secIfObj.loadState() == LS_LOADED;
#endif

    return _configured;
}

void BauSystemBDevice::doMasterReset(EraseCode eraseCode, uint8_t channel)
{
    _deviceObj.masterReset(eraseCode, channel);
    _appProgram.masterReset(eraseCode, channel);
    _addrTable.masterReset(eraseCode, channel);
    _assocTable.masterReset(eraseCode, channel);
    _groupObjTable.masterReset(eraseCode, channel);
#ifdef USE_DATASECURE
    _secIfObj.masterReset(eraseCode, channel);
#endif
}

void BauSystemBDevice::groupValueWriteLocalConfirm(AckType ack, uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t * data, uint8_t dataLength, bool status)
{
    GroupObject& go = _groupObjTable.get(asap);
    if (status)
        go.commFlag(Ok);
    else
        go.commFlag(Error);
}

void BauSystemBDevice::groupValueReadLocalConfirm(AckType ack, uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl, bool status)
{
    GroupObject& go = _groupObjTable.get(asap);
    if (status)
        go.commFlag(Ok);
    else
        go.commFlag(Error);
}

void BauSystemBDevice::groupValueReadIndication(uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl)
{
#ifdef USE_DATASECURE
    DataSecurity requiredGoSecurity;

    // Get security flags from Security Interface Object for this group object
    requiredGoSecurity = _secIfObj.getGroupObjectSecurity(asap);

    if (secCtrl.dataSecurity != requiredGoSecurity)
    {
        println("GroupValueRead: access denied due to wrong security flags");
        return;
    }
#endif

    GroupObject& go = _groupObjTable.get(asap);

    if (!go.communicationEnable() || !go.readEnable())
        return;
    
    uint8_t* data = go.valueRef();
    _appLayer.groupValueReadResponse(AckRequested, asap, priority, hopType, secCtrl, data, go.sizeInTelegram());
}

void BauSystemBDevice::groupValueReadAppLayerConfirm(uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t* data,
    uint8_t dataLength)
{
    GroupObject& go = _groupObjTable.get(asap);

    if (!go.communicationEnable() || !go.responseUpdateEnable())
        return;

    updateGroupObject(go, data, dataLength);
}

void BauSystemBDevice::groupValueWriteIndication(uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t * data, uint8_t dataLength)
{
#ifdef USE_DATASECURE
    DataSecurity requiredGoSecurity;

    // Get security flags from Security Interface Object for this group object
    requiredGoSecurity = _secIfObj.getGroupObjectSecurity(asap);

    if (secCtrl.dataSecurity != requiredGoSecurity)
    {
        println("GroupValueWrite: access denied due to wrong security flags");
        return;
    }
#endif
    GroupObject& go = _groupObjTable.get(asap);

    if (!go.communicationEnable() || !go.writeEnable())
        return;

    updateGroupObject(go, data, dataLength);
}


enum NmReadSerialNumberType
{
    NM_Read_SerialNumber_By_ProgrammingMode = 0x01,
    NM_Read_SerialNumber_By_ExFactoryState = 0x02,
    NM_Read_SerialNumber_By_PowerReset = 0x03,
    NM_Read_SerialNumber_By_ManufacturerSpecific = 0xFE,
};

static constexpr auto kFunctionPropertyResultBufferMaxSize = 64;
static constexpr auto kRestartProcessTime = 3;


void BauSystemBDevice::readMemory()
{
    _memory.readMemory();
}

void BauSystemBDevice::writeMemory()
{
    _memory.writeMemory();
}

ArduinoPlatform& BauSystemBDevice::platform()
{
    return _platform;
}

ApplicationProgramObject& BauSystemBDevice::parameters()
{
    return _appProgram;
}

DeviceObject& BauSystemBDevice::deviceObject()
{
    return _deviceObj;
}

uint8_t BauSystemBDevice::checkmasterResetValidity(EraseCode eraseCode, uint8_t channel)
{
    static constexpr uint8_t successCode = 0x00; // Where does this come from? It is the code for "success".
    static constexpr uint8_t invalidEraseCode = 0x02; // Where does this come from? It is the error code for "unspported erase code".

    switch (eraseCode)
    {
        case EraseCode::ConfirmedRestart:
        {
            println("Confirmed restart requested.");
            return successCode;
        }
        case EraseCode::ResetAP:
        {
            // TODO: increase download counter except for confirmed restart (PID_DOWNLOAD_COUNTER)
            println("ResetAP requested. Not implemented yet.");
            return successCode;
        }
        case EraseCode::ResetIA:
        {
            // TODO: increase download counter except for confirmed restart (PID_DOWNLOAD_COUNTER)
            println("ResetIA requested. Not implemented yet.");
            return successCode;
        }
        case EraseCode::ResetLinks:
        {
            // TODO: increase download counter except for confirmed restart (PID_DOWNLOAD_COUNTER)
            println("ResetLinks requested. Not implemented yet.");
            return successCode;
        }
        case EraseCode::ResetParam:
        {
            // TODO: increase download counter except for confirmed restart (PID_DOWNLOAD_COUNTER)
            println("ResetParam requested. Not implemented yet.");
            return successCode;
        }
        case EraseCode::FactoryReset:
        {
            // TODO: increase download counter except for confirmed restart (PID_DOWNLOAD_COUNTER)
            println("Factory reset requested. type: with IA");
            return successCode;
        }
        case EraseCode::FactoryResetWithoutIA:
        {
            // TODO: increase download counter except for confirmed restart (PID_DOWNLOAD_COUNTER)
            println("Factory reset requested. type: without IA");
            return successCode;
        }
        default:
        {
            print("Unhandled erase code: ");
            println(eraseCode, HEX);
            return invalidEraseCode;
        }
    }
}

void BauSystemBDevice::deviceDescriptorReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptorType)
{
    if (descriptorType != 0)
        descriptorType = 0x3f;
    
    uint8_t data[2];
    pushWord(_deviceObj.maskVersion(), data);
    applicationLayer().deviceDescriptorReadResponse(AckRequested, priority, hopType, asap, secCtrl, descriptorType, data);
}

void BauSystemBDevice::memoryWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
    uint16_t memoryAddress, uint8_t * data)
{
    _memory.writeMemory(memoryAddress, number, data);
    if (_deviceObj.verifyMode())
        memoryReadIndication(priority, hopType, asap, secCtrl, number, memoryAddress, data);
}

void BauSystemBDevice::memoryReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
    uint16_t memoryAddress, uint8_t * data)
{
    applicationLayer().memoryReadResponse(AckRequested, priority, hopType, asap, secCtrl, number, memoryAddress, data);
}

void BauSystemBDevice::memoryReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
    uint16_t memoryAddress)
{
    applicationLayer().memoryReadResponse(AckRequested, priority, hopType, asap, secCtrl, number, memoryAddress,
        _memory.toAbsolute(memoryAddress));
}

void BauSystemBDevice::memoryExtWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t * data)
{
    _memory.writeMemory(memoryAddress, number, data);

    applicationLayer().memoryExtWriteResponse(AckRequested, priority, hopType, asap, secCtrl, ReturnCodes::Success, number, memoryAddress, _memory.toAbsolute(memoryAddress));
}

void BauSystemBDevice::memoryExtReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress)
{
    applicationLayer().memoryExtReadResponse(AckRequested, priority, hopType, asap, secCtrl, ReturnCodes::Success, number, memoryAddress, _memory.toAbsolute(memoryAddress));
}

void BauSystemBDevice::restartRequestIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, RestartType restartType, EraseCode eraseCode, uint8_t channel)
{
    if (restartType == RestartType::BasicRestart)
    {
        println("Basic restart requested");
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
        println("Unhandled restart type.");
        _platform.fatalError();
    }

    // Flush the EEPROM before resetting
    _memory.writeMemory();
    _platform.restart();
}

void BauSystemBDevice::authorizeIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint32_t key)
{
    applicationLayer().authorizeResponse(AckRequested, priority, hopType, asap, secCtrl, 0);
}

void BauSystemBDevice::userMemoryReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress)
{
    applicationLayer().userMemoryReadResponse(AckRequested, priority, hopType, asap, secCtrl, number, memoryAddress,
        _memory.toAbsolute(memoryAddress));
}

void BauSystemBDevice::userMemoryWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* data)
{
    _memory.writeMemory(memoryAddress, number, data);

    if (_deviceObj.verifyMode())
        userMemoryReadIndication(priority, hopType, asap, secCtrl, number, memoryAddress);
}

void BauSystemBDevice::propertyDescriptionReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
    uint8_t propertyId, uint8_t propertyIndex)
{
    uint8_t pid = propertyId;
    bool writeEnable = false;
    uint8_t type = 0;
    uint16_t numberOfElements = 0;
    uint8_t access = 0;
    InterfaceObject* obj = getInterfaceObject(objectIndex);
    if (obj)
        obj->readPropertyDescription(pid, propertyIndex, writeEnable, type, numberOfElements, access);

    applicationLayer().propertyDescriptionReadResponse(AckRequested, priority, hopType, asap, secCtrl, objectIndex, pid, propertyIndex,
        writeEnable, type, numberOfElements, access);
}

void BauSystemBDevice::propertyValueWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
    uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t* data, uint8_t length)
{
    InterfaceObject* obj = getInterfaceObject(objectIndex);
    if(obj)
        obj->writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
    propertyValueReadIndication(priority, hopType, asap, secCtrl, objectIndex, propertyId, numberOfElements, startIndex);
}

void BauSystemBDevice::propertyValueExtWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
    uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t* data, uint8_t length, bool confirmed)
{
    uint8_t returnCode = ReturnCodes::Success;

    InterfaceObject* obj = getInterfaceObject(objectType, objectInstance);
    if(obj)
        obj->writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
    else
        returnCode = ReturnCodes::AddressVoid;

    if (confirmed)
    {
        applicationLayer().propertyValueExtWriteConResponse(AckRequested, priority, hopType, asap, secCtrl, objectType, objectInstance, propertyId, numberOfElements, startIndex, returnCode);
    }
}

void BauSystemBDevice::propertyValueReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
    uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex)
{
    uint8_t size = 0;
    uint8_t elementCount = numberOfElements;
    InterfaceObject* obj = getInterfaceObject(objectIndex);
    if (obj)
    {
        uint8_t elementSize = obj->propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of property array entry 0 which contains the current number of elements
    }
    else
        elementCount = 0;

    uint8_t data[size];
    if(obj)
        obj->readProperty((PropertyID)propertyId, startIndex, elementCount, data);
    
    if (elementCount == 0)
        size = 0;
    
    applicationLayer().propertyValueReadResponse(AckRequested, priority, hopType, asap, secCtrl, objectIndex, propertyId, elementCount,
                                        startIndex, data, size);
}

void BauSystemBDevice::propertyValueExtReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
    uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex)
{
    uint8_t size = 0;
    uint8_t elementCount = numberOfElements;
    InterfaceObject* obj = getInterfaceObject(objectType, objectInstance);
    if (obj)
    {
        uint8_t elementSize = obj->propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of propert array entry 0 which is the size
    }
    else
        elementCount = 0;

    uint8_t data[size];
    if(obj)
        obj->readProperty((PropertyID)propertyId, startIndex, elementCount, data);

    if (elementCount == 0)
        size = 0;

    applicationLayer().propertyValueExtReadResponse(AckRequested, priority, hopType, asap, secCtrl, objectType, objectInstance, propertyId, elementCount,
                                           startIndex, data, size);
}

void BauSystemBDevice::functionPropertyCommandIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                                   uint8_t propertyId, uint8_t* data, uint8_t length)
{
    uint8_t resultData[kFunctionPropertyResultBufferMaxSize];
    uint8_t resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer

    bool handled = false;

    InterfaceObject* obj = getInterfaceObject(objectIndex);
    if(obj)
    {
        if (obj->property((PropertyID)propertyId)->Type() == PDT_FUNCTION)
        {
            obj->command((PropertyID)propertyId, data, length, resultData, resultLength);
            handled = true;
        }
        else
        {
            if(_functionProperty != 0)
                if(_functionProperty(objectIndex, propertyId, length, data, resultData, resultLength))
                    handled = true;
        }
    } else {
        if(_functionProperty != 0)
            if(_functionProperty(objectIndex, propertyId, length, data, resultData, resultLength))
                handled = true;
    }

    //only return a value it was handled by a property or function
    if(handled)
        applicationLayer().functionPropertyStateResponse(AckRequested, priority, hopType, asap, secCtrl, objectIndex, propertyId, resultData, resultLength);
}

void BauSystemBDevice::functionPropertyStateIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                                 uint8_t propertyId, uint8_t* data, uint8_t length)
{
    uint8_t resultData[kFunctionPropertyResultBufferMaxSize];
    uint8_t resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer

    bool handled = true;

    InterfaceObject* obj = getInterfaceObject(objectIndex);
    if(obj)
    {
        if (obj->property((PropertyID)propertyId)->Type() == PDT_FUNCTION)
        {
            obj->state((PropertyID)propertyId, data, length, resultData, resultLength);
            handled = true;
        }
        else
        {
            if(_functionPropertyState != 0)
                if(_functionPropertyState(objectIndex, propertyId, length, data, resultData, resultLength))
                    handled = true;
        }
    } else {
        if(_functionPropertyState != 0)
            if(_functionPropertyState(objectIndex, propertyId, length, data, resultData, resultLength))
                handled = true;
    }

    //only return a value it was handled by a property or function
    if(handled)
        applicationLayer().functionPropertyStateResponse(AckRequested, priority, hopType, asap, secCtrl, objectIndex, propertyId, resultData, resultLength);
}

void BauSystemBDevice::functionPropertyExtCommandIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
                                                      uint8_t propertyId, uint8_t* data, uint8_t length)
{
    uint8_t resultData[kFunctionPropertyResultBufferMaxSize];
    uint8_t resultLength = 1; // we always have to include the return code at least

    InterfaceObject* obj = getInterfaceObject(objectType, objectInstance);
    if(obj)
    {
        PropertyDataType propType = obj->property((PropertyID)propertyId)->Type();

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
                obj->command((PropertyID)propertyId, data, length, resultData, resultLength);
                // resultLength was modified by the callee
            }
        }
        else if (propType == PDT_CONTROL)
        {
            uint8_t count = 1;
            // write the event
            obj->writeProperty((PropertyID)propertyId, 1, data, count);
            if (count == 1)
            {
                // Read the current state (one byte only) for the response
                obj->readProperty((PropertyID)propertyId, 1, count, &resultData[1]);
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
    }
    else
    {
        resultData[0] = ReturnCodes::GenericError;
    }

    applicationLayer().functionPropertyExtStateResponse(AckRequested, priority, hopType, asap, secCtrl, objectType, objectInstance, propertyId, resultData, resultLength);
}

void BauSystemBDevice::functionPropertyExtStateIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
                                                    uint8_t propertyId, uint8_t* data, uint8_t length)
{
    uint8_t resultData[kFunctionPropertyResultBufferMaxSize];
    uint8_t resultLength = sizeof(resultData); // tell the callee the maximum size of the buffer

    InterfaceObject* obj = getInterfaceObject(objectType, objectInstance);
    if(obj)
    {
        PropertyDataType propType = obj->property((PropertyID)propertyId)->Type();

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
                obj->state((PropertyID)propertyId, data, length, resultData, resultLength);
                // resultLength was modified by the callee
            }
        }
        else if (propType == PDT_CONTROL)
        {
            uint8_t count = 1;
            // Read the current state (one byte only) for the response
            obj->readProperty((PropertyID)propertyId, 1, count, &resultData[1]);
            resultLength = count ? 2 : 1;
            resultData[0] = count ? ReturnCodes::Success : ReturnCodes::DataVoid;
        }
        else
        {
            resultData[0] = ReturnCodes::DataTypeConflict;
        }
    }
    else
    {
        resultData[0] = ReturnCodes::GenericError;
    }

    applicationLayer().functionPropertyExtStateResponse(AckRequested, priority, hopType, asap, secCtrl, objectType, objectInstance, propertyId, resultData, resultLength);
}

void BauSystemBDevice::individualAddressReadIndication(HopCountType hopType, const SecurityControl &secCtrl)
{
    if (_deviceObj.progMode())
        applicationLayer().individualAddressReadResponse(AckRequested, hopType, secCtrl);
}

void BauSystemBDevice::individualAddressWriteIndication(HopCountType hopType, const SecurityControl &secCtrl, uint16_t newaddress)
{
    if (_deviceObj.progMode())
        _deviceObj.individualAddress(newaddress);
}

void BauSystemBDevice::individualAddressSerialNumberWriteIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint16_t newIndividualAddress,
                                                          uint8_t* knxSerialNumber)
{
    // If the received serial number matches our serial number
    // then store the received new individual address in the device object
    if (!memcmp(knxSerialNumber, _deviceObj.propertyData(PID_SERIAL_NUMBER), 6))
        _deviceObj.individualAddress(newIndividualAddress);
}

void BauSystemBDevice::individualAddressSerialNumberReadIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t* knxSerialNumber)
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

void BauSystemBDevice::addSaveRestore(SaveRestore* obj)
{
    _memory.addSaveRestore(obj);
}

bool BauSystemBDevice::restartRequest(uint16_t asap, const SecurityControl secCtrl)
{
    if (applicationLayer().isConnected())
        return false;
    _restartState = Connecting; // order important, has to be set BEFORE connectRequest
    _restartSecurity = secCtrl;
    applicationLayer().connectRequest(asap, SystemPriority);
    applicationLayer().deviceDescriptorReadRequest(AckRequested, SystemPriority, NetworkLayerParameter, asap, secCtrl, 0);
    return true;
}

void BauSystemBDevice::connectConfirm(uint16_t tsap)
{
    if (_restartState == Connecting)
    {
        /* restart connection is confirmed, go to the next state */
        _restartState = Connected;
        _restartDelay = millis();
    }
    else
    {
        _restartState = Idle;
    }
}

void BauSystemBDevice::nextRestartState()
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
            if (millis() - _restartDelay > 30)
            {
                applicationLayer().restartRequest(AckRequested, SystemPriority, NetworkLayerParameter, _restartSecurity);
                _restartState = Restarted;
                _restartDelay = millis();
            }
            break;
        case Restarted:
            /* restart is finished, we send a disconnect */
            if (millis() - _restartDelay > 30)
            {
                applicationLayer().disconnectRequest(SystemPriority);
                _restartState = Idle;
            }
        default:
            break;
    }
}

void BauSystemBDevice::systemNetworkParameterReadIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint16_t objectType,
                                                      uint16_t propertyId, uint8_t* testInfo, uint16_t testInfoLength)
{
    uint8_t operand;

    popByte(operand, testInfo + 1); // First byte (+ 0) contains only 4 reserved bits (0)

    // See KNX spec. 3.5.2 p.33 (Management Procedures: Procedures with A_SystemNetworkParameter_Read)
    switch((NmReadSerialNumberType)operand)
    {
        case NM_Read_SerialNumber_By_ProgrammingMode: // NM_Read_SerialNumber_By_ProgrammingMode
            // Only send a reply if programming mode is on
            if (_deviceObj.progMode() && (objectType == OT_DEVICE) && (propertyId == PID_SERIAL_NUMBER))
            {
                // Send reply. testResult data is KNX serial number
                applicationLayer().systemNetworkParameterReadResponse(priority, hopType, secCtrl, objectType, propertyId,
                                                             testInfo, testInfoLength, (uint8_t*)_deviceObj.propertyData(PID_SERIAL_NUMBER), 6);
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

void BauSystemBDevice::systemNetworkParameterReadLocalConfirm(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint16_t objectType,
                                                         uint16_t propertyId, uint8_t* testInfo, uint16_t testInfoLength, bool status)
{
}

void BauSystemBDevice::propertyValueRead(ObjectType objectType, uint8_t objectInstance, uint8_t propertyId,
                                   uint8_t &numberOfElements, uint16_t startIndex,
                                   uint8_t **data, uint32_t &length)
{
    uint32_t size = 0;
    uint8_t elementCount = numberOfElements;

    InterfaceObject* obj = getInterfaceObject(objectType, objectInstance);

    if (obj)
    {
        uint8_t elementSize = obj->propertySize((PropertyID)propertyId);
        if (startIndex > 0)
            size = elementSize * numberOfElements;
        else
            size = sizeof(uint16_t); // size of property array entry 0 which contains the current number of elements
        *data = new uint8_t [size];
        obj->readProperty((PropertyID)propertyId, startIndex, elementCount, *data);
    }
    else
    {
        elementCount = 0;
        *data = nullptr;
    }

    numberOfElements = elementCount;
    length = size;
}

void BauSystemBDevice::propertyValueWrite(ObjectType objectType, uint8_t objectInstance, uint8_t propertyId,
                                    uint8_t &numberOfElements, uint16_t startIndex,
                                    uint8_t* data, uint32_t length)
{
    InterfaceObject* obj =  getInterfaceObject(objectType, objectInstance);
    if(obj)
        obj->writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
    else 
        numberOfElements = 0;
}

Memory& BauSystemBDevice::memory()
{
    return _memory;
}

void BauSystemBDevice::versionCheckCallback(VersionCheckCallback func)
{
    _memory.versionCheckCallback(func);
}

VersionCheckCallback BauSystemBDevice::versionCheckCallback()
{
    return _memory.versionCheckCallback();
}

void BauSystemBDevice::beforeRestartCallback(BeforeRestartCallback func)
{
    _beforeRestart = func;
}

BeforeRestartCallback BauSystemBDevice::beforeRestartCallback()
{
    return _beforeRestart;
}

void BauSystemBDevice::functionPropertyCallback(FunctionPropertyCallback func)
{
    _functionProperty = func;
}

FunctionPropertyCallback BauSystemBDevice::functionPropertyCallback()
{
    return _functionProperty;
}
void BauSystemBDevice::functionPropertyStateCallback(FunctionPropertyCallback func)
{
    _functionPropertyState = func;
}

FunctionPropertyCallback BauSystemBDevice::functionPropertyStateCallback()
{
    return _functionPropertyState;
}

void BauSystemBDevice::groupValueReadResponseConfirm(AckType ack, uint16_t asap, Priority priority, HopCountType hopTtype, const SecurityControl &secCtrl, uint8_t* data, uint8_t dataLength, bool status)
{
}

void BauSystemBDevice::individualAddressWriteLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint16_t newaddress, bool status)
{
}

void BauSystemBDevice::individualAddressReadLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, bool status)
{
}

void BauSystemBDevice::individualAddressReadResponseConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, bool status)
{
}

void BauSystemBDevice::individualAddressReadAppLayerConfirm(HopCountType hopType, const SecurityControl &secCtrl, uint16_t individualAddress)
{
}

void BauSystemBDevice::individualAddressSerialNumberReadLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint8_t* serialNumber, bool status)
{
}

void BauSystemBDevice::individualAddressSerialNumberReadResponseConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint8_t* serialNumber, uint16_t domainAddress, bool status)
{
}

void BauSystemBDevice::individualAddressSerialNumberReadAppLayerConfirm(HopCountType hopType, const SecurityControl &secCtrl, uint8_t* serialNumber, uint16_t individualAddress, uint16_t domainAddress)
{
}

void BauSystemBDevice::individualAddressSerialNumberWriteLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint8_t* serialNumber, uint16_t newaddress, bool status)
{
}

void BauSystemBDevice::deviceDescriptorReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptorType, bool status)
{
}

void BauSystemBDevice::deviceDescriptorReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptor_type,
                                                        uint8_t* device_descriptor, bool status)
{
}

void BauSystemBDevice::deviceDescriptorReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptortype, uint8_t* deviceDescriptor)
{
}

void BauSystemBDevice::restartRequestLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, bool status)
{
}

void BauSystemBDevice::propertyValueReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, bool status)
{
}

void BauSystemBDevice::propertyValueReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t* data, uint8_t length, bool status)
{
}

void BauSystemBDevice::propertyValueReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t* data, uint8_t length)
{
}

void BauSystemBDevice::propertyValueWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t* data, uint8_t length, bool status)
{
}

void BauSystemBDevice::propertyDescriptionReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool status)
{
}

void BauSystemBDevice::propertyDescriptionReadResponse(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool writeEnable, uint8_t type, uint16_t maxNumberOfElements, uint8_t access)
{
}

void BauSystemBDevice::propertyDescriptionReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool writeEnable, uint8_t type, uint16_t maxNumberOfElements, uint8_t access, bool status)
{
}

void BauSystemBDevice::propertyDescriptionReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool writeEnable, uint8_t type, uint16_t maxNumberOfElements, uint8_t access)
{
}

void BauSystemBDevice::memoryReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint16_t memoryAddress, bool status)
{
}

void BauSystemBDevice::memoryReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint16_t memoryAddress, uint8_t* data, bool status)
{
}

void BauSystemBDevice::memoryReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint16_t memoryAddress, uint8_t* data)
{
}

void BauSystemBDevice::memoryWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint16_t memoryAddress, uint8_t* data, bool status)
{
}

void BauSystemBDevice::memoryExtReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, bool status)
{
}

void BauSystemBDevice::memoryExtReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* data, bool status)
{
}

void BauSystemBDevice::memoryExtReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* data)
{
}

void BauSystemBDevice::memoryExtWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* data, bool status)
{
}

void BauSystemBDevice::memoryExtWriteResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* data, bool status)
{
}

void BauSystemBDevice::memoryExtWriteAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* data)
{
}

void BauSystemBDevice::userMemoryReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, bool status)
{
}

void BauSystemBDevice::userMemoryReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* memoryData, bool status)
{
}

void BauSystemBDevice::userMemoryReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* memoryData)
{
}

void BauSystemBDevice::userMemoryWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* memoryData, bool status)
{
}

void BauSystemBDevice::userManufacturerInfoLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, bool status)
{
}

void BauSystemBDevice::userManufacturerInfoIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl)
{
}

void BauSystemBDevice::userManufacturerInfoResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t* info, bool status)
{
}

void BauSystemBDevice::userManufacturerInfoAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t* info)
{
}

void BauSystemBDevice::authorizeLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint32_t key, bool status)
{
}

void BauSystemBDevice::authorizeResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level, bool status)
{
}

void BauSystemBDevice::authorizeAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level)
{
}

void BauSystemBDevice::keyWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level, uint32_t key, bool status)
{
}

void BauSystemBDevice::keyWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level, uint32_t key)
{
}

void BauSystemBDevice::keyWriteResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level, bool status)
{
}

void BauSystemBDevice::keyWriteAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level)
{
}

void BauSystemBDevice::domainAddressSerialNumberWriteIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t* rfDoA,
                                                             const uint8_t* knxSerialNumber)
{
}

void BauSystemBDevice::domainAddressSerialNumberReadIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t* knxSerialNumber)
{
}

void BauSystemBDevice::domainAddressSerialNumberWriteLocalConfirm(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t* rfDoA,
                                                             const uint8_t* knxSerialNumber, bool status)
{
}

void BauSystemBDevice::domainAddressSerialNumberReadLocalConfirm(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t* knxSerialNumber, bool status)
{
}


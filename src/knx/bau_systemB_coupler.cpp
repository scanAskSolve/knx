#include "bau_systemB_coupler.h"
#include "bits.h"
#include <string.h>
#include <stdio.h>

BauSystemBCoupler::BauSystemBCoupler(ArduinoPlatform& platform)
    : _memory(platform, _deviceObj),
     _appProgram(_memory),
    _platform(platform),
    _platform(platform),
#ifdef USE_DATASECURE
    _appLayer(_deviceObj, _secIfObj, *this),
#else
    _appLayer(*this),
#endif
    _transLayer(_appLayer),
    _netLayer(_deviceObj, _transLayer)
{
    _appLayer.transportLayer(_transLayer);
    _transLayer.networkLayer(_netLayer);
    _memory.addSaveRestore(&_deviceObj);
    _memory.addSaveRestore(&_appProgram);
#ifdef USE_DATASECURE
    _memory.addSaveRestore(&_secIfObj);
#endif
}

ApplicationLayer& BauSystemBCoupler::applicationLayer()
{
    return _appLayer;
}

void BauSystemBCoupler::loop()
{
    _transLayer.loop();
#ifdef USE_DATASECURE
    _appLayer.loop();
#endif
}

bool BauSystemBCoupler::configured()
{
    // _configured is set to true initially, if the device was configured with ETS it will be set to true after restart
    
    if (!_configured)
        return false;
    
    _configured = _appProgram.loadState() == LS_LOADED;
#ifdef USE_DATASECURE
    _configured &= _secIfObj.loadState() == LS_LOADED;
#endif
    
    return _configured;
}

void BauSystemBCoupler::doMasterReset(EraseCode eraseCode, uint8_t channel)
{
    doMasterReset(eraseCode, channel);

#ifdef USE_DATASECURE
    _secIfObj.masterReset(eraseCode, channel);
#endif
}

void BauSystemBCoupler::readMemory()
{
    _memory.readMemory();
}

void BauSystemBCoupler::writeMemory()
{
    _memory.writeMemory();
}

ArduinoPlatform& BauSystemBCoupler::platform()
{
    return _platform;
}

ApplicationProgramObject& BauSystemBCoupler::parameters()
{
    return _appProgram;
}

DeviceObject& BauSystemBCoupler::deviceObject()
{
    return _deviceObj;
}

uint8_t BauSystemBCoupler::checkmasterResetValidity(EraseCode eraseCode, uint8_t channel)
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

void BauSystemBCoupler::deviceDescriptorReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptorType)
{
    if (descriptorType != 0)
        descriptorType = 0x3f;
    
    uint8_t data[2];
    pushWord(_deviceObj.maskVersion(), data);
    applicationLayer().deviceDescriptorReadResponse(AckRequested, priority, hopType, asap, secCtrl, descriptorType, data);
}

void BauSystemBCoupler::memoryWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
    uint16_t memoryAddress, uint8_t * data)
{
    _memory.writeMemory(memoryAddress, number, data);
    if (_deviceObj.verifyMode())
        memoryReadIndication(priority, hopType, asap, secCtrl, number, memoryAddress, data);
}

void BauSystemBCoupler::memoryReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
    uint16_t memoryAddress, uint8_t * data)
{
    applicationLayer().memoryReadResponse(AckRequested, priority, hopType, asap, secCtrl, number, memoryAddress, data);
}

void BauSystemBCoupler::memoryReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
    uint16_t memoryAddress)
{
    applicationLayer().memoryReadResponse(AckRequested, priority, hopType, asap, secCtrl, number, memoryAddress,
        _memory.toAbsolute(memoryAddress));
}

void BauSystemBCoupler::memoryExtWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t * data)
{
    _memory.writeMemory(memoryAddress, number, data);

    applicationLayer().memoryExtWriteResponse(AckRequested, priority, hopType, asap, secCtrl, ReturnCodes::Success, number, memoryAddress, _memory.toAbsolute(memoryAddress));
}

void BauSystemBCoupler::memoryExtReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress)
{
    applicationLayer().memoryExtReadResponse(AckRequested, priority, hopType, asap, secCtrl, ReturnCodes::Success, number, memoryAddress, _memory.toAbsolute(memoryAddress));
}

void BauSystemBCoupler::doMasterReset(EraseCode eraseCode, uint8_t channel)
{
    _deviceObj.masterReset(eraseCode, channel);
    _appProgram.masterReset(eraseCode, channel);
}

void BauSystemBCoupler::restartRequestIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, RestartType restartType, EraseCode eraseCode, uint8_t channel)
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

void BauSystemBCoupler::authorizeIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint32_t key)
{
    applicationLayer().authorizeResponse(AckRequested, priority, hopType, asap, secCtrl, 0);
}

void BauSystemBCoupler::userMemoryReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress)
{
    applicationLayer().userMemoryReadResponse(AckRequested, priority, hopType, asap, secCtrl, number, memoryAddress,
        _memory.toAbsolute(memoryAddress));
}

void BauSystemBCoupler::userMemoryWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* data)
{
    _memory.writeMemory(memoryAddress, number, data);

    if (_deviceObj.verifyMode())
        userMemoryReadIndication(priority, hopType, asap, secCtrl, number, memoryAddress);
}

void BauSystemBCoupler::propertyDescriptionReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
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

void BauSystemBCoupler::propertyValueWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
    uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t* data, uint8_t length)
{
    InterfaceObject* obj = getInterfaceObject(objectIndex);
    if(obj)
        obj->writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
    propertyValueReadIndication(priority, hopType, asap, secCtrl, objectIndex, propertyId, numberOfElements, startIndex);
}

void BauSystemBCoupler::propertyValueExtWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
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

void BauSystemBCoupler::propertyValueReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
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

void BauSystemBCoupler::propertyValueExtReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
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

void BauSystemBCoupler::functionPropertyCommandIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
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

void BauSystemBCoupler::functionPropertyStateIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
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

void BauSystemBCoupler::functionPropertyExtCommandIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
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

void BauSystemBCoupler::functionPropertyExtStateIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
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

void BauSystemBCoupler::individualAddressReadIndication(HopCountType hopType, const SecurityControl &secCtrl)
{
    if (_deviceObj.progMode())
        applicationLayer().individualAddressReadResponse(AckRequested, hopType, secCtrl);
}

void BauSystemBCoupler::individualAddressWriteIndication(HopCountType hopType, const SecurityControl &secCtrl, uint16_t newaddress)
{
    if (_deviceObj.progMode())
        _deviceObj.individualAddress(newaddress);
}

void BauSystemBCoupler::individualAddressSerialNumberWriteIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint16_t newIndividualAddress,
                                                          uint8_t* knxSerialNumber)
{
    // If the received serial number matches our serial number
    // then store the received new individual address in the device object
    if (!memcmp(knxSerialNumber, _deviceObj.propertyData(PID_SERIAL_NUMBER), 6))
        _deviceObj.individualAddress(newIndividualAddress);
}

void BauSystemBCoupler::individualAddressSerialNumberReadIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t* knxSerialNumber)
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

void BauSystemBCoupler::addSaveRestore(SaveRestore* obj)
{
    _memory.addSaveRestore(obj);
}

bool BauSystemBCoupler::restartRequest(uint16_t asap, const SecurityControl secCtrl)
{
    if (applicationLayer().isConnected())
        return false;
    _restartState = Connecting; // order important, has to be set BEFORE connectRequest
    _restartSecurity = secCtrl;
    applicationLayer().connectRequest(asap, SystemPriority);
    applicationLayer().deviceDescriptorReadRequest(AckRequested, SystemPriority, NetworkLayerParameter, asap, secCtrl, 0);
    return true;
}

void BauSystemBCoupler::connectConfirm(uint16_t tsap)
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

void BauSystemBCoupler::nextRestartState()
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

void BauSystemBCoupler::systemNetworkParameterReadIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint16_t objectType,
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

void BauSystemBCoupler::systemNetworkParameterReadLocalConfirm(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint16_t objectType,
                                                         uint16_t propertyId, uint8_t* testInfo, uint16_t testInfoLength, bool status)
{
}

void BauSystemBCoupler::propertyValueRead(ObjectType objectType, uint8_t objectInstance, uint8_t propertyId,
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

void BauSystemBCoupler::propertyValueWrite(ObjectType objectType, uint8_t objectInstance, uint8_t propertyId,
                                    uint8_t &numberOfElements, uint16_t startIndex,
                                    uint8_t* data, uint32_t length)
{
    InterfaceObject* obj =  getInterfaceObject(objectType, objectInstance);
    if(obj)
        obj->writeProperty((PropertyID)propertyId, startIndex, data, numberOfElements);
    else 
        numberOfElements = 0;
}

Memory& BauSystemBCoupler::memory()
{
    return _memory;
}

void BauSystemBCoupler::versionCheckCallback(VersionCheckCallback func)
{
    _memory.versionCheckCallback(func);
}

VersionCheckCallback BauSystemBCoupler::versionCheckCallback()
{
    return _memory.versionCheckCallback();
}

void BauSystemBCoupler::beforeRestartCallback(BeforeRestartCallback func)
{
    _beforeRestart = func;
}

BeforeRestartCallback BauSystemBCoupler::beforeRestartCallback()
{
    return _beforeRestart;
}

void BauSystemBCoupler::functionPropertyCallback(FunctionPropertyCallback func)
{
    _functionProperty = func;
}

FunctionPropertyCallback BauSystemBCoupler::functionPropertyCallback()
{
    return _functionProperty;
}
void BauSystemBCoupler::functionPropertyStateCallback(FunctionPropertyCallback func)
{
    _functionPropertyState = func;
}

FunctionPropertyCallback BauSystemBCoupler::functionPropertyStateCallback()
{
    return _functionPropertyState;
}


void BauSystemBCoupler::groupValueReadLocalConfirm(AckType ack, uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl& secCtrl, bool status)
{
}

void BauSystemBCoupler::groupValueReadIndication(uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl)
{
}

void BauSystemBCoupler::groupValueReadResponseConfirm(AckType ack, uint16_t asap, Priority priority, HopCountType hopTtype, const SecurityControl &secCtrl, uint8_t* data, uint8_t dataLength, bool status)
{
}

void BauSystemBCoupler::groupValueReadAppLayerConfirm(uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t* data, uint8_t dataLength)
{
}

void BauSystemBCoupler::groupValueWriteLocalConfirm(AckType ack, uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t* data, uint8_t dataLength, bool status)
{
}

void BauSystemBCoupler::groupValueWriteIndication(uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t* data, uint8_t dataLength)
{
}

void BauSystemBCoupler::individualAddressWriteLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint16_t newaddress, bool status)
{
}

void BauSystemBCoupler::individualAddressReadLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, bool status)
{
}

void BauSystemBCoupler::individualAddressReadResponseConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, bool status)
{
}

void BauSystemBCoupler::individualAddressReadAppLayerConfirm(HopCountType hopType, const SecurityControl &secCtrl, uint16_t individualAddress)
{
}

void BauSystemBCoupler::individualAddressSerialNumberReadLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint8_t* serialNumber, bool status)
{
}

void BauSystemBCoupler::individualAddressSerialNumberReadResponseConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint8_t* serialNumber, uint16_t domainAddress, bool status)
{
}

void BauSystemBCoupler::individualAddressSerialNumberReadAppLayerConfirm(HopCountType hopType, const SecurityControl &secCtrl, uint8_t* serialNumber, uint16_t individualAddress, uint16_t domainAddress)
{
}

void BauSystemBCoupler::individualAddressSerialNumberWriteLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint8_t* serialNumber, uint16_t newaddress, bool status)
{
}

void BauSystemBCoupler::deviceDescriptorReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptorType, bool status)
{
}

void BauSystemBCoupler::deviceDescriptorReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptor_type,
                                                        uint8_t* device_descriptor, bool status)
{
}

void BauSystemBCoupler::deviceDescriptorReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptortype, uint8_t* deviceDescriptor)
{
}

void BauSystemBCoupler::restartRequestLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, bool status)
{
}

void BauSystemBCoupler::propertyValueReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, bool status)
{
}

void BauSystemBCoupler::propertyValueReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t* data, uint8_t length, bool status)
{
}

void BauSystemBCoupler::propertyValueReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t* data, uint8_t length)
{
}

void BauSystemBCoupler::propertyValueWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t* data, uint8_t length, bool status)
{
}

void BauSystemBCoupler::propertyDescriptionReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool status)
{
}

void BauSystemBCoupler::propertyDescriptionReadResponse(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool writeEnable, uint8_t type, uint16_t maxNumberOfElements, uint8_t access)
{
}

void BauSystemBCoupler::propertyDescriptionReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool writeEnable, uint8_t type, uint16_t maxNumberOfElements, uint8_t access, bool status)
{
}

void BauSystemBCoupler::propertyDescriptionReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool writeEnable, uint8_t type, uint16_t maxNumberOfElements, uint8_t access)
{
}

void BauSystemBCoupler::memoryReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint16_t memoryAddress, bool status)
{
}

void BauSystemBCoupler::memoryReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint16_t memoryAddress, uint8_t* data, bool status)
{
}

void BauSystemBCoupler::memoryReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint16_t memoryAddress, uint8_t* data)
{
}

void BauSystemBCoupler::memoryWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint16_t memoryAddress, uint8_t* data, bool status)
{
}

void BauSystemBCoupler::memoryExtReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, bool status)
{
}

void BauSystemBCoupler::memoryExtReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* data, bool status)
{
}

void BauSystemBCoupler::memoryExtReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* data)
{
}

void BauSystemBCoupler::memoryExtWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* data, bool status)
{
}

void BauSystemBCoupler::memoryExtWriteResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* data, bool status)
{
}

void BauSystemBCoupler::memoryExtWriteAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* data)
{
}

void BauSystemBCoupler::userMemoryReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, bool status)
{
}

void BauSystemBCoupler::userMemoryReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* memoryData, bool status)
{
}

void BauSystemBCoupler::userMemoryReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* memoryData)
{
}

void BauSystemBCoupler::userMemoryWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress, uint8_t* memoryData, bool status)
{
}

void BauSystemBCoupler::userManufacturerInfoLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, bool status)
{
}

void BauSystemBCoupler::userManufacturerInfoIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl)
{
}

void BauSystemBCoupler::userManufacturerInfoResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t* info, bool status)
{
}

void BauSystemBCoupler::userManufacturerInfoAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t* info)
{
}

void BauSystemBCoupler::authorizeLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint32_t key, bool status)
{
}

void BauSystemBCoupler::authorizeResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level, bool status)
{
}

void BauSystemBCoupler::authorizeAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level)
{
}

void BauSystemBCoupler::keyWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level, uint32_t key, bool status)
{
}

void BauSystemBCoupler::keyWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level, uint32_t key)
{
}

void BauSystemBCoupler::keyWriteResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level, bool status)
{
}

void BauSystemBCoupler::keyWriteAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level)
{
}

void BauSystemBCoupler::domainAddressSerialNumberWriteIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t* rfDoA,
                                                             const uint8_t* knxSerialNumber)
{
}

void BauSystemBCoupler::domainAddressSerialNumberReadIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t* knxSerialNumber)
{
}

void BauSystemBCoupler::domainAddressSerialNumberWriteLocalConfirm(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t* rfDoA,
                                                             const uint8_t* knxSerialNumber, bool status)
{
}

void BauSystemBCoupler::domainAddressSerialNumberReadLocalConfirm(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t* knxSerialNumber, bool status)
{
}


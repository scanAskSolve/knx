#pragma once

#include "config.h"
#include "bau_systemB.h"
#include "device_object.h"
#include "security_interface_object.h"
#include "application_program_object.h"
#include "router_object.h"
#include "application_layer.h"
#include "secure_application_layer.h"
#include "transport_layer.h"
#include "network_layer_coupler.h"
#include "data_link_layer.h"
#include "arduino_platform.h"
#include "memory.h"

// class BauSystemBCoupler : public BauSystemB
class BauSystemBCoupler{
  public:
    BauSystemBCoupler(ArduinoPlatform& platform);
    virtual void loop();
    virtual bool configured();

  protected:
    virtual ApplicationLayer& applicationLayer();

    virtual void doMasterReset(EraseCode eraseCode, uint8_t channel);

    ArduinoPlatform& _platform;

#ifdef USE_DATASECURE
    SecureApplicationLayer _appLayer;
    SecurityInterfaceObject _secIfObj;
#else
    ApplicationLayer _appLayer;
#endif
    TransportLayer _transLayer;
    NetworkLayerCoupler _netLayer;
    bool _configured = true;
  
  public:
    virtual bool enabled();
    virtual void enabled(bool value);

    ArduinoPlatform& platform();
    ApplicationProgramObject& parameters();
    DeviceObject& deviceObject();

    Memory& memory();
    void readMemory();
    void writeMemory();
    void addSaveRestore(SaveRestore* obj);

    bool restartRequest(uint16_t asap, const SecurityControl secCtrl);
    uint8_t checkmasterResetValidity(EraseCode eraseCode, uint8_t channel);

    virtual void propertyValueRead(ObjectType objectType, uint8_t objectInstance, uint8_t propertyId,
                           uint8_t& numberOfElements, uint16_t startIndex, 
                           uint8_t **data, uint32_t &length);
    virtual void propertyValueWrite(ObjectType objectType, uint8_t objectInstance, uint8_t propertyId,
                            uint8_t& numberOfElements, uint16_t startIndex,
                            uint8_t* data, uint32_t length);
    void versionCheckCallback(VersionCheckCallback func);
    VersionCheckCallback versionCheckCallback();
    void beforeRestartCallback(BeforeRestartCallback func);
    BeforeRestartCallback beforeRestartCallback();
    void functionPropertyCallback(FunctionPropertyCallback func);
    FunctionPropertyCallback functionPropertyCallback();
    void functionPropertyStateCallback(FunctionPropertyCallback func);
    FunctionPropertyCallback functionPropertyStateCallback();

};

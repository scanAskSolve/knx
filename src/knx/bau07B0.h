#pragma once

#include "config.h"
#if MASK_VERSION == 0x07B0
#include "../arduino_platform.h"
#include "bau_systemB.h"
#include "tpuart_data_link_layer.h"
#include "cemi_server.h"
#include "cemi_server_object.h"
//class Bau07B0 : public BauSystemBDevice, public ITpUartCallBacks
class Bau07B0 : public BauSystemB
{
  public:
    Bau07B0(ArduinoPlatform& platform);
    void loop() override;
    virtual bool enabled();
    virtual void enabled(bool value);
    
  protected:
    InterfaceObject* getInterfaceObject(uint8_t idx);
    InterfaceObject* getInterfaceObject(ObjectType objectType, uint8_t objectInstance);

    // For TP1 only
    virtual bool isAckRequired(uint16_t address, bool isGrpAddr);

  private:
    TpUartDataLinkLayer _dlLayer;
#ifdef USE_CEMI_SERVER
    CemiServer _cemiServer;
    CemiServerObject _cemiServerObject;
#endif                                  
};
#endif

#pragma once

#include <stdint.h>
#include "knx_types.h"
#include "npdu.h"
#include "transport_layer.h"

class DeviceObject;
class APDU;
class RouterObject;
class NetworkLayerEntity;

enum LayerType
{
  coupler,
  device,
  none
};
class NetworkLayer
{

public:
  NetworkLayer(DeviceObject &deviceObj, TransportLayer &layer);
  NetworkLayerEntity &getInterface();
  static constexpr uint8_t kInterfaceIndex = 0;

  enum CouplerType
  {
    LineCoupler,
    BackboneCoupler,
    TP1Bridge,
    TP1Repeater
  };
  LayerType _layerType = none;

  NetworkLayerEntity* _netLayerEntities[2];

  RouterObject* _rtObjPrimary{nullptr};
  RouterObject* _rtObjSecondary{nullptr};

  CouplerType _couplerType;
  uint16_t _currentAddress;

  void routeDataIndividual(AckType ack, uint16_t destination, NPDU &npdu, Priority priority, uint16_t source, uint8_t srcIfIndex);
  void sendMsgHopCount(AckType ack, AddressType addrType, uint16_t destination, NPDU &npdu, Priority priority,
                       SystemBroadcast broadcastType, uint8_t sourceInterfaceIndex, uint16_t source);

  void evaluateCouplerType();
  bool isGroupAddressInFilterTable(uint16_t groupAddress);

  static constexpr uint8_t kPrimaryIfIndex = 0;
  static constexpr uint8_t kSecondaryIfIndex = 1;
  static constexpr uint8_t kLocalIfIndex = 99;

  NetworkLayerEntity &getPrimaryInterface();
  NetworkLayerEntity &getSecondaryInterface();

  bool isRoutedIndividualAddress(uint16_t individualAddress);

  void rtObjPrimary(RouterObject &rtObjPrimary);     // Coupler model 2.0
  void rtObjSecondary(RouterObject &rtObjSecondary); // Coupler model 2.0
  void rtObj(RouterObject &rtObj);                   // Coupler model 1.x

  // NetworkLayer(DeviceObject& deviceObj, TransportLayer& layer,LayerType layerType);

  void setLayerType(LayerType LayerType);
  uint8_t hopCount() const;
  bool isApciSystemBroadcast(APDU &apdu);

  // from transport layer
  void dataIndividualRequest(AckType ack, uint16_t destination, HopCountType hopType, Priority priority, TPDU &tpdu);
  void dataGroupRequest(AckType ack, uint16_t destination, HopCountType hopType, Priority priority, TPDU &tpdu);
  void dataBroadcastRequest(AckType ack, HopCountType hopType, Priority priority, TPDU &tpdu);
  void dataSystemBroadcastRequest(AckType ack, HopCountType hopType, Priority priority, TPDU &tpdu);

  // protected:
  DeviceObject &_deviceObj;
  TransportLayer &_transportLayer;

  // from entities
  void dataIndication(AckType ack, AddressType addType, uint16_t destination, FrameFormat format, NPDU &npdu,
                      Priority priority, uint16_t source, uint8_t srcIfIdx);
  void dataConfirm(AckType ack, AddressType addressType, uint16_t destination, FrameFormat format, Priority priority,
                   uint16_t source, NPDU &npdu, bool status, uint8_t srcIfIdx);
  void broadcastIndication(AckType ack, FrameFormat format, NPDU &npdu,
                           Priority priority, uint16_t source, uint8_t srcIfIdx);
  void broadcastConfirm(AckType ack, FrameFormat format, Priority priority, uint16_t source, NPDU &npdu, bool status, uint8_t srcIfIdx);
  void systemBroadcastIndication(AckType ack, FrameFormat format, NPDU &npdu,
                                 Priority priority, uint16_t source, uint8_t srcIfIdx);
  void systemBroadcastConfirm(AckType ack, FrameFormat format, Priority priority, uint16_t source, NPDU &npdu, bool status, uint8_t srcIfIdx);

  // private:
  uint8_t _hopCount; // Network Layer Parameter hop_count for the device's own outgoing frames (default value from PID_ROUTING_COUNT)
};

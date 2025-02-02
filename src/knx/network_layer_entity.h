#pragma once

#include <stdint.h>
#include "knx_types.h"
#include "npdu.h"
#include "network_layer.h"

class TpUartDataLinkLayer;
class NetworkLayer;

class NetworkLayerEntity
{

public:
  NetworkLayerEntity(NetworkLayer &netLayer, uint8_t entityIndex);

  void dataLinkLayer(TpUartDataLinkLayer &layer);
  TpUartDataLinkLayer &dataLinkLayer();

  DptMedium mediumType() const;

  // from data link layer
  void dataIndication(AckType ack, AddressType addType, uint16_t destination, FrameFormat format, NPDU &npdu,
                      Priority priority, uint16_t source);
  void dataConfirm(AckType ack, AddressType addressType, uint16_t destination, FrameFormat format, Priority priority,
                   uint16_t source, NPDU &npdu, bool status);
  void broadcastIndication(AckType ack, FrameFormat format, NPDU &npdu,
                           Priority priority, uint16_t source);
  void broadcastConfirm(AckType ack, FrameFormat format, Priority priority, uint16_t source, NPDU &npdu, bool status);
  void systemBroadcastIndication(AckType ack, FrameFormat format, NPDU &npdu,
                                 Priority priority, uint16_t source);
  void systemBroadcastConfirm(AckType ack, FrameFormat format, Priority priority, uint16_t source, NPDU &npdu, bool status);

  // private:
  //  From network layer
  void sendDataRequest(NPDU &npdu, AckType ack, uint16_t destination, uint16_t source, Priority priority, AddressType addrType, SystemBroadcast systemBroadcast);

  TpUartDataLinkLayer *_dataLinkLayer = 0;
  NetworkLayer &_netLayer;
  uint8_t _entityIndex;
};

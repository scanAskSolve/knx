#pragma once

#include "config.h"
#ifdef USE_TP

#include "device_object.h"
#include "arduino_platform.h"
#include "knx_types.h"
#include "cemi_frame.h"
#include "network_layer_entity.h"
#include <stdint.h>
// #include "data_link_layer.h"

#define MAX_KNX_TELEGRAM_SIZE 263

class ITpUartCallBacks
{
public:
  virtual ~ITpUartCallBacks() = default;
  virtual bool isAckRequired(uint16_t address, bool isGrpAddr) = 0;
};

class TpUartDataLinkLayer
{

public:
  TpUartDataLinkLayer(DeviceObject &devObj, NetworkLayerEntity &netLayerEntity,
                      ITpUartCallBacks &cb);

  void loop();
  void enabled(bool value);
  bool enabled() const;
  DptMedium mediumType() const;

  DeviceObject &_deviceObject;
  NetworkLayerEntity &_networkLayerEntity;

  void dataRequest(AckType ack, AddressType addrType, uint16_t destinationAddr, uint16_t sourceAddr, FrameFormat format,
                   Priority priority, NPDU &npdu);
  void systemBroadcastRequest(AckType ack, FrameFormat format, Priority priority, NPDU &npdu, uint16_t sourceAddr);
  bool sendTelegram(NPDU &npdu, AckType ack, uint16_t destinationAddr, AddressType addrType, uint16_t sourceAddr, FrameFormat format, Priority priority, SystemBroadcast systemBroadcast);
  void dataConReceived(CemiFrame &frame, bool success);
  void frameReceived(CemiFrame &frame);

private:
  bool _enabled = false;
  uint8_t *_sendBuffer = 0;
  uint16_t _sendBufferLength = 0;
  uint8_t _receiveBuffer[MAX_KNX_TELEGRAM_SIZE];
  uint8_t _txState = 0;
  uint8_t _rxState = 0;
  uint16_t _RxByteCnt = 0;
  uint16_t _TxByteCnt = 0;
  uint8_t _oldIdx = 0;
  bool _isEcho = false;
  bool _convert = false;
  uint8_t _xorSum = 0;
  uint32_t _lastByteRxTime;
  uint32_t _lastByteTxTime;
  uint32_t _lastLoopTime;
  uint32_t _waitConfirmStartTime = 0;
  uint32_t _lastResetChipTime = 0;

  struct _tx_queue_frame_t
  {
    uint8_t *data;
    uint16_t length;
    _tx_queue_frame_t *next;
  };

  struct _tx_queue_t
  {
    _tx_queue_frame_t *front = NULL;
    _tx_queue_frame_t *back = NULL;
  } _tx_queue;

  void addFrameTxQueue(CemiFrame &frame);
  bool isTxQueueEmpty();
  void loadNextTxFrame();
  bool sendSingleFrameByte();
  bool sendFrame(CemiFrame &frame);
  void frameBytesReceived(uint8_t *buffer, uint16_t length);
  void dataConBytesReceived(uint8_t *buffer, uint16_t length, bool success);
  void enterRxWaitEOP();
  bool resetChip();
  bool resetChipTick();
  void stopChip();

  ITpUartCallBacks &_cb;
};
#endif

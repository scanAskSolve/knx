#pragma once

#include "config.h"
// #include "bau.h"
// #include "security_interface_object.h"
#include "association_table_object.h"
#include "application_program_object.h"
#include "application_layer.h"
// #include "secure_application_layer.h"
#include "transport_layer.h"
#include "network_layer.h"
#include "data_link_layer.h"
#include "arduino_platform.h"
#include "memory.h"
#include "knx_types.h"
#include "group_object_table_object.h"
#include "tpuart_data_link_layer.h"

#include "device_object.h"
#include "address_table_object.h"

typedef void (*BeforeRestartCallback)(void);
typedef bool (*FunctionPropertyCallback)(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength);

class BauSystemB
{
public:
  virtual bool isAckRequired(uint16_t address, bool isGrpAddr);
  BauSystemB();

  virtual void loop();
  bool configured();
  virtual bool enabled();
  virtual void enabled(bool value);

  ApplicationProgramObject &parameters();
  DeviceObject &deviceObject();

  Memory &memory();
  void readMemory();
  void writeMemory();
  void addSaveRestore(DeviceObject *obj);

  bool restartRequest(uint16_t asap, const SecurityControl secCtrl);
  uint8_t checkmasterResetValidity(EraseCode eraseCode, uint8_t channel);

  virtual void propertyValueRead(ObjectType objectType, uint8_t objectInstance, uint8_t propertyId,
                                 uint8_t &numberOfElements, uint16_t startIndex,
                                 uint8_t **data, uint32_t &length);
  virtual void propertyValueWrite(ObjectType objectType, uint8_t objectInstance, uint8_t propertyId,
                                  uint8_t &numberOfElements, uint16_t startIndex,
                                  uint8_t *data, uint32_t length);
  void versionCheckCallback(VersionCheckCallback func);
  VersionCheckCallback versionCheckCallback();
  void beforeRestartCallback(BeforeRestartCallback func);
  BeforeRestartCallback beforeRestartCallback();
  void functionPropertyCallback(FunctionPropertyCallback func);
  FunctionPropertyCallback functionPropertyCallback();
  void functionPropertyStateCallback(FunctionPropertyCallback func);
  FunctionPropertyCallback functionPropertyStateCallback();

public:
  ApplicationLayer &applicationLayer();
  // virtual InterfaceObject *getInterfaceObject(uint8_t idx);
  // virtual InterfaceObject *getInterfaceObject(ObjectType objectType, uint8_t objectInstance);

  virtual void memoryWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                     uint16_t memoryAddress, uint8_t *data);
  virtual void memoryReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                    uint16_t memoryAddress);
  virtual void memoryReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                    uint16_t memoryAddress, uint8_t *data);
  virtual void memoryExtWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                        uint32_t memoryAddress, uint8_t *data);
  virtual void memoryExtReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                       uint32_t memoryAddress);
  virtual void deviceDescriptorReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t descriptorType);
  virtual void restartRequestIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, RestartType restartType, EraseCode eraseCode, uint8_t channel);
  virtual void authorizeIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint32_t key);
  virtual void userMemoryReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number, uint32_t memoryAddress);
  virtual void userMemoryWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                         uint32_t memoryAddress, uint8_t *memoryData);
  virtual void propertyDescriptionReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                                 uint8_t propertyId, uint8_t propertyIndex);
  virtual void propertyValueWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                            uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t *data, uint8_t length);
  virtual void propertyValueReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                           uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex);
  virtual void propertyValueExtReadIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
                                              uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex);
  virtual void functionPropertyCommandIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                                 uint8_t propertyId, uint8_t *data, uint8_t length);
  virtual void functionPropertyStateIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                               uint8_t propertyId, uint8_t *data, uint8_t length);
  virtual void functionPropertyExtCommandIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
                                                    uint8_t propertyId, uint8_t *data, uint8_t length);
  virtual void functionPropertyExtStateIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
                                                  uint8_t propertyId, uint8_t *data, uint8_t length);
  virtual void individualAddressReadIndication(HopCountType hopType, const SecurityControl &secCtrl);
  virtual void individualAddressWriteIndication(HopCountType hopType, const SecurityControl &secCtrl, uint16_t newaddress);
  virtual void individualAddressSerialNumberWriteIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint16_t newIndividualAddress,
                                                            uint8_t *knxSerialNumber);
  virtual void individualAddressSerialNumberReadIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint8_t *knxSerialNumber);
  virtual void systemNetworkParameterReadIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint16_t objectType,
                                                    uint16_t propertyId, uint8_t *testInfo, uint16_t testinfoLength);
  virtual void systemNetworkParameterReadLocalConfirm(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, uint16_t objectType,
                                                      uint16_t propertyId, uint8_t *testInfo, uint16_t testInfoLength, bool status);
  virtual void connectConfirm(uint16_t tsap);

  void nextRestartState();
  virtual void doMasterReset(EraseCode eraseCode, uint8_t channel);

  enum RestartState
  {
    Idle,
    Connecting,
    Connected,
    Restarted
  };

  Memory _memory;
  DeviceObject _deviceObj;
  ApplicationProgramObject _appProgram;
  RestartState _restartState = Idle;
  SecurityControl _restartSecurity;
  uint32_t _restartDelay = 0;
  BeforeRestartCallback _beforeRestart = 0;
  FunctionPropertyCallback _functionProperty = 0;
  FunctionPropertyCallback _functionPropertyState = 0;

  virtual void propertyValueExtWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, ObjectType objectType, uint8_t objectInstance,
                                               uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t *data, uint8_t length, bool confirmed);

  virtual void groupValueReadLocalConfirm(AckType ack, uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl, bool status);
  virtual void groupValueReadIndication(uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl);
  virtual void groupValueReadResponseConfirm(AckType ack, uint16_t asap, Priority priority, HopCountType hopTtype, const SecurityControl &secCtrl,
                                             uint8_t *data, uint8_t dataLength, bool status);
  virtual void groupValueReadAppLayerConfirm(uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl,
                                             uint8_t *data, uint8_t dataLength);
  virtual void groupValueWriteLocalConfirm(AckType ack, uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl,
                                           uint8_t *data, uint8_t dataLength, bool status);
  virtual void groupValueWriteIndication(uint16_t asap, Priority priority, HopCountType hopType, const SecurityControl &secCtrl,
                                         uint8_t *data, uint8_t dataLength);
  virtual void individualAddressWriteLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl,
                                                  uint16_t newaddress, bool status);
  virtual void individualAddressReadLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, bool status);
  virtual void individualAddressReadResponseConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, bool status);
  virtual void individualAddressReadAppLayerConfirm(HopCountType hopType, const SecurityControl &secCtrl, uint16_t individualAddress);
  virtual void individualAddressSerialNumberReadLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl,
                                                             uint8_t *serialNumber, bool status);
  virtual void individualAddressSerialNumberReadResponseConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl,
                                                                uint8_t *serialNumber, uint16_t domainAddress, bool status);
  virtual void individualAddressSerialNumberReadAppLayerConfirm(HopCountType hopType, const SecurityControl &secCtrl, uint8_t *serialNumber,
                                                                uint16_t individualAddress, uint16_t domainAddress);
  virtual void individualAddressSerialNumberWriteLocalConfirm(AckType ack, HopCountType hopType, const SecurityControl &secCtrl, uint8_t *serialNumber,
                                                              uint16_t newaddress, bool status);
  virtual void deviceDescriptorReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl,
                                                uint8_t descriptorType, bool status);
  virtual void deviceDescriptorReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl,
                                                   uint8_t descriptor_type, uint8_t *device_descriptor, bool status);
  virtual void deviceDescriptorReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl,
                                                   uint8_t descriptortype, uint8_t *deviceDescriptor);
  virtual void restartRequestLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, bool status);
  virtual void propertyValueReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl,
                                             uint8_t objectIndex, uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, bool status);
  virtual void propertyValueReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                                uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t *data, uint8_t length, bool status);
  virtual void propertyValueReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                                uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t *data, uint8_t length);
  virtual void propertyValueWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t objectIndex,
                                              uint8_t propertyId, uint8_t numberOfElements, uint16_t startIndex, uint8_t *data, uint8_t length, bool status);
  virtual void propertyDescriptionReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl,
                                                   uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool status);
  virtual void propertyDescriptionReadResponse(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl,
                                               uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool writeEnable, uint8_t type,
                                               uint16_t maxNumberOfElements, uint8_t access);
  virtual void propertyDescriptionReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl,
                                                      uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool writeEnable, uint8_t type,
                                                      uint16_t maxNumberOfElements, uint8_t access, bool status);
  virtual void propertyDescriptionReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl,
                                                      uint8_t objectIndex, uint8_t propertyId, uint8_t propertyIndex, bool writeEnable, uint8_t type,
                                                      uint16_t maxNumberOfElements, uint8_t access);
  virtual void memoryReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                      uint16_t memoryAddress, bool status);
  virtual void memoryReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                         uint16_t memoryAddress, uint8_t *data, bool status);
  virtual void memoryReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                         uint16_t memoryAddress, uint8_t *data);
  virtual void memoryWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                       uint16_t memoryAddress, uint8_t *data, bool status);
  virtual void memoryExtReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                         uint32_t memoryAddress, bool status);
  virtual void memoryExtReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                            uint32_t memoryAddress, uint8_t *data, bool status);
  virtual void memoryExtReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                            uint32_t memoryAddress, uint8_t *data);
  virtual void memoryExtWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                          uint32_t memoryAddress, uint8_t *data, bool status);
  virtual void memoryExtWriteResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                             uint32_t memoryAddress, uint8_t *data, bool status);
  virtual void memoryExtWriteAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                             uint32_t memoryAddress, uint8_t *data);
  virtual void userMemoryReadLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                          uint32_t memoryAddress, bool status);
  virtual void userMemoryReadResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                             uint32_t memoryAddress, uint8_t *memoryData, bool status);
  virtual void userMemoryReadAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                             uint32_t memoryAddress, uint8_t *memoryData);
  virtual void userMemoryWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t number,
                                           uint32_t memoryAddress, uint8_t *memoryData, bool status);
  virtual void userManufacturerInfoLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, bool status);
  virtual void userManufacturerInfoIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl);
  virtual void userManufacturerInfoResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl,
                                                   uint8_t *info, bool status);
  virtual void userManufacturerInfoAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl,
                                                   uint8_t *info);
  virtual void authorizeLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint32_t key, bool status);
  virtual void authorizeResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level,
                                        bool status);
  virtual void authorizeAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level);
  virtual void keyWriteLocalConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level,
                                    uint32_t key, bool status);
  virtual void keyWriteIndication(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level,
                                  uint32_t key);
  virtual void keyWriteResponseConfirm(AckType ack, Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level,
                                       bool status);
  virtual void keyWriteAppLayerConfirm(Priority priority, HopCountType hopType, uint16_t asap, const SecurityControl &secCtrl, uint8_t level);

  virtual void domainAddressSerialNumberWriteIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t *rfDoA,
                                                        const uint8_t *knxSerialNumber);

  virtual void domainAddressSerialNumberReadIndication(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t *knxSerialNumber);

  virtual void domainAddressSerialNumberWriteLocalConfirm(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t *rfDoA,
                                                          const uint8_t *knxSerialNumber, bool status);

  virtual void domainAddressSerialNumberReadLocalConfirm(Priority priority, HopCountType hopType, const SecurityControl &secCtrl, const uint8_t *knxSerialNumber, bool status);

public:
  GroupObjectTableObject &groupObjectTable();

  void sendNextGroupTelegram();
  void updateGroupObject(GroupObject &go, uint8_t *data, uint8_t length);

  // virtual void doMasterReset(EraseCode eraseCode, uint8_t channel);

  AddressTableObject _addrTable;
  AssociationTableObject _assocTable;
  GroupObjectTableObject _groupObjTable;
  ApplicationLayer _appLayer;
  TransportLayer _transLayer;
  NetworkLayer _netLayer;

  bool _configured = true;
  TpUartDataLinkLayer _dlLayer;
};

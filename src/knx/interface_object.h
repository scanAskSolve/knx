#pragma once

#include <stddef.h>
#include "property.h"
#include "save_restore.h"
#include "knx_types.h"

enum ObjectType
{
  OT_DEVICE = 0,

  OT_ADDR_TABLE = 1,

  OT_ASSOC_TABLE = 2,

  OT_APPLICATION_PROG = 3,

  OT_INTERFACE_PROG = 4,

  OT_OJB_ASSOC_TABLE = 5,

  OT_ROUTER = 6,

  OT_LTE_ADDR_ROUTING_TABLE = 7,

  OT_CEMI_SERVER = 8,

  OT_GRP_OBJ_TABLE = 9,

  OT_POLLING_MASTER = 10,

  OT_IP_PARAMETER = 11,

  OT_RESERVED = 12,

  OT_FILE_SERVER = 13,

  OT_SECURITY = 17,

  OT_RF_MEDIUM = 19
};

// class InterfaceObject : public SaveRestore
class InterfaceObject
{
public:
  virtual ~InterfaceObject();
  virtual void readProperty(PropertyID id, uint16_t start, uint8_t &count, uint8_t *data);
  virtual void writeProperty(PropertyID id, uint16_t start, uint8_t *data, uint8_t &count);
  virtual uint8_t propertySize(PropertyID id);
  virtual void command(PropertyID id, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength);
  virtual void state(PropertyID id, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength);
  void readPropertyDescription(uint8_t &propertyId, uint8_t &propertyIndex, bool &writeEnable, uint8_t &type, uint16_t &numberOfElements, uint8_t &access);

  virtual void masterReset(EraseCode eraseCode, uint8_t channel);

  Property *property(PropertyID id);

  // const uint8_t* propertyData(PropertyID id);
  // const uint8_t* propertyData(PropertyID id, uint16_t elementIndex);
  const Property *property(PropertyID id) const;

  virtual uint8_t *save(uint8_t *buffer);
  virtual const uint8_t *restore(const uint8_t *buffer);
  virtual uint16_t saveSize();

protected:
  virtual void initializeProperties(size_t propertiesSize, Property **properties);

  Property **_properties = nullptr;
  uint8_t _propertyCount = 0;
};

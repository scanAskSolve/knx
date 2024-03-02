#pragma once

#include <stddef.h>
#include "property.h"

#include "knx_types.h"

/** Enum for the type of an interface object. See Section 2.2 of knx:3/7/3 */
enum ObjectType
{
  /** Device object. */
  OT_DEVICE = 0,

  /** Address table object. */
  OT_ADDR_TABLE = 1,

  /** Association table object. */
  OT_ASSOC_TABLE = 2,

  /** Application program object. */
  OT_APPLICATION_PROG = 3,

  /** Interface program object. */
  OT_INTERFACE_PROG = 4,

  /** KNX - Object Associationtable. */
  OT_OJB_ASSOC_TABLE = 5,

  /** Router Object */
  OT_ROUTER = 6,

  /** LTE Address Routing Table Object */
  OT_LTE_ADDR_ROUTING_TABLE = 7,

  /** cEMI Server Object */
  OT_CEMI_SERVER = 8,

  /** Group Object Table Object */
  OT_GRP_OBJ_TABLE = 9,

  /** Polling Master */
  OT_POLLING_MASTER = 10,

  /** KNXnet/IP Parameter Object */
  OT_IP_PARAMETER = 11,

  /** Reserved. Shall not be used. */
  OT_RESERVED = 12,

  /** File Server Object */
  OT_FILE_SERVER = 13,

  /** Security Interface Object */
  OT_SECURITY = 17,

  /** RF Medium Object */
  OT_RF_MEDIUM = 19
};

/**
 * This class represents and interface object. See section 4 of @cite knx:3/4/1.
 */
// class InterfaceObject : public SaveRestore
class InterfaceObject
{
public:
  ~InterfaceObject();

  void readProperty(PropertyID id, uint16_t start, uint8_t &count, uint8_t *data);
  void writeProperty(PropertyID id, uint16_t start, uint8_t *data, uint8_t &count);
  uint8_t propertySize(PropertyID id);
  void command(PropertyID id, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength);
  void state(PropertyID id, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength);
  void readPropertyDescription(uint8_t &propertyId, uint8_t &propertyIndex, bool &writeEnable, uint8_t &type, uint16_t &numberOfElements, uint8_t &access);
  void masterReset(EraseCode eraseCode, uint8_t channel);
  Property *property(PropertyID id);
  const Property *property(PropertyID id) const;

  virtual uint8_t *save(uint8_t *buffer);
  virtual const uint8_t *restore(const uint8_t *buffer);
  virtual uint16_t saveSize();

protected:
  virtual void initializeProperties(size_t propertiesSize, Property **properties);

  Property **_properties = nullptr;
  uint8_t _propertyCount = 0;
};

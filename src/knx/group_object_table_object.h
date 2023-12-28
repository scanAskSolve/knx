#pragma once

#include "table_object.h"
#include "group_object.h"

class GroupObjectTableObject : public TableObject
{
  // friend class GroupObject;

public:
  GroupObjectTableObject(Memory &memory);
  virtual ~GroupObjectTableObject();
  uint16_t entryCount();
  GroupObject &get(uint16_t asap);
  GroupObject &nextUpdatedObject(bool &valid);
  void groupObjects(GroupObject *objs, uint16_t size);

  const uint8_t *restore(const uint8_t *buffer) override;

  // protected:
  void beforeStateChange(LoadState &newState) override;

  // private:
  void freeGroupObjects();
  bool initGroupObjects();
  uint16_t *_tableData = 0;
  GroupObject *_groupObjects = 0;
  uint16_t _groupObjectCount = 0;
  void readProperty(PropertyID id, uint16_t start, uint8_t &count, uint8_t *data);
  void writeProperty(PropertyID id, uint16_t start, uint8_t *data, uint8_t &count);
void readPropertyDescription(uint8_t &propertyId, uint8_t &propertyIndex, bool &writeEnable, uint8_t &type, uint16_t &numberOfElements, uint8_t &access);
 uint8_t propertySize(PropertyID id);
 void command(PropertyID id, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength);
  void state(PropertyID id, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength);
  Property *property(PropertyID id);
};
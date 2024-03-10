#pragma once

#include "table_object.h"
#include "group_object.h"

class Memory;

// class GroupObjectTableObject : public TableObject
class GroupObjectTableObject
{
  // friend class GroupObject;

public:
  GroupObjectTableObject(Memory &memory);
  virtual ~GroupObjectTableObject();
  uint16_t entryCount();
  GroupObject &get(uint16_t asap);
  GroupObject &nextUpdatedObject(bool &valid);
  void groupObjects(GroupObject *objs, uint16_t size);

  const uint8_t *restore(const uint8_t *buffer);

  // protected:
  void beforeStateChange(LoadState &newState);

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
  void masterReset(EraseCode eraseCode, uint8_t channel);

public:
  LoadState _state = LS_UNLOADED;
  Memory &_memory;

  uint8_t *_data = 0;
  /**
   * This method returns the ::LoadState of the interface object.
   */
  LoadState loadState();
  void loadState(LoadState newState);
  virtual uint8_t *save(uint8_t *buffer);
  virtual const uint8_t *tableObjectRestore(const uint8_t *buffer);
  virtual uint16_t saveSize();
  /**
   * returns the internal data of the interface object. This pointer belongs to the TableObject class and
   * must not be freed.
   */
  uint8_t *data();
  uint16_t interfaceSaveSize();
  Property **_properties = nullptr;
  uint8_t _propertyCount = 0;

  uint32_t _size = 0;
  uint8_t _tableUnloadCount;
  void tableObjectBeforeStateChange(LoadState &newState);
  void errorCode(ErrorCode errorCode);

  void initializeProperties(size_t propertiesSize, Property **properties);
  void interfaceInitializeProperties(size_t propertiesSize, Property **properties);
  uint32_t tableReference();
  bool allocTable(uint32_t size, bool doFill, uint8_t fillByte);
  void loadEvent(const uint8_t *data);
  void loadEventUnloaded(const uint8_t *data);
  void loadEventLoading(const uint8_t *data);
  void loadEventLoaded(const uint8_t *data);
  void loadEventError(const uint8_t *data);
  void additionalLoadControls(const uint8_t *data);
};
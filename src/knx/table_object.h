#pragma once

#include "stddef.h"
#include "property.h"
#include "knx_types.h"
class Memory;

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

typedef void (*BeforeTablesUnloadCallback)();

/**
 * This class provides common functionality for interface objects that are configured by ETS with MemorWrite.
 */
// class TableObject : public InterfaceObject
class TableObject
{
  // friend class Memory;

public:
  /**
   * The constuctor.
   * @param memory The instance of the memory management class to use.
   */
  TableObject(Memory &memory);

  /**
   * The destructor.
   */
  virtual ~TableObject();
  /**
   * This method returns the ::LoadState of the interface object.
   */
  LoadState loadState();
  uint8_t *save(uint8_t *buffer);
  const uint8_t *restore(const uint8_t *buffer);
  uint16_t saveSize();

  static void beforeTablesUnloadCallback(BeforeTablesUnloadCallback func);
  static BeforeTablesUnloadCallback beforeTablesUnloadCallback();

  // protected:
  /**
   * This method is called before the interface object enters a new ::LoadState.
   * If there is a error changing the state newState should be set to ::LS_ERROR and errorCode()
   * to a reason for the failure.
   */
  virtual void beforeStateChange(LoadState &newState);

  /**
   * returns the internal data of the interface object. This pointer belongs to the TableObject class and
   * must not be freed.
   */
  uint8_t *data();
  /**
   * Set the reason for a state change failure.
   */
  void errorCode(ErrorCode errorCode);

  void initializeProperties(size_t propertiesSize, Property **properties);

  static BeforeTablesUnloadCallback _beforeTablesUnload;

  // private:
  uint32_t tableReference();
  bool allocTable(uint32_t size, bool doFill, uint8_t fillByte);
  void loadEvent(const uint8_t *data);
  void loadEventUnloaded(const uint8_t *data);
  void loadEventLoading(const uint8_t *data);
  void loadEventLoaded(const uint8_t *data);
  void loadEventError(const uint8_t *data);
  void additionalLoadControls(const uint8_t *data);
  /**
   * set the ::LoadState of the interface object.
   *
   * Calls beforeStateChange().
   *
   * @param newState the new ::LoadState
   */
  void loadState(LoadState newState);
  LoadState _state = LS_UNLOADED;
  Memory &_memory;
  uint8_t *_data = 0;
  static uint8_t _tableUnloadCount;

  /**
   * used to store size of data() in allocTable(), needed for calculation of crc in PID_MCB_TABLE.
   * This value is also saved and restored.
   * The size of the memory block cannot be used because it is changed during alignment to page size.
   */
  uint32_t _size = 0;

public:
  uint16_t intergaceSaveSize();
  void interfaceInitializeProperties(size_t propertiesSize, Property **properties);
  Property *property(PropertyID id);
  Property **_properties = nullptr;
  uint8_t _propertyCount = 0;
};

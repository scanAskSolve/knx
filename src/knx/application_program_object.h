#pragma once

#include "table_object.h"
#include "bits.h"

class ApplicationProgramObject : public TableObject
{
  public:
    ApplicationProgramObject(Memory& memory);
    uint8_t* save(uint8_t* buffer) override;
    const uint8_t* restore(const uint8_t* buffer) override;
    uint16_t saveSize() override;
    uint8_t* data(uint32_t addr);
    uint8_t getByte(uint32_t addr);
    uint16_t getWord(uint32_t addr);
    uint32_t getInt(uint32_t addr);
    double getFloat(uint32_t addr, ParameterFloatEncodings encoding);
   void readProperty(PropertyID id, uint16_t start, uint8_t &count, uint8_t *data);
    void writeProperty(PropertyID id, uint16_t start, uint8_t *data, uint8_t &count);
    void readPropertyDescription(uint8_t &propertyId, uint8_t &propertyIndex, bool &writeEnable, uint8_t &type, uint16_t &numberOfElements, uint8_t &access);
 uint8_t propertySize(PropertyID id);
};

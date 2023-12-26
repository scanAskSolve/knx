#pragma once

#include "interface_object.h"

#define LEN_HARDWARE_TYPE 6

class DeviceObject : public InterfaceObject
{
public:
    // increase this version anytime DeviceObject-API changes
    // the following value represents the serialized representation of DeviceObject.
    const uint16_t apiVersion = 1;

    DeviceObject();
    uint8_t *save(uint8_t *buffer);
    const uint8_t *restore(const uint8_t *buffer);
    uint16_t saveSize();

    uint16_t individualAddress();
    void individualAddress(uint16_t value);

    void individualAddressDuplication(bool value);
    bool verifyMode();
    void verifyMode(bool value);
    bool progMode();
    void progMode(bool value);
    uint16_t manufacturerId();
    void manufacturerId(uint16_t value);
    uint32_t bauNumber();
    void bauNumber(uint32_t value);
    const uint8_t *orderNumber();
    void orderNumber(const uint8_t *value);
    const uint8_t *hardwareType();
    void hardwareType(const uint8_t *value);
    uint16_t version();
    void version(uint16_t value);
    uint16_t maskVersion();
    void maskVersion(uint16_t value);
    uint16_t maxApduLength();
    void maxApduLength(uint16_t value);
    const uint8_t *rfDomainAddress();
    void rfDomainAddress(uint8_t *value);
    uint8_t defaultHopCount();

private:
    uint8_t _prgMode = 0;
    uint16_t _ownAddress = 65535; // 15.15.255;

public:
    void propertyValue(PropertyID id, uint8_t *value);

    const uint8_t *propertyData(PropertyID id);
    const uint8_t *propertyData(PropertyID id, uint16_t elementIndex);
    void readProperty(PropertyID id, uint16_t start, uint8_t &count, uint8_t *data);
    void writeProperty(PropertyID id, uint16_t start, uint8_t *data, uint8_t &count);
    void readPropertyDescription(uint8_t &propertyId, uint8_t &propertyIndex, bool &writeEnable, uint8_t &type, uint16_t &numberOfElements, uint8_t &access);
 uint8_t propertySize(PropertyID id);
};

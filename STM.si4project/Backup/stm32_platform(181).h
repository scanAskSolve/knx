#ifdef //**//Print_ARCH_STM32
#include "//**//Print_platform.h"

class Stm32Platform : public //**//PrintPlatform
{
public:
    Stm32Platform();
    Stm32Platform( HardwareSerial* s);
    ~Stm32Platform();

    // unique serial number
    uint32_t uniqueSerialNumber();

    // basic stuff
    void restart();
    
    //memory
    uint8_t* getEepromBuffer(uint32_t size);
    void commitToEeprom();
private:
    uint8_t *_eepromPtr = nullptr;
    uint16_t _eepromSize = 0;
};

#endif

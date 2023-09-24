#include "knx/platform_c.h"

#include "Arduino.h"

//#ifdef KNX_PRINT_ENABLE
	#ifndef KNX_DEBUG_SERIAL
		#define KNX_DEBUG_SERIAL Serial
	#endif
//#endif

class ArduinoPlatform : public Platform_C
{
  public:
    ArduinoPlatform();
    ArduinoPlatform(HardwareSerial* knxSerial);

    // basic stuff
    void fatalError();

    //uart
    virtual void knxUart( HardwareSerial* serial);
    virtual HardwareSerial* knxUart();
    virtual void setupUart();
    virtual void closeUart();
    virtual int uartAvailable();
    virtual size_t writeUart(const uint8_t data);
    virtual size_t writeUart(const uint8_t* buffer, size_t size);
    virtual int readUart();
    virtual size_t readBytesUart(uint8_t* buffer, size_t length);

    //spi
/*#ifndef KNX_NO_SPI

    void setupSpi() override;
    void closeSpi() override;
    int readWriteSpi (uint8_t *data, size_t len) override;
    
#endif*/
#ifndef KNX_PRINT_DISABLE

    static Stream* SerialDebug;
#endif

  protected:
    HardwareSerial* _knxSerial;
};


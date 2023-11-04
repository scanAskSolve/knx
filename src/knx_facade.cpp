#include "knx_facade.h"
#include "knx/bits.h"


#ifndef PROG_BTN_PRESS_MIN_MILLIS
	#define PROG_BTN_PRESS_MIN_MILLIS 50
#endif

#ifndef PROG_BTN_PRESS_MAX_MILLIS
	#define PROG_BTN_PRESS_MAX_MILLIS 500
#endif



//#ifndef KNX_NO_AUTOMATIC_GLOBAL_INSTANCE

    #if defined(ARDUINO_ARCH_STM32)

        // Only ESP8266 and ESP32 have this define. For all other platforms this is just empty.
        //#ifndef ICACHE_RAM_ATTR
            //#define ICACHE_RAM_ATTR
        //#endif
		//ICACHE_RAM_ATTR void buttonEvent()
        void buttonEvent()
        {
            static uint32_t lastEvent=0;
            static uint32_t lastPressed=0;

            uint32_t diff = millis() - lastEvent;
            if (diff >= PROG_BTN_PRESS_MIN_MILLIS && diff <= PROG_BTN_PRESS_MAX_MILLIS){
                if (millis() - lastPressed > 200)
                {  
                    knx.toggleProgMode();
                    lastPressed = millis();
                }
            }
            lastEvent = millis();
        }
    #endif

    #ifdef ARDUINO_ARCH_STM32
        #if MASK_VERSION == 0x07B0
            //KnxFacade<Stm32Platform, Bau07B0> knx(buttonEvent);
		    KnxFacade knx(buttonEvent);
        #else
            #error "Mask version not supported on ARDUINO_ARCH_STM32"
        #endif
    #else // Non-Arduino platforms and Linux platform
        // no predefined global instance
        #error "no predefined global instance"
    #endif


//#endif // KNX_NO_AUTOMATIC_GLOBAL_INSTANCE

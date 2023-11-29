
#include "knx_wrapper.h"
#include "knx_facade.h"

#ifndef PROG_BTN_PRESS_MIN_MILLIS
	#define PROG_BTN_PRESS_MIN_MILLIS 50
#endif

#ifndef PROG_BTN_PRESS_MAX_MILLIS
	#define PROG_BTN_PRESS_MAX_MILLIS 500
#endif

void buttonEvent2()
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


struct KnxFacadeTop : KnxFacade {
     KnxFacadeTop() : KnxFacade(buttonEvent2) {
     }
};
KnxFacadeTop* _knxFacadeTop;
extern "C" {

void knx_create() {
    _knxFacadeTop = new KnxFacadeTop();
}

void knx_destroy() {
    delete _knxFacadeTop;
}

void knx_getGroupObject(int goNr) {
    _knxFacadeTop->getGroupObject(goNr);
}

uint8_t knx_paramByte(uint32_t addr) {
    return _knxFacadeTop->paramByte(addr);
}

void knx_loop() {
    _knxFacadeTop->loop();
}

int knx_configured() {
    return _knxFacadeTop->configured();
}

void knx_start() {
    _knxFacadeTop->start();
}

void knx_readMemory() {
    _knxFacadeTop->readMemory();
}

}  // extern "C"
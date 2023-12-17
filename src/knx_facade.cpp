#include "knx_facade.h"
#include "knx/bits.h"


#ifndef PROG_BTN_PRESS_MIN_MILLIS
	#define PROG_BTN_PRESS_MIN_MILLIS 50
#endif

#ifndef PROG_BTN_PRESS_MAX_MILLIS
	#define PROG_BTN_PRESS_MAX_MILLIS 500
#endif

#if MASK_VERSION == 0x07B0
    //KnxFacade<Stm32Platform, BauSystemB> knx(buttonEvent);
    KnxFacade knx(buttonEvent);
#else
    #error "Mask version not supported on ARDUINO_ARCH_STM32"
#endif

//****************************************************************************
//KnxFacade::KnxFacade(IsrFunctionPtr buttonISRFunction) : _platformPtr(new Stm32Platform()), _bauPtr(new BauSystemB(*_platformPtr)), _bau(*_bauPtr)
KnxFacade::KnxFacade(IsrFunctionPtr buttonISRFunction)
{
    manufacturerId(0xfa);
    bauNumber(platform().uniqueSerialNumber());
    _bau.addSaveRestore(this);
    setButtonISRFunction(buttonISRFunction);
}

KnxFacade:: ~KnxFacade()
{
    if (_bauPtr)
        delete _bauPtr;

    if (_platformPtr)
        delete _platformPtr;
}

ArduinoPlatform& KnxFacade::platform()
{
    return *_platformPtr;
}

BauSystemB& KnxFacade::bau()
{
    return _bau;
}

bool KnxFacade::enabled()
{
    return _bau.enabled();
}

void KnxFacade::enabled(bool value)
{
    _bau.enabled(value);
}

bool KnxFacade::progMode()
{
    return _bau.deviceObject().progMode();
}

void KnxFacade::progMode(bool value)
{
    _bau.deviceObject().progMode(value);
}

/**
 * To be called by ISR handling on button press.
 */
void KnxFacade::toggleProgMode()
{
    _toggleProgMode = true;
}

bool KnxFacade::configured()
{
    return _bau.configured();
}

/**
 * returns HIGH if led is active on HIGH, LOW otherwise
 */
uint32_t KnxFacade::ledPinActiveOn()
{
    return _ledPinActiveOn;
}

/**
 * Sets if the programming led is active on HIGH or LOW. 
 * 
 * Set to HIGH for GPIO--RESISTOR--LED--GND or to LOW for GPIO--LED--RESISTOR--VDD
 */
void KnxFacade::ledPinActiveOn(uint32_t value)
{
    _ledPinActiveOn = value;
}

uint32_t KnxFacade::ledPin()
{
    return _ledPin;
}

void KnxFacade::ledPin(uint32_t value)
{
    _ledPin = value;
}

/*void KnxFacade::setProgLedOffCallback(ProgLedOffCallback progLedOffCallback)
{
    _progLedOffCallback = progLedOffCallback;
}

void KnxFacade::setProgLedOnCallback(ProgLedOnCallback progLedOnCallback)
{
    _progLedOnCallback = progLedOnCallback;
}*/


int32_t KnxFacade::buttonPin()
{
    return _buttonPin;
}

void KnxFacade::buttonPin(int32_t value)
{
    _buttonPin = value;
}

void KnxFacade::readMemory()
{
    _bau.readMemory();
}

void KnxFacade::writeMemory()
{
    _bau.writeMemory();
}

uint16_t KnxFacade::individualAddress()
{
    return _bau.deviceObject().individualAddress();
}

void KnxFacade::loop()
{
    if (progMode() != _progLedState)
    {
        _progLedState = progMode();
        if (_progLedState)
        {
            println("progmode on");
            progLedOn();
        }
        else
        {
            println("progmode off");
            progLedOff();
        }
    }
    if (_toggleProgMode)
    {
        progMode(!progMode());
        _toggleProgMode = false;
    }
    _bau.loop();
}

void KnxFacade::manufacturerId(uint16_t value)
{
    _bau.deviceObject().manufacturerId(value);
}

void KnxFacade::bauNumber(uint32_t value)
{
    _bau.deviceObject().bauNumber(value);
}

void KnxFacade::orderNumber(const uint8_t* value)
{
    _bau.deviceObject().orderNumber(value);
}

void KnxFacade::hardwareType(const uint8_t* value)
{
    _bau.deviceObject().hardwareType(value);
}

void KnxFacade::version(uint16_t value)
{
    _bau.deviceObject().version(value);
}

void KnxFacade::start()
{
    /*if (_progLedOffCallback == 0 || _progLedOnCallback == 0)
        pinMode(ledPin(), OUTPUT);*/
    pinMode(ledPin(), OUTPUT);

    progLedOff();
    pinMode(buttonPin(), INPUT_PULLUP);

    if (_progButtonISRFuncPtr && _buttonPin >= 0)
    {
        // Workaround for https://github.com/arduino/ArduinoCore-samd/issues/587
        #if (ARDUINO_API_VERSION >= 10200)
            attachInterrupt(_buttonPin, _progButtonISRFuncPtr, (PinStatus)CHANGE);
        #else
            attachInterrupt(_buttonPin, _progButtonISRFuncPtr, CHANGE);
        #endif
    }

    enabled(true);
}

void KnxFacade::setButtonISRFunction(IsrFunctionPtr progButtonISRFuncPtr)
{
    _progButtonISRFuncPtr = progButtonISRFuncPtr;
}

void KnxFacade::setSaveCallback(SaveCallback func)
{
    _saveCallback = func;
}

void KnxFacade::setRestoreCallback(RestoreCallback func)
{
    _restoreCallback = func;
}

uint8_t* KnxFacade::paramData(uint32_t addr)
{
    if (!_bau.configured())
        return nullptr;

    return _bau.parameters().data(addr);
}

// paramBit(address, shift)
// get state of a parameter as a boolean like "enable/disable", ...
// Declaration in XML file:
// ...
// <ParameterType Id="M-00FA_A-0066-EA-0001_PT-toggle" Name="toggle">
//   <TypeRestriction Base="Value" SizeInBit="1">
//     <Enumeration Text="Désactivé" Value="0" Id="M-00FA_A-0066-EA-0001_PT-toggle_EN-0"/>
//     <Enumeration Text="Activé" Value="1" Id="M-00FA_A-0066-EA-0001_PT-toggle_EN-1"/>
//  </TypeRestriction>
// </ParameterType>
// ...
// <Parameter Id="M-00FA_A-0066-EA-0001_P-2" Name="Input 1" ParameterType="M-00FA_A-0066-EA-0001_PT-toggle" Text="Input 1" Value="1">
//   <Memory CodeSegment="M-00FA_A-0066-EA-0001_RS-04-00000" Offset="1" BitOffset="0"/>
// </Parameter>
// <Parameter Id="M-00FA_A-0066-EA-0001_P-3" Name="Input 2" ParameterType="M-00FA_A-0066-EA-0001_PT-toggle" Text="Input 2" Value="1">
//   <Memory CodeSegment="M-00FA_A-0066-EA-0001_RS-04-00000" Offset="1" BitOffset="1"/>
// </Parameter>
// <Parameter Id="M-00FA_A-0066-EA-0001_P-4" Name="Inout 3" ParameterType="M-00FA_A-0066-EA-0001_PT-toggle" Text="Input 3" Value="1">
//   <Memory CodeSegment="M-00FA_A-0066-EA-0001_RS-04-00000" Offset="1" BitOffset="2"/>
// </Parameter>
// ...
// Usage in code :
//   if ( knx.paramBit(1,1))
//   {
//      //do somthings ....
//   }
bool KnxFacade::paramBit(uint32_t addr, uint8_t shift)
{
    if (!_bau.configured())
        return 0;

    return (bool) ((_bau.parameters().getByte(addr) >> (7-shift)) & 0x01); 
}

uint8_t KnxFacade::paramByte(uint32_t addr)
{
    if (!_bau.configured())
        return 0;

    return _bau.parameters().getByte(addr);
}

// Same usage than paramByte(addresse) for signed parameters
// Declaration in XML file
// <ParameterType Id="M-00FA_A-0066-EA-0001_PT-delta" Name="delta">
//   <TypeNumber SizeInBit="8" Type="signedInt" minInclusive="-10" maxInclusive="10"/>
// </ParameterType>
int8_t KnxFacade::paramSignedByte(uint32_t addr)
{
    if (!_bau.configured())
        return 0;

    return (int8_t) _bau.parameters().getByte(addr);
}

uint16_t KnxFacade::paramWord(uint32_t addr)
{
    if (!_bau.configured())
        return 0;

    return _bau.parameters().getWord(addr);
}

uint32_t KnxFacade::paramInt(uint32_t addr)
{
    if (!_bau.configured())
        return 0;

    return _bau.parameters().getInt(addr);
}

double KnxFacade::paramFloat(uint32_t addr, ParameterFloatEncodings enc)
{
    if (!_bau.configured())
        return 0;

    return _bau.parameters().getFloat(addr, enc);
}

#if (MASK_VERSION == 0x07B0) || (MASK_VERSION == 0x27B0) || (MASK_VERSION == 0x57B0)
GroupObject& KnxFacade::getGroupObject(uint16_t goNr)
{
    return _bau.groupObjectTable().get(goNr);
}
#endif

void KnxFacade::restart(uint16_t individualAddress)
{
    SecurityControl sc = {false, None};
    _bau.restartRequest(individualAddress, sc);
}

void KnxFacade::beforeRestartCallback(BeforeRestartCallback func)
{
    _bau.beforeRestartCallback(func);
}

BeforeRestartCallback KnxFacade::beforeRestartCallback()
{
    return _bau.beforeRestartCallback();
}

uint8_t* KnxFacade::save(uint8_t* buffer)
{
    if (_saveCallback != 0)
        return _saveCallback(buffer);

    return buffer;
}

const uint8_t* KnxFacade::restore(const uint8_t* buffer)
{
    if (_restoreCallback != 0)
        return _restoreCallback(buffer);

    return buffer;
}

uint16_t KnxFacade::saveSize()
{
    return _saveSize;
}

void KnxFacade::saveSize(uint16_t size)
{
    _saveSize = size;
}

void KnxFacade::progLedOn()
{
    /*if (_progLedOnCallback == 0)
        digitalWrite(ledPin(), _ledPinActiveOn);
    else
        _progLedOnCallback();*/
    digitalWrite(ledPin(), _ledPinActiveOn);
}

void KnxFacade::progLedOff()
{
    /*if (_progLedOffCallback == 0)
        digitalWrite(ledPin(), HIGH - _ledPinActiveOn);
    else
        _progLedOffCallback();*/
    digitalWrite(ledPin(), HIGH - _ledPinActiveOn);
}


void buttonEvent()
    {
        static uint32_t lastEvent=0;
        static uint32_t lastPressed=0;

        uint32_t diff = HAL_GetTick() - lastEvent;
        if (diff >= PROG_BTN_PRESS_MIN_MILLIS && diff <= PROG_BTN_PRESS_MAX_MILLIS){
            if (HAL_GetTick() - lastPressed > 200)
            {  
                knx.toggleProgMode();
                lastPressed = HAL_GetTick();
            }
        }
        lastEvent = HAL_GetTick();
    }
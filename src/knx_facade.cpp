#include "knx_facade.h"
#include "arduino_platform.h"
#include "knx/bits.h"


/*#ifndef PROG_BTN_PRESS_MIN_MILLIS
	#define PROG_BTN_PRESS_MIN_MILLIS 50
#endif

#ifndef PROG_BTN_PRESS_MAX_MILLIS
	#define PROG_BTN_PRESS_MAX_MILLIS 500
#endif*/

/*#if MASK_VERSION == 0x07B0
    //KnxFacade<Stm32Platform, BauSystemB> knx(buttonEvent);
    //KnxFacade knx(buttonEvent);
    KnxFacade knx;
#else
    #error "Mask version not supported on ARDUINO_ARCH_STM32"
#endif*/
GPIO_PinState _ledPinActiveOn = KNX_LED_ACTIVE_ON;
GPIO_infoTypeDef _ledPin;
uint16_t _saveSize = USERDATA_SAVE_SIZE;
GPIO_infoTypeDef _buttonPin;
volatile bool _toggleProgMode = false;
bool _progLedState = false;

BauSystemB *_bau;

//void KNX_initKnxFacade(HardwareSerial* knxSerial)
void KNX_initKnxFacade()
{
    //ArduinoPlatform* _platformPtr  =  new ArduinoPlatform(knxSerial);
    //_bau = new BauSystemB(*_platformPtr);
    _bau = new BauSystemB();
    
    KNX_manufacturerId(0xfa);
    KNX_bauNumber(uniqueSerialNumber());
    _bau->addSaveRestore(new DeviceObject());
}

bool KNX_enabled()
{
    return _bau->enabled();
}

void KNX_enabled(bool value)
{
    _bau->enabled(value);
}

bool KNX_progMode()
{
    return _bau->deviceObject().progMode();
}

void KNX_progMode(bool value)
{
    _bau->deviceObject().progMode(value);
}

/**
 * To be called by ISR handling on button press.
 */
void KNX_toggleProgMode()
{
    _toggleProgMode = true;
}

bool KNX_configured()
{
    return _bau->configured();
}

/**
 * returns HIGH if led is active on HIGH, LOW otherwise
 */
GPIO_PinState KNX_ledPinActiveOn()
{
    return _ledPinActiveOn;
}

/**
 * Sets if the programming led is active on HIGH or LOW. 
 * 
 * Set to HIGH for GPIO--RESISTOR--LED--GND or to LOW for GPIO--LED--RESISTOR--VDD
 */
void KNX_ledPinActiveOn(GPIO_PinState value)
{
    _ledPinActiveOn = value;
}

GPIO_infoTypeDef KNX_ledPin()
{
    return _ledPin;
}

void KNX_ledPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    _ledPin.GPIO_Pin = GPIO_Pin;
    _ledPin.GPIOx = GPIOx;

}

GPIO_infoTypeDef KNX_buttonPin()
{
    return _buttonPin;
}

void KNX_buttonPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    _buttonPin.GPIO_Pin = GPIO_Pin;
    _buttonPin.GPIOx = GPIOx;
}

void KNX_readMemory()
{
    _bau->readMemory();
}

void KNX_writeMemory()
{
    _bau->writeMemory();
}

uint16_t KNX_individualAddress()
{
    return _bau->deviceObject().individualAddress();
}

void KNX_loop()
{
    if (KNX_progMode() != _progLedState)
    {
        _progLedState = KNX_progMode();
        if (_progLedState)
        {
            println("progmode on");
            KNX_progLedOn();
        }
        else
        {
            println("progmode off");
            KNX_progLedOff();
        }
    }
    if (_toggleProgMode)
    {
        KNX_progMode(!KNX_progMode());
        _toggleProgMode = false;
    }
    _bau->loop();
}

void KNX_manufacturerId(uint16_t value)
{
    _bau->deviceObject().manufacturerId(value);
}

void KNX_bauNumber(uint32_t value)
{
    _bau->deviceObject().bauNumber(value);
}

void KNX_orderNumber(const uint8_t* value)
{
    _bau->deviceObject().orderNumber(value);
}

void KNX_hardwareType(const uint8_t* value)
{
    _bau->deviceObject().hardwareType(value);
}

void KNX_version(uint16_t value)
{
    _bau->deviceObject().version(value);
}

void KNX_start()
{
    //EDA Use HAL_GPIO_EXTI_Callback!!!!!
    attachInterrupt(digitalPinToInterrupt(PA0), KNX_buttonEvent, CHANGE);
    KNX_enabled(true);
}


uint8_t* KNX_paramData(uint32_t addr)
{
    if (!_bau->configured())
        return nullptr;

    return _bau->parameters().data(addr);
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
bool KNX_paramBit(uint32_t addr, uint8_t shift)
{
    if (!_bau->configured())
        return 0;

    return (bool) ((_bau->parameters().getByte(addr) >> (7-shift)) & 0x01); 
}

uint8_t KNX_paramByte(uint32_t addr)
{
    if (!_bau->configured())
        return 0;

    return _bau->parameters().getByte(addr);
}

// Same usage than paramByte(addresse) for signed parameters
// Declaration in XML file
// <ParameterType Id="M-00FA_A-0066-EA-0001_PT-delta" Name="delta">
//   <TypeNumber SizeInBit="8" Type="signedInt" minInclusive="-10" maxInclusive="10"/>
// </ParameterType>
int8_t KNX_paramSignedByte(uint32_t addr)
{
    if (!_bau->configured())
        return 0;

    return (int8_t) _bau->parameters().getByte(addr);
}

uint16_t KNX_paramWord(uint32_t addr)
{
    if (!_bau->configured())
        return 0;

    return _bau->parameters().getWord(addr);
}

uint32_t KNX_paramInt(uint32_t addr)
{
    if (!_bau->configured())
        return 0;

    return _bau->parameters().getInt(addr);
}

double KNX_paramFloat(uint32_t addr, ParameterFloatEncodings enc)
{
    if (!_bau->configured())
        return 0;

    return _bau->parameters().getFloat(addr, enc);
}

#if (MASK_VERSION == 0x07B0) || (MASK_VERSION == 0x27B0) || (MASK_VERSION == 0x57B0)
GroupObject& KNX_getGroupObject(uint16_t goNr)
{
    return _bau->groupObjectTable().get(goNr);
}
#endif

void KNX_restart(uint16_t individualAddress)
{
    SecurityControl sc = {false, None};
    _bau->restartRequest(individualAddress, sc);
}

void KNX_beforeRestartCallback(BeforeRestartCallback func)
{
    _bau->beforeRestartCallback(func);
}

BeforeRestartCallback KNX_beforeRestartCallback()
{
    return _bau->beforeRestartCallback();
}

void KNX_progLedOn()
{
    HAL_GPIO_WritePin(_ledPin.GPIOx, _ledPin.GPIO_Pin, _ledPinActiveOn);
}

void KNX_progLedOff()
{
    if(_ledPinActiveOn == GPIO_PIN_SET){
        HAL_GPIO_WritePin(_ledPin.GPIOx, _ledPin.GPIO_Pin, GPIO_PIN_RESET);
    }
    else{
        HAL_GPIO_WritePin(_ledPin.GPIOx, _ledPin.GPIO_Pin, GPIO_PIN_SET);
    }
}

void KNX_buttonEvent()
{
    static uint32_t lastEvent=0;
    static uint32_t lastPressed=0;

    uint32_t diff = HAL_GetTick() - lastEvent;
    if (diff >= PROG_BTN_PRESS_MIN_MILLIS && diff <= PROG_BTN_PRESS_MAX_MILLIS){
        if (HAL_GetTick() - lastPressed > 200)
        {  
            KNX_toggleProgMode();
            
            lastPressed = HAL_GetTick();
        }
    }
    lastEvent = HAL_GetTick();
}

  
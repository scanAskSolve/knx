#include "arduino_platform.h"
#include "Print_Function.h"
#include "knx/bits.h"
#include "knx/config.h"
#include "knx/bau_systemB.h"

#ifndef USERDATA_SAVE_SIZE
#define USERDATA_SAVE_SIZE 0
#endif

#define PROG_BTN_PRESS_MIN_MILLIS 50
#define PROG_BTN_PRESS_MAX_MILLIS 500

#define KNX_LED_ACTIVE_ON GPIO_PIN_RESET
#define KNX_BUTTON -1

typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} GPIO_infoTypeDef;
// void KNX_initKnxFacade(HardwareSerial* knxSerial);
void KNX_initKnxFacade();

bool KNX_enabled();

void KNX_enabled(bool value);

bool KNX_progMode();

void KNX_progMode(bool value);

/**
 * To be called by ISR handling on button press.
 */
void KNX_toggleProgMode();

bool KNX_configured();

/**
 * returns HIGH if led is active on HIGH, LOW otherwise
 */
GPIO_PinState KNX_ledPinActiveOn();

/**
 * Sets if the programming led is active on HIGH or LOW.
 *
 * Set to HIGH for GPIO--RESISTOR--LED--GND or to LOW for GPIO--LED--RESISTOR--VDD
 */
void KNX_ledPinActiveOn(GPIO_PinState value);

GPIO_infoTypeDef KNX_ledPin();

void KNX_ledPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

GPIO_infoTypeDef KNX_buttonPin();

void KNX_buttonPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void KNX_readMemory();

void KNX_writeMemory();

uint16_t KNX_individualAddress();

void KNX_loop();

void KNX_manufacturerId(uint16_t value);

void KNX_bauNumber(uint32_t value);

void KNX_orderNumber(const uint8_t *value);

void KNX_hardwareType(const uint8_t *value);

void KNX_version(uint16_t value);

void KNX_start();

uint8_t *KNX_paramData(uint32_t addr);

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
bool KNX_paramBit(uint32_t addr, uint8_t shift);

uint8_t KNX_paramByte(uint32_t addr);

// Same usage than paramByte(addresse) for signed parameters
// Declaration in XML file
// <ParameterType Id="M-00FA_A-0066-EA-0001_PT-delta" Name="delta">
//   <TypeNumber SizeInBit="8" Type="signedInt" minInclusive="-10" maxInclusive="10"/>
// </ParameterType>
int8_t KNX_paramSignedByte(uint32_t addr);

uint16_t KNX_paramWord(uint32_t addr);

uint32_t KNX_paramInt(uint32_t addr);

double KNX_paramFloat(uint32_t addr, ParameterFloatEncodings enc);

#if (MASK_VERSION == 0x07B0) || (MASK_VERSION == 0x27B0) || (MASK_VERSION == 0x57B0)
GroupObject &KNX_getGroupObject(uint16_t goNr);
#endif

void KNX_restart(uint16_t individualAddress);

void KNX_beforeRestartCallback(BeforeRestartCallback func);

BeforeRestartCallback KNX_beforeRestartCallback();

void KNX_progLedOn();

void KNX_progLedOff();

void KNX_buttonEvent();

/*#ifdef ARDUINO_ARCH_STM32
    // predefined global instance for TP only
    #if MASK_VERSION == 0x07B0
        extern KnxFacade knx;
        //extern KnxFacade<Stm32Platform, BauSystemB> knx;
    #else
        #error "Mask version not supported on ARDUINO_ARCH_STM32"
    #endif
#else // Non-Arduino platforms and Linux platform
    // no predefined global instance
    #error "no predefined global instance"
#endif*/
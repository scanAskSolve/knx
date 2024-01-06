#pragma once

#include "arduino_platform.h"

#include "knx/bits.h"
#include "knx/config.h"
#include "knx/bau_systemB.h"

#ifndef USERDATA_SAVE_SIZE
#define USERDATA_SAVE_SIZE 0
#endif

/*
#ifdef ARDUINO_ARCH_STM32
    #include "stm32_platform.h"
#endif*/

/*#ifndef KNX_LED
    #define KNX_LED LED_BUILTIN
#endif*/
#ifndef KNX_LED_ACTIVE_ON
    #define KNX_LED_ACTIVE_ON GPIO_PIN_RESET
#endif
#ifndef KNX_BUTTON
    #define KNX_BUTTON -1
#endif

typedef const uint8_t* (*RestoreCallback)(const uint8_t* buffer);
typedef uint8_t* (*SaveCallback)(uint8_t* buffer);
//typedef void (*IsrFunctionPtr)();
//typedef void (*ProgLedOnCallback)();
//typedef void (*ProgLedOffCallback)();

typedef struct
{
    GPIO_TypeDef *GPIOx; 
    uint16_t GPIO_Pin;
} GPIO_infoTypeDef;

class ArduinoPlatform;
class BauSystemB;

// class KnxFacade : private DeviceObject
class KnxFacade 
{
  public:

    KnxFacade();

    void initKnxFacade(HardwareSerial* knxSerial);


    //void Set_chip_platform(HardwareSerial* PORT);

    ArduinoPlatform& platform();

    BauSystemB& bau();

    bool enabled();

    void enabled(bool value);

    bool progMode();

    void progMode(bool value);

    /**
     * To be called by ISR handling on button press.
     */
    void toggleProgMode();

    bool configured();

    /**
     * returns HIGH if led is active on HIGH, LOW otherwise
     */
    GPIO_PinState ledPinActiveOn();

    /**
     * Sets if the programming led is active on HIGH or LOW. 
     * 
     * Set to HIGH for GPIO--RESISTOR--LED--GND or to LOW for GPIO--LED--RESISTOR--VDD
     */
    void ledPinActiveOn(GPIO_PinState value);

    //uint32_t ledPin();
    GPIO_infoTypeDef ledPin();

    //void ledPin(uint32_t value);
    void ledPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

    //void setProgLedOffCallback(ProgLedOffCallback progLedOffCallback);

    //void setProgLedOnCallback(ProgLedOnCallback progLedOnCallback);

  
    GPIO_infoTypeDef buttonPin();

    void buttonPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

    void readMemory();

    void writeMemory();

    uint16_t individualAddress();

    void loop();

    void manufacturerId(uint16_t value);

    void bauNumber(uint32_t value);
    
    void orderNumber(const uint8_t* value);

    void hardwareType(const uint8_t* value);
    
    void version(uint16_t value);

    void start();

    //void setButtonISRFunction(IsrFunctionPtr progButtonISRFuncPtr);

    void setSaveCallback(SaveCallback func);

    void setRestoreCallback(RestoreCallback func);

    uint8_t* paramData(uint32_t addr);

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
    bool paramBit(uint32_t addr, uint8_t shift);

    uint8_t paramByte(uint32_t addr);
    
    // Same usage than paramByte(addresse) for signed parameters
    // Declaration in XML file
    // <ParameterType Id="M-00FA_A-0066-EA-0001_PT-delta" Name="delta">
    //   <TypeNumber SizeInBit="8" Type="signedInt" minInclusive="-10" maxInclusive="10"/>
    // </ParameterType>
    int8_t paramSignedByte(uint32_t addr);
 
    uint16_t paramWord(uint32_t addr);

    uint32_t paramInt(uint32_t addr);

    double paramFloat(uint32_t addr, ParameterFloatEncodings enc);
    
#if (MASK_VERSION == 0x07B0) || (MASK_VERSION == 0x27B0) || (MASK_VERSION == 0x57B0)
    GroupObject& getGroupObject(uint16_t goNr);
#endif

    void restart(uint16_t individualAddress);

    void beforeRestartCallback(BeforeRestartCallback func);

    BeforeRestartCallback beforeRestartCallback();

  //private:
    //ArduinoPlatform* _platformPtr  = new ArduinoPlatform(&huart2);

    ArduinoPlatform* _platformPtr  = new ArduinoPlatform(&KNX_SERIAL);
    // ArduinoPlatform* _platformPtr ; 

    //ArduinoPlatform* _platformPtr  = nullptr;
    //BauSystemB* _bauPtr = new BauSystemB(*_platformPtr);
    //BauSystemB* _bauPtr;
    //BauSystemB& _bau = *_bauPtr;

    BauSystemB& _bau = *new BauSystemB(*_platformPtr);
    // BauSystemB& _bau ;

    //BauSystemB& _bau = *new BauSystemB();
    //BauSystemB& _bau;
    //ProgLedOnCallback _progLedOnCallback = 0;
    //ProgLedOffCallback _progLedOffCallback = 0;
    GPIO_PinState _ledPinActiveOn = KNX_LED_ACTIVE_ON;
    //uint32_t _ledPin = KNX_LED;
    GPIO_infoTypeDef _ledPin;
    //int32_t _buttonPin = KNX_BUTTON;
    GPIO_infoTypeDef _buttonPin;

    SaveCallback _saveCallback = 0;
    RestoreCallback _restoreCallback = 0;
    volatile bool _toggleProgMode = false;
    bool _progLedState = false;
    uint16_t _saveSize = USERDATA_SAVE_SIZE;
    //IsrFunctionPtr _progButtonISRFuncPtr = 0;

    uint8_t* save(uint8_t* buffer);

    const uint8_t* restore(const uint8_t* buffer);

    uint16_t saveSize();

    void saveSize(uint16_t size);

    void progLedOn();

    void progLedOff();
};


void buttonEvent();

#ifdef ARDUINO_ARCH_STM32
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
#endif
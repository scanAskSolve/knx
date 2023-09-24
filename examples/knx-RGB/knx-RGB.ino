#include <knx.h>

// create named references for easy access to group objects
#define SWITCH1 knx.getGroupObject(1)
long lastsend = 0;
bool LED_S = 0;
bool flag = 0;

HardwareSerial Serial2(USART2);   // PA3  (RX)  PA2  (TX)

void TEST_Function() {
  long now = HAL_GetTick();
  if ((now - lastsend) < 100) 
    return;

  lastsend = now;

  // write new value to groupobject
  if(flag == 0){
    uint8_t RR = knx.paramByte(0);
    RR >>= 7;
    RR &= 0x01;

    SWITCH1.valueNoSend(RR);
    //SWITCH1.valueNoSend((knx.paramByte(0) >> 7) & 0x01);
    flag=1;
  }

    bool LED_NOW = SWITCH1.value();
    if(LED_NOW != LED_S){
    LED_S = LED_NOW;
    digitalWrite(PB11,LED_S);
    //Serial.print("LED_S: ");
    //Serial.println(LED_S);
  }

    
}



void setup() {
  //Serial.setRx(PA10);
  //Serial.setTx(PA9);
  
  Serial.begin(115200);
  ArduinoPlatform::SerialDebug = &Serial;
  
 


    pinMode(PB11,OUTPUT);
    //digitalWrite(27,knx.paramByte(0) && 0x80);

    pinMode(PB11,OUTPUT);

  randomSeed(HAL_GetTick());


  // read adress table, association table, groupobject table and parameters from eeprom
  knx.readMemory();

  // print values of parameters if device is already configured
  if (knx.configured()) {
    // register callback for reset GO
      Serial.println("configured START");
      //goCurrent.dataPointType(DPT_Value_Temp);
      //SWITCH.callback(switchCallback);
      //LED.dataPointType(DPT_Switch);
      SWITCH1.dataPointType(DPT_Switch);
    /**/

    
    
    Serial.print("knx.paramByte(0): ");
    Serial.println(knx.paramByte(0));
    Serial.print("knx.paramByte(1): ");
    Serial.println(knx.paramByte(1));
    Serial.print("knx.paramByte(2): ");
    Serial.println(knx.paramByte(2));
    Serial.print("knx.paramByte(3): ");
    Serial.println(knx.paramByte(3));
    Serial.print("knx.paramByte(4): ");
    Serial.println(knx.paramByte(4));


  }
  Serial.println("configured PASS");

  // pin or GPIO the programming led is connected to. Default is LED_BUILTIN
   knx.ledPin(LED_BUILTIN);
  // is the led active on HIGH or low? Default is LOW
   knx.ledPinActiveOn(HIGH);
  // pin or GPIO programming button is connected to. Default is 0
   knx.buttonPin(PA0);

  // start the framework.
  knx.start();
  
  Serial.println("knx.start");
}

void loop() {
  // don't delay here to much. Otherwise you might lose packages or mess up the timing with ETS
  knx.loop();

  // only run the application code if the device was configured with ETS
  if (!knx.configured())
    return;
  TEST_Function();

}
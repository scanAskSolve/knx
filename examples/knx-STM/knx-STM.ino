//#include <Arduino.h>
#include <knx.h>

// create named references for easy access to group objects
#define SWITCH1 knx.getGroupObject(1)
long lastsend = 0;
bool LED_S = 0;
bool flag = 0;

HardwareSerial Serial2(USART2);   // PA3  (RX)  PA2  (TX)
static void MX_GPIO_Init(void);

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
    if(LED_S)
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
    else
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
    Serial.print("LED_S: ");
    Serial.println(LED_S);
  }

    
}



void setup() {
  //Serial.setRx(PA10);
  //Serial.setTx(PA9);
  
  Serial.begin(115200);
  ArduinoPlatform::SerialDebug = &Serial;
  MX_GPIO_Init();
 
    //pinMode(PB11, OUTPUT);
    /*for(int i = 0; i < 5;i++){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        delay(1000);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        delay(1000);
    }*/

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
   knx.ledPin(GPIOC, GPIO_PIN_13);
  // is the led active on HIGH or low? Default is LOW
   knx.ledPinActiveOn(GPIO_PIN_SET);
  // pin or GPIO programming button is connected to. Default is 0
   knx.buttonPin(GPIOA, GPIO_PIN_0);

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

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/*void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin){
  if(GPIO_Pin == GPIO_PIN_0){
    buttonEvent();
  }
}*/
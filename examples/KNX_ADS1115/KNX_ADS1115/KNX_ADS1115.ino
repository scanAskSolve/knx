#include <knx.h>
#include "ADS1X15.h"
#include "Stdbool.h"
// create named references for easy access to group objects
#define CH1_Value KNX_getGroupObject(1)
#define CH2_Value KNX_getGroupObject(2)
#define CH3_Value KNX_getGroupObject(3)
#define CH4_Value KNX_getGroupObject(4)
#define CH1_Upper_Limit_Flag KNX_getGroupObject(5)
#define CH1_Upper_Warning_Limit_Flag KNX_getGroupObject(6)
#define CH1_Lower_Warning_Limit_Flag KNX_getGroupObject(7)
#define CH1_Lower_Limit_Flag KNX_getGroupObject(8)
#define CH2_Upper_Limit_Flag KNX_getGroupObject(9)
#define CH2_Upper_Warning_Limit_Flag KNX_getGroupObject(10)
#define CH2_Lower_Warning_Limit_Flag KNX_getGroupObject(11)
#define CH2_Lower_Limit_Flag KNX_getGroupObject(12)
#define CH3_Upper_Limit_Flag KNX_getGroupObject(13)
#define CH3_Upper_Warning_Limit_Flag KNX_getGroupObject(14)
#define CH3_Lower_Warning_Limit_Flag KNX_getGroupObject(15)
#define CH3_Lower_Limit_Flag KNX_getGroupObject(16)
#define CH4_Upper_Limit_Flag KNX_getGroupObject(17)
#define CH4_Upper_Warning_Limit_Flag KNX_getGroupObject(18)
#define CH4_Lower_Warning_Limit_Flag KNX_getGroupObject(19)
#define CH4_Lower_Limit_Flag KNX_getGroupObject(20)

// Offset: 0, Size: 1 Bit, Text: CH1 Enable Select
#define APP_CH1_Enable KNX_paramBit(0, 0)
// Offset: 0, BitOffset: 1, Size: 1 Bit, Text: CH2 Enable Select
#define APP_CH2_Enable KNX_paramBit(0, 1)
// Offset: 0, BitOffset: 2, Size: 1 Bit, Text: CH3 Enable Select
#define APP_CH3_Enable KNX_paramBit(0, 2)
// Offset: 0, BitOffset: 3, Size: 1 Bit, Text: CH4 Enable Select
#define APP_CH4_Enable KNX_paramBit(0, 3)

#define PT_Enable_bit_Disable 0
#define PT_Enable_bit_Enable 1


#define PT_Manual_Auto_bit_Disable 0
#define PT_Manual_Auto_bit_Enable 1

#define PT_Auto_Mode_delay_1s 0
#define PT_Auto_Mode_delay_3s 1
#define PT_Auto_Mode_delay_5s 2
#define PT_Auto_Mode_delay_10s 3



struct ADS_Setting {
  bool Enable;
  uint8_t Upper_Limit;
  uint8_t Upper_Warning_Limit;
  uint8_t Lower_Warning_Limit;
  uint8_t Lower_Limit;
  uint8_t requestADC_value;
};



long lastsend = 0;
long now = 0;

bool Transmit_Mode = 0;
int Auto_Mode_delay = 0;
int Now_Read_State = 0;

ADS1115 ADS(0x48);
ADS_Setting ADC_CH1;
ADS_Setting ADC_CH2;
ADS_Setting ADC_CH3;
ADS_Setting ADC_CH4;


HardwareSerial Serial2(USART2);  // PA3  (RX)  PA2  (TX)

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
bool ADC_Process(ADS_Setting ADS_Setting_CH);

void setup() {
  SystemClock_Config();
  MX_USART1_UART_Init();

  //MX_USART2_UART_Init();

  STM_Print_init(&huart1);
  //KNX_initKnxFacade(&Serial2);
  KNX_UART_Init(&Serial2);
  KNX_initKnxFacade();

  MX_GPIO_Init();

  // read adress table, association table, groupobject table and parameters from eeprom
  KNX_readMemory();
  // print values of parameters if device is already configured
  if (KNX_configured()) {
    // register callback for reset GO
    print("configured START\r\n");

    CH1_Value.dataPointType(DPT_Scaling);
    CH2_Value.dataPointType(DPT_Scaling);
    CH3_Value.dataPointType(DPT_Scaling);
    CH4_Value.dataPointType(DPT_Scaling);


    CH1_Upper_Limit_Flag.dataPointType(DPT_Alarm);
    CH1_Upper_Warning_Limit_Flag.dataPointType(DPT_Alarm);
    CH1_Lower_Warning_Limit_Flag.dataPointType(DPT_Alarm);
    CH1_Lower_Limit_Flag.dataPointType(DPT_Alarm);

    CH2_Upper_Limit_Flag.dataPointType(DPT_Alarm);
    CH2_Upper_Warning_Limit_Flag.dataPointType(DPT_Alarm);
    CH2_Lower_Warning_Limit_Flag.dataPointType(DPT_Alarm);
    CH2_Lower_Limit_Flag.dataPointType(DPT_Alarm);


    CH3_Upper_Limit_Flag.dataPointType(DPT_Alarm);
    CH3_Upper_Warning_Limit_Flag.dataPointType(DPT_Alarm);
    CH3_Lower_Warning_Limit_Flag.dataPointType(DPT_Alarm);
    CH3_Lower_Limit_Flag.dataPointType(DPT_Alarm);


    CH4_Upper_Limit_Flag.dataPointType(DPT_Alarm);
    CH4_Upper_Warning_Limit_Flag.dataPointType(DPT_Alarm);
    CH4_Lower_Warning_Limit_Flag.dataPointType(DPT_Alarm);
    CH4_Lower_Limit_Flag.dataPointType(DPT_Alarm);

    print("knx.paramByte(0): ");
    print(KNX_paramByte(0));
    print("\r\n");
    print("knx.paramByte(1): ");
    print(KNX_paramByte(1));
    print("\r\n");
    print("knx.paramByte(2): ");
    print(KNX_paramByte(2));
    print("\r\n");
    print("knx.paramByte(3): ");
    print(KNX_paramByte(3));
    print("\r\n");
    print("knx.paramByte(4): ");
    print(KNX_paramByte(4));
    print("\r\n");


    if (APP_CH1_Enable == PT_Enable_bit_Enable) {
      ADC_CH1.Enable = PT_Enable_bit_Enable;
      ADC_CH1.Upper_Limit = KNX_paramByte(1);
      ADC_CH1.Upper_Warning_Limit = KNX_paramByte(2);
      ADC_CH1.Lower_Warning_Limit = KNX_paramByte(3);
      ADC_CH1.Lower_Limit = KNX_paramByte(4);
      ADC_CH1.requestADC_value = 0;

      print("ADC_CH1.Enable\r\n");
      print("ADC_CH1.Upper_Limit = ");
      print(ADC_CH1.Upper_Limit);
      print("\r\n");
      print("ADC_CH1.Upper_Warning_Limit = ");
      print(ADC_CH1.Upper_Warning_Limit);
      print("\r\n");
      print("ADC_CH1.Lower_Warning_Limit = ");
      print(ADC_CH1.Lower_Warning_Limit);
      print("\r\n");
      print("ADC_CH1.Lower_Limit = ");
      print(ADC_CH1.Lower_Limit);
      print("\r\n");
    } else {
      ADC_CH1.Enable = PT_Enable_bit_Disable;
    }
    if (APP_CH2_Enable == PT_Enable_bit_Enable) {
      ADC_CH2.Enable = PT_Enable_bit_Enable;

      ADC_CH2.Upper_Limit = KNX_paramByte(5);
      ADC_CH2.Upper_Warning_Limit = KNX_paramByte(6);
      ADC_CH2.Lower_Warning_Limit = KNX_paramByte(7);
      ADC_CH2.Lower_Limit = KNX_paramByte(8);
      ADC_CH2.requestADC_value = 1;
      print("ADC_CH2.Enable\r\n");
      print("ADC_CH2.Enable\r\n");
      print("ADC_CH2.Upper_Limit = ");
      print(ADC_CH2.Upper_Limit);
      print("\r\n");
      print("ADC_CH2.Upper_Warning_Limit = ");
      print(ADC_CH2.Upper_Warning_Limit);
      print("\r\n");
      print("ADC_CH2.Lower_Warning_Limit = ");
      print(ADC_CH2.Lower_Warning_Limit);
      print("\r\n");
      print("ADC_CH2.Lower_Limit = ");
      print(ADC_CH2.Lower_Limit);
      print("\r\n");
    } else {
      ADC_CH2.Enable = PT_Enable_bit_Disable;
    }
    if (APP_CH3_Enable == PT_Enable_bit_Enable) {
      ADC_CH3.Enable = PT_Enable_bit_Enable;
      ADC_CH3.Upper_Limit = KNX_paramByte(9);
      ADC_CH3.Upper_Warning_Limit = KNX_paramByte(10);
      ADC_CH3.Lower_Warning_Limit = KNX_paramByte(11);
      ADC_CH3.Lower_Limit = KNX_paramByte(12);
      ADC_CH3.requestADC_value = 2;
      print("ADC_CH3.Enable\r\n");
      print("ADC_CH3.Enable\r\n");
      print("ADC_CH3.Upper_Limit = ");
      print(ADC_CH3.Upper_Limit);
      print("\r\n");
      print("ADC_CH3.Upper_Warning_Limit = ");
      print(ADC_CH3.Upper_Warning_Limit);
      print("\r\n");
      print("ADC_CH3.Lower_Warning_Limit = ");
      print(ADC_CH3.Lower_Warning_Limit);
      print("\r\n");
      print("ADC_CH3.Lower_Limit = ");
      print(ADC_CH3.Lower_Limit);
      print("\r\n");
    } else {
      ADC_CH3.Enable = PT_Enable_bit_Disable;
    }
    if (APP_CH4_Enable == PT_Enable_bit_Enable) {
      ADC_CH4.Enable = PT_Enable_bit_Enable;
      ADC_CH4.Upper_Limit = KNX_paramByte(13);
      ADC_CH4.Upper_Warning_Limit = KNX_paramByte(14);
      ADC_CH4.Lower_Warning_Limit = KNX_paramByte(15);
      ADC_CH4.Lower_Limit = KNX_paramByte(16);
      ADC_CH4.requestADC_value = 3;
      print("ADC_CH4.Enable\r\n");
      print("ADC_CH4.Enable\r\n");
      print("ADC_CH4.Upper_Limit = ");
      print(ADC_CH4.Upper_Limit);
      print("\r\n");
      print("ADC_CH4.Upper_Warning_Limit = ");
      print(ADC_CH4.Upper_Warning_Limit);
      print("\r\n");
      print("ADC_CH4.Lower_Warning_Limit = ");
      print(ADC_CH4.Lower_Warning_Limit);
      print("\r\n");
      print("ADC_CH4.Lower_Limit = ");
      print(ADC_CH4.Lower_Limit);
      print("\r\n");
    } else {
      ADC_CH4.Enable = PT_Enable_bit_Disable;
    }


    Transmit_Mode = (KNX_paramByte(0) >> 3) & 0x01;
    if (Transmit_Mode == PT_Manual_Auto_bit_Enable) {
      print("Transmit_Mode = PT_Manual_Auto_bit_Enable\r\n");
      switch ((KNX_paramByte(0) >> 1) & 0x03) {
        case 0:
          Auto_Mode_delay = 1000;
          break;
        case 1:
          Auto_Mode_delay = 3000;
          break;
        case 2:
          Auto_Mode_delay = 5000;
          break;
        case 3:
          Auto_Mode_delay = 10000;
          break;
      }
      print("Auto_Mode_delay = ");
      print((KNX_paramByte(0) >> 1) & 0x03);
    } else {
      print("Transmit_Mode = PT_Manual_Auto_bit_Disable");
      //Auto_Mode_delay = 1000;
    }
    print("\r\n");
  }
  print("configured PASS\r\n");

  // pin or GPIO the programming led is connected to. Default is LED_BUILTIN
  KNX_ledPin(GPIOB, GPIO_PIN_13);
  // is the led active on HIGH or low? Default is LOW
  KNX_ledPinActiveOn(GPIO_PIN_RESET);
  // pin or GPIO programming button is connected to. Default is 0
  KNX_buttonPin(GPIOA, GPIO_PIN_0);

  // start the framework.
  KNX_start();

  print("knx.start\r\n");


  Wire.begin();

  ADS.begin();
  ADS.setGain(8);
  print("EDA\r\n");
  now = HAL_GetTick();
}

void loop() {
  // don't delay here to much. Otherwise you might lose packages or mess up the timing with ETS
  KNX_loop();

  // only run the application code if the device was configured with ETS
  if (!KNX_configured())
    return;

  if (KNX_progMode() == 0) {
    if (ADS.isBusy() == false) {

      if ((HAL_GetTick() - now) >= Auto_Mode_delay) {
        switch (Now_Read_State) {
          case 0:
            if (ADC_Process(ADC_CH1) == 0) {
              now = HAL_GetTick();
            }
            break;
          case 1:
            if (ADC_Process(ADC_CH2) == 0) {
              now = HAL_GetTick();
            }
            break;
          case 2:
            if (ADC_Process(ADC_CH3) == 0) {
              now = HAL_GetTick();
            }
            break;
          case 3:
            if (ADC_Process(ADC_CH4) == 0) {
              now = HAL_GetTick();
            }
            break;
        }
        Now_Read_State++;
        Now_Read_State = (Now_Read_State == 4) ? 0 : Now_Read_State;
      }
    }
  }
}



bool ADC_Process(ADS_Setting ADS_Setting_CH) {
  if (ADS_Setting_CH.Enable == PT_Enable_bit_Enable) {
    ADS.requestADC(ADS_Setting_CH.requestADC_value);
    delay(50);
    int16_t val_0 = ADS.getValue();
    print("Voltage");
    print(ADS_Setting_CH.requestADC_value);
    print(": ");
    float v = val_0 * ADS.toVoltage();
    print(v);




    // uint8_t Real_percent = (uint8_t)((v / 0.47029) * 100);
    // print(" Real_percent: ");
    // print(Real_percent);

    bool NO_SENSOR = 0;
    if (v > 0.5) { //0.47029
      print("  Not connect sensor");
      NO_SENSOR = 1;
    }



    uint8_t percent = (uint8_t)((v / 0.47029) * 100);
    
    if(percent <= 0) percent = 0;
    else if(percent >= 100) percent = 100;
    print(" percent: ");
    print(percent);
    print("\r\n");





    if (ADS_Setting_CH.requestADC_value == 0) {
      if (!NO_SENSOR) {
        CH1_Value.value(percent);    
        print("KNXpercent: ");
        print(percent);
        print("\r\n");


        if (percent >= ADS_Setting_CH.Upper_Warning_Limit) {
          bool REG = 1;
          if (percent >= ADS_Setting_CH.Upper_Limit)
            CH1_Upper_Limit_Flag.value(REG);
          else
            CH1_Upper_Warning_Limit_Flag.value(REG);
        } else if (percent <= ADS_Setting_CH.Lower_Warning_Limit) {
          bool REG = 1;
          if (percent <= ADS_Setting_CH.Lower_Limit)
            CH1_Lower_Limit_Flag.value(REG);
          else
            CH1_Lower_Warning_Limit_Flag.value(REG);
        }
      }
    } else if (ADS_Setting_CH.requestADC_value == 1) {
      if (!NO_SENSOR) {
        CH2_Value.value(percent);

        if (percent >= ADS_Setting_CH.Upper_Warning_Limit) {
          bool REG = 1;
          if (percent >= ADS_Setting_CH.Upper_Limit)
            CH2_Upper_Limit_Flag.value(REG);
          else
            CH2_Upper_Warning_Limit_Flag.value(REG);
        } else if (percent <= ADS_Setting_CH.Lower_Warning_Limit) {
          bool REG = 1;
          if (percent <= ADS_Setting_CH.Lower_Limit)
            CH2_Lower_Limit_Flag.value(REG);
          else
            CH2_Lower_Warning_Limit_Flag.value(REG);
        }
      }
    } else if (ADS_Setting_CH.requestADC_value == 2) {
      if (!NO_SENSOR) {
        CH3_Value.value(percent);

        if (percent >= ADS_Setting_CH.Upper_Warning_Limit) {
          bool REG = 1;
          if (percent >= ADS_Setting_CH.Upper_Limit)
            CH3_Upper_Limit_Flag.value(REG);
          else
            CH3_Upper_Warning_Limit_Flag.value(REG);
        } else if (percent <= ADS_Setting_CH.Lower_Warning_Limit) {
          bool REG = 1;
          if (percent <= ADS_Setting_CH.Lower_Limit)
            CH3_Lower_Limit_Flag.value(REG);
          else
            CH3_Lower_Warning_Limit_Flag.value(REG);
        }
      }
    } else if (ADS_Setting_CH.requestADC_value == 3) {
      if (!NO_SENSOR) {
        CH4_Value.value(percent);

        if (percent >= ADS_Setting_CH.Upper_Warning_Limit) {
          bool REG = 1;
          if (percent >= ADS_Setting_CH.Upper_Limit)
            CH4_Upper_Limit_Flag.value(REG);
          else
            CH4_Upper_Warning_Limit_Flag.value(REG);
        } else if (percent <= ADS_Setting_CH.Lower_Warning_Limit) {
          bool REG = 1;
          if (percent <= ADS_Setting_CH.Lower_Limit)
            CH4_Lower_Limit_Flag.value(REG);
          else
            CH4_Lower_Warning_Limit_Flag.value(REG);
        }
      }
    }
    return 0;
  } else {
    return 1;
  }
}


static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void) {

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void) {

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 19200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}
void HAL_UART_MspInit(UART_HandleTypeDef* huart) {
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };
  if (huart->Instance == USART1) {
    /* USER CODE BEGIN USART1_MspInit 0 */

    /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USER CODE BEGIN USART1_MspInit 1 */

    /* USER CODE END USART1_MspInit 1 */
  } else if (huart->Instance == USART2) {
    /* USER CODE BEGIN USART2_MspInit 0 */

    /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USER CODE BEGIN USART2_MspInit 1 */

    /* USER CODE END USART2_MspInit 1 */
  }
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
  RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }
}
/*void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin){
  if(GPIO_Pin == GPIO_PIN_0){
    buttonEvent();
  }
}*/
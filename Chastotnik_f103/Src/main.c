
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim4;

volatile uint8_t delay=1,revers=0,phase=0;
volatile uint16_t i=0;

static const uint16_t sinus[240]={500,526,552,578,604,630,655,680,704,728,751,774,
795,816,836,855,873,890,906,921,934,947,958,968,
976,983,989,994,997,999,999,998,995,992,986,980,
972,963,952,941,928,913,898,882,864,846,826,806,
784,762,740,716,692,668,643,617,591,565,539,513,
486,460,434,408,382,356,331,307,283,259,237,215,
193,173,153,135,117,101,86,71,58,47,36,27,
19,13,7,4,1,0,0,2,5,10,16,23,
31,41,52,65,78,93,109,126,144,163,183,204,
225,248,271,295,319,344,369,395,421,447,473,500,
500,526,552,578,604,630,655,680,704,728,751,774,
795,816,836,855,873,890,906,921,934,947,958,968,
976,983,989,994,997,999,999,998,995,992,986,980,
972,963,952,941,928,913,898,882,864,846,826,806,
784,762,740,716,692,668,643,617,591,565,539,513,
486,460,434,408,382,356,331,307,283,259,237,215,
193,173,153,135,117,101,86,71,58,47,36,27,
19,13,7,4,1,0,0,2,5,10,16,23,
31,41,52,65,78,93,109,126,144,163,183,204,
225,248,271,295,319,344,369,395,421,447,473,500};

   //for 1000000 clock and 120 point sinus
static const uint16_t freqency[108]={91,92,93,94,95,96,97,98,99,      
100,101,102,103,104,105,106,108,           
109,110,111,112,114,115,116,118,
119,120,122,123,125,127,128,130,
132,133,135,137,139,141,143,145,
147,149,152,154,156,159,161,164,
167,169,172,175,179,182,185,189,
192,196,200,204,208,213,217,222,
227,233,238,244,250,256,263,270,
278,286,294,303,313,323,333,345,
357,370,385,400,417,435,455,476,
500,526,556,588,625,667,714,769,
833,909,1000,1500,2000,2500,3000,3500,4000,5000};

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM1_Init(void);             
void TIM4_IRQHandler(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void TIM4_IRQHandler(void);                                







int main(void)
{
  HAL_Init();
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM4_Init();
  MX_TIM1_Init();
  //soft start for capacitors
  HAL_Delay(1500);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15, GPIO_PIN_SET);
  uint32_t freq=50;
  /* USER CODE END 2 */
  uint8_t stop=1;
  
  
  while (1)
  {
    
    
     if (TIM4->CNT>freqency[freq])
     {
     TIM4->CNT=0;
     }
     TIM4->ARR=freqency[freq];   //period for tim4
     HAL_Delay(100);
      
     //soft start
     if (delay!=1)
     {
      delay--;
     }
      
     // Down Button
     if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8)== GPIO_PIN_SET)&&(freq>1))
     {
        freq--;
     }    
   
     //UP Button
     if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)== GPIO_PIN_SET)&&freq<106)
     {
       freq++;
     }
    
     //stop
     if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)== GPIO_PIN_SET)&&(stop==0))
     {
       stop=1;
       NVIC_DisableIRQ(TIM4_IRQn);
       HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
       HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
       HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
       HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
       HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
       HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3); 
       HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14, GPIO_PIN_RESET);
       HAL_Delay(1000);
       
     }
     //start
     if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)== GPIO_PIN_SET)&&(stop==1))
     {
       stop=0;
       freq=106;
       HAL_TIM_Base_Start_IT(&htim4);
       NVIC_EnableIRQ(TIM4_IRQn); 
       HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14, GPIO_PIN_SET);
       HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
       HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
       HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
       HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
       HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
       HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
       while (freq>50)
       {
          if (TIM4->CNT>freqency[freq])
          {
            TIM4->CNT=0;
          }
          TIM4->ARR=freqency[freq];   //period for tim4
          HAL_Delay(100);
          freq--;
        }
     }
     // Revers Button
       if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)== GPIO_PIN_RESET)
      {
         revers=~revers&0x01;
      }    
     // 3 phase one phase
       if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)== GPIO_PIN_RESET)
      {
         phase=1;
      }else{
         phase=0;
      }    
  }
  /* USER CODE END 3 */

}
//-------------------------interrupt---------------------------

void TIM4_IRQHandler(void)
{
  if (i >119) {
    i=0;
    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
  } 
  //phase1+2 90 degres
  if (phase==1){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,sinus[i]);  
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,sinus[i+30]);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3,sinus[i+60]);
  }
  //phase1+2+3 120 degres
  if (phase==0){
    if (revers)
    {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,sinus[i]);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,sinus[i+40]);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3,sinus[i+80]);
    }
    else
    {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,sinus[i+80]);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,sinus[i+40]);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3,sinus[i]);
    }
  }
  i++;
  __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_BREAK);
  HAL_TIM_IRQHandler(&htim4);
}

//--------------------------------------------------------------


void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  //RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* TIM1 init function */
static void MX_TIM1_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 9;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 3;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_ENABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim1);

}

/* TIM4 init function */
static void MX_TIM4_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 71;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 5000;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 PB6 
                           PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6 
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

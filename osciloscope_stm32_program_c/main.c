/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include <string.h>
#include <stdbool.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim5;

/* USER CODE BEGIN PV */
const uint16_t Delay = 3;
const uint16_t size_of_array_u = 2048;											//размер одного массива с данными
const uint16_t value_of_cikles  = size_of_array_u-1;			//размер одного массива с данными, с учетом счетчика от 0
const uint16_t size_of_transmit_arr = size_of_array_u*2;	//размер для отправки. U1_H, U1_L; U2_H, U2_L;

volatile uint16_t ADC_Data_u1[size_of_array_u];				//Переменная для храниния значения с АЦП
volatile uint16_t ADC_Data_u2[size_of_array_u];						//Переменная для храниния значения с АЦП

uint16_t ip = 0;

uint16_t i = 0;
uint16_t f = 0;

uint16_t f_2 = 0;
uint16_t i_1 = 0;

uint16_t f_2_L = 0;
uint16_t f_2_H = 0;

uint16_t i_1_L = 0;
uint16_t i_1_H = 0;

uint8_t trans_val = 0;

uint16_t value_u1 = 0;
uint16_t value_u2 = 0;

volatile uint16_t u1_HIGH[size_of_array_u];
volatile uint16_t u1_LOW[size_of_array_u];

volatile uint16_t u2_HIGH[size_of_array_u];
volatile uint16_t u2_LOW[size_of_array_u];

volatile uint8_t u1_transmit[size_of_transmit_arr];
volatile uint8_t u2_transmit[size_of_transmit_arr];

int btn_prev = 0;						//предыдущее состояние кнопки
int btn_cur = 0;						//текущее состояние кнопки
bool btn = 0;								//Вспомогательная переменная, начальное значение False
volatile bool ADC_ready = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM5_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USB_DEVICE_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start(&htim5);
	HAL_ADCEx_InjectedStart_IT(&hadc1);	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
  {
    /* USER CODE END WHILE */
		btn_cur = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);				//Считываем состояние кнопки
	  if ((btn_prev == 0) && (btn_cur != 0)) 
		{																										//Если предыдущие состояние 0, т.е. кнопка не была нажата, и текущее состояние не равно 0, т.е. кнопка нажата
		 btn = !btn;																				//В этом случае меняем состояние переменной a на True
		}
	  btn_prev = btn_cur;																	//Запоминаем текущее состояние кнопки, чтобы исключить ложное срабатывание светодиода, если кнопка останется зажатой
	  if (btn==1) 																				//Теперь, если переменная a = True, то
		{
//-------------------------------------------------------------------------------------------------		
			if (ADC_ready == 1)
			{			
					value_u1=2;
					value_u2=2;
					HAL_TIM_Base_Stop(&htim5);   //Отключение Таймера TIM5
					HAL_ADCEx_InjectedStop_IT(&hadc1);	
				
			//--u1 array--------------------------------------------------------------------------------------//
					for (i_1_H = 0; i_1_H < value_of_cikles; i_1_H++) u1_HIGH[i_1_H] = ADC_Data_u1[i_1_H] / 256;	//
					for (i_1_L = 0; i_1_L < value_of_cikles; i_1_L++) u1_LOW[i_1_L] = ADC_Data_u1[i_1_L] % 256;		//
					u1_transmit[0]=111;																																						// метка
					u1_transmit[1]=111;																																						//
					for (i_1 = 0; i_1 < value_of_cikles; i_1++)																										//
					{																																															//	
						u1_transmit[value_u1++] = u1_HIGH[i_1];																											//	
						u1_transmit[value_u1++] = u1_LOW[i_1];																											//	
					}																																															//	
			//-----Transmit_u1--------------------------------------------------------------------------------//
				CDC_Transmit_FS((uint8_t*)u1_transmit, size_of_transmit_arr);
				HAL_Delay(Delay);
			//--u2 array--------------------------------------------------------------------------------------//
					for (f_2_H = 0; f_2_H < value_of_cikles; f_2_H++) u2_HIGH[f_2_H] = ADC_Data_u2[f_2_H] / 256;	//
					for (f_2_L = 0; f_2_L < value_of_cikles; f_2_L++) u2_LOW[f_2_L] = ADC_Data_u2[f_2_L] % 256;		//
					u2_transmit[0]=222;																																						// метка
					u2_transmit[1]=222;																																						//
					for (f_2 = 0; f_2 < value_of_cikles; f_2++)																										//
					{																																															//
						u2_transmit[value_u2++] = u2_HIGH[f_2];																											//	
						u2_transmit[value_u2++] = u2_LOW[f_2];																											//
					}																																															//
				//-----Transmit_u2------------------------------------------------------------------------------//
				CDC_Transmit_FS((uint8_t*)u2_transmit, size_of_transmit_arr);
				HAL_Delay(Delay);
				//-----Transmit_u1------------------------------------------------------------------------------//	
				ADC_ready = 0;					
				HAL_TIM_Base_Start(&htim5);
				HAL_ADCEx_InjectedStart_IT(&hadc1);
				}
				//----------------------------------------------------------------------------------------------------				
				else
				{
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);																													//Изменяем состояние пина LED, к которому подключен светодиод*/
				HAL_Delay(Delay);
				}
//-------------------------------------------------------------------------------------------------					
	  } 
		else
		{
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);																						//Если переменная a=False, то светодиод должен погаснуть
		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};
  ADC_InjectionConfTypeDef sConfigInjected = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_1;
  sConfigInjected.InjectedRank = 1;
  sConfigInjected.InjectedNbrOfConversion = 2;
  sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_3CYCLES;
  sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONVEDGE_RISING;
  sConfigInjected.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJECCONV_T5_TRGO;
  sConfigInjected.AutoInjectedConv = DISABLE;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.InjectedOffset = 0;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_2;
  sConfigInjected.InjectedRank = 2;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */
}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 8400-1;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc1)				//обработчик прерываний АЦП
{
		if (ip < value_of_cikles)
		{	ADC_Data_u1[ip]=HAL_ADCEx_InjectedGetValue(hadc1,ADC_INJECTED_RANK_1);
			ADC_Data_u2[ip]=HAL_ADCEx_InjectedGetValue(hadc1,ADC_INJECTED_RANK_2);
			if (ip >= (value_of_cikles-1))
			{
				ADC_ready = 1;
				//	HAL_TIM_Base_Stop(&htim5);   //Отключение Таймера TIM5
			}
			ip++;
		}
		
		else	
		ip=0;
		__NOP();		
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

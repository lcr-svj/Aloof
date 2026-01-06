/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include<stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t receiveData[200];
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/*小灯闪烁任务*/
void led_blink(void){
	while(1){
		HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, SET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(led_GPIO_Port,led_Pin, RESET);
		HAL_Delay(500);
	}
}

// 串口接受完成中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	GPIO_PinState state = GPIO_PIN_SET;
	HAL_UART_Transmit_DMA(&huart1, receiveData, 2);
	if (receiveData[1] == '0')
    {
	  state = GPIO_PIN_RESET;
	}
	if (receiveData[1] == '1')
	{
	  state = GPIO_PIN_SET;
	}

	HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, state);
    HAL_UART_Receive_DMA(&huart1, receiveData, 2);
}

//串口接法任务
void uart(void){
	//char message[] = "Hello world";
	uint8_t receivedata[2];
	while(1){
		HAL_UART_Receive(&huart1, receivedata, 2, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart1, receivedata, 2, 100);
		HAL_Delay(1000);

		GPIO_PinState state = GPIO_PIN_SET;
		if (receivedata[1] == '0')
		{
		  state = GPIO_PIN_RESET;
		}
		if (receivedata[1] == '1')
		{
		  state = GPIO_PIN_SET;
		}

		HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, state);
	}
}

//空闲中断回调函数
//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
//	if(huart == &huart1){
//		HAL_UART_Transmit_DMA(huart, receiveData, Size);
//
//		HAL_UARTEx_ReceiveToIdle_DMA(huart, receiveData, sizeof(receiveData));
//		__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
//	}
//}

//3.  串口与中断
void uart_it(void){
	  HAL_UART_Receive_IT(&huart1, receiveData, 2);
	  while(1){
		  HAL_Delay(1000);
	  }
}


//4.  串口DMA与收发不定长数据
void uart_dma(void){
	HAL_UART_Receive_DMA(&huart1, receiveData, 2);
	while(1){
		  HAL_Delay(1000);
	}
}

//5.   空闲中断
void uart_time(void){
	  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, receiveData, sizeof(receiveData));
	  __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
	  while(1){
		  HAL_Delay(1000);
	  }
}


//6.   定时器
void uart_time_start(void){
	  HAL_TIM_Base_Start(&htim4);
	  int counter =0;
	  char message[20];
	  while(1){
		  counter = __HAL_TIM_GET_COUNTER(&htim4);
		  sprintf(message,"counter:%d",counter);
		  HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message),1000);
		  HAL_Delay(1000);
	  }
}

//7.   解析数据包
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	char message1[] = "乖";
	char message2[] = "超级乖";
	char message3[] = "今天宝宝睡觉";
	char message4[] = "今天宝宝吃饭";
	char message5[] = "今天宝宝学习";
	if(huart == &huart1)
	{
		HAL_UART_Transmit_DMA(&huart1, receiveData, Size);

		if(receiveData[0] == 0xAA)
		{
			HAL_UART_Transmit(&huart1, (uint8_t*)"报文头ok\r\n", strlen("报文头ok\r\n"),100);
			uint8_t sum = 0;
			for (int i = 0; i < Size - 1; i++)
			{
			    sum += receiveData[i];
			}
			if (sum == receiveData[Size - 1])
			{
				for(int i = 3;i < Size - 1; i  += 2)
				{
					if(receiveData[i] == 0x00)
					{
						HAL_UART_Transmit(&huart1, (uint8_t*)message1, strlen(message1),100);
						HAL_Delay(1000);
					}
					else if(receiveData[i] == 0x00)
					{
						HAL_UART_Transmit(&huart1, (uint8_t*)message2, strlen(message1),100);
						HAL_Delay(1000);
					}
					if(receiveData[i + 1] == 0x01)
					{
						HAL_UART_Transmit(&huart1, (uint8_t*)message3, strlen(message1),100);
						HAL_Delay(1000);
					}
					else if(receiveData[i + 1] == 0x02)
					{
						HAL_UART_Transmit(&huart1, (uint8_t*)message4, strlen(message1),100);
						HAL_Delay(1000);
					}
					else if(receiveData[i + 1] == 0x03)
					{
						HAL_UART_Transmit(&huart1, (uint8_t*)message5, strlen(message1),100);
						HAL_Delay(1000);
					}
				}
			}
			else
				HAL_UART_Transmit(&huart1, (uint8_t*)"校验位错误\r\n", strlen("校验位错误\r\n"),100);
		}
		else
			HAL_UART_Transmit(&huart1, (uint8_t*)"报文头错误\r\n", strlen("报文头错误\r\n"),100);

		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, receiveData, 200);
		__HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
	}
}
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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Transmit(&huart1, (uint8_t*)"system start\r\n", 20,100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // 1. 小灯闪烁
//  led_blink();

  // 2. 串口
 //uart();


  //3.  串口与中断
 //uart_it();


  //4.  串口DMA与收发不定长数据
 //uart_dma();

  //5.   空闲中断
//uart_time();

  //6.   定时器
 //uart_time_start();


  //7.   报文协议练习
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, receiveData, 200);
   __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
  while(1){
	  HAL_Delay(1000);
  }




    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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

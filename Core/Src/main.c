/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LCD1602.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum {
	false,
	true
} bool;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CLOCK 72000000
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
int calculacte_frequency();
int calculate_duty();
bool check_value_change(int, int);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint16_t duty = 120;
uint16_t period = 239;

bool valueChanged = false;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	char buffer[20] = "INITIALIZATION";
	uint16_t oldPeriod = period;
	uint16_t oldDuty = duty;
	bool valueChanged = false;

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
  MX_TIM3_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim3);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
  lcd_init();

  lcd_send_string(buffer);
  HAL_Delay(3000);

  lcd_clear();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  volatile int buttonPressed = 0;
  volatile int buttonPressedConfidenceLevel = 0;
  volatile int buttonReleasedConfidenceLevel = 0;
  volatile int confidenceThreshold = 20;


  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  /*

	if(HAL_GPIO_ReadPin(BTN4_GPIO_Port, BTN4_Pin) == 0)
	{
		if (buttonPressed == 0)
		{
			if (buttonPressedConfidenceLevel > confidenceThreshold)
			{
				duty += 10;
				buttonPressed = 1;
			}
			else
			{
				buttonPressedConfidenceLevel++;
				buttonReleasedConfidenceLevel = 0;
			}
		}
	}

	else
	{
		if (buttonPressed == 1)
		{
			if (buttonReleasedConfidenceLevel > confidenceThreshold)
			{
				buttonPressed = 0;
			}
			else
			{
				buttonReleasedConfidenceLevel++;
				buttonPressedConfidenceLevel = 0;
			}
		}
	}


	*/


	  __HAL_TIM_SET_AUTORELOAD(&htim2, period);
	  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);


	  valueChanged = check_value_change(oldDuty, oldPeriod);
	  if(valueChanged)
	  {
		  lcd_clear();
		  lcd_put_cur(0, 0);
		  sprintf(buffer, "Freq: %d KHz", calculacte_frequency());
		  lcd_send_string(buffer);
		  oldPeriod = period;

		  lcd_put_cur(1, 0);
		  sprintf(buffer, "Duty: %d", calculate_duty());
		  lcd_send_string(buffer);
		  lcd_send_string("%");
		  oldDuty = duty;
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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == BTN1_Pin)
	{
		if(period > 239)
		{
			if(duty >= period)			// to not overlimit duty cycle
				duty = period--;
			period -= 1;
		}
	}
	else if(GPIO_Pin == BTN2_Pin)
	{
		if(period < 719 )
			period++;
	}
	else if(GPIO_Pin == BTN3_Pin)
	{
		if(duty > 0)
			duty--;
	}
	else if(GPIO_Pin == BTN4_Pin)
	{
		if(duty < period)
			duty++;
	}

}

int calculacte_frequency()
{
	return CLOCK / (period + 1) / 1000;
}

int calculate_duty()
{
	float result;
	result = (float)duty/(float)period * 100;

	return (int)result;
}

bool check_value_change(int oldDuty, int oldPeriod)
{
	if(oldDuty != duty || oldPeriod != period)
		return true;
	else
		return false;

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

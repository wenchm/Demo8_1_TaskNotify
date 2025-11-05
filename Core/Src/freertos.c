/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "usart.h"
#include "adc.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_Show */
osThreadId_t Task_ShowHandle;
const osThreadAttr_t Task_Show_attributes = {
  .name = "Task_Show",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_Show(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_Show */
  Task_ShowHandle = osThreadNew(AppTask_Show, NULL, &Task_Show_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_Show */
/**
  * @brief  Function implementing the Task_Show thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_Show */
void AppTask_Show(void *argument)
{
	/* USER CODE BEGIN AppTask_Show */
	/* Infinite loop */
	uint32_t  notifyValue = 0;
	for(;;)
	{
		uint32_t ulBitsToClearOnEntry=0x00;				//进入时不清除数据
		uint32_t ulBitsToClearOnExit=0xFFFFFFFF;	//退出时清零数据
		BaseType_t result=xTaskNotifyWait(ulBitsToClearOnEntry, ulBitsToClearOnExit,
					&notifyValue, portMAX_DELAY);		//接收任务通知
		if (result==pdTRUE)
		{
			uint32_t tmpValue = notifyValue;				//ADC原始数值
		  	printf("Notify Value = %ld\r\n",tmpValue );

			uint32_t Volt = 3300*tmpValue;				//单位：mV
			Volt = Volt>>12;										//除以2^12
		  	printf("Engineering Value = %ld\r\n",Volt );
		}
	}
	/* USER CODE END AppTask_Show */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance != ADC3)
		return;

	uint32_t adc_value=HAL_ADC_GetValue(hadc);  	//Get ADC Value
	if (Task_ShowHandle != NULL)
	{
		BaseType_t taskWoken=pdFALSE;
		xTaskNotifyFromISR(Task_ShowHandle, adc_value,
				eSetValueWithOverwrite, &taskWoken);	//Transmit ADC
		portYIELD_FROM_ISR(taskWoken);  					//Required, context switch.
	}
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart3,(uint8_t*)&ch,1,0xFFFF);
	return ch;
}
/* USER CODE END Application */


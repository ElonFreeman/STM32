/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "string.h"
#include "tim.h"
#include "usart.h"
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
extern commands;
char IllegalCommand[]="ILLEAGAL_COMMAND";
char lable[]="ON";

enum LIGHT_STATUS {LIGHT_ON=1,LIGHT_OFF=0} light_status;
enum ACTUATION {TURN_ON=1,TURN_OFF=0,INIT=-1} actuation=INIT;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for BT_Transfer */
osThreadId_t BT_TransferHandle;
const osThreadAttr_t BT_Transfer_attributes = {
  .name = "BT_Transfer",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for Servo_Drive */
osThreadId_t Servo_DriveHandle;
const osThreadAttr_t Servo_Drive_attributes = {
  .name = "Servo_Drive",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for status_trans */
osMessageQueueId_t status_transHandle;
const osMessageQueueAttr_t status_trans_attributes = {
  .name = "status_trans"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartBT_Transfer(void *argument);
void Start_Servo_Drive(void *argument);

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

  /* Create the queue(s) */
  /* creation of status_trans */
  status_transHandle = osMessageQueueNew (16, sizeof(uint16_t), &status_trans_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of BT_Transfer */
  BT_TransferHandle = osThreadNew(StartBT_Transfer, NULL, &BT_Transfer_attributes);

  /* creation of Servo_Drive */
  Servo_DriveHandle = osThreadNew(Start_Servo_Drive, NULL, &Servo_Drive_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartBT_Transfer */
/**
* @brief Function implementing the BT_Transfer thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBT_Transfer */
void StartBT_Transfer(void *argument)
{
  /* USER CODE BEGIN StartBT_Transfer */
  /* Infinite loop */
  for(;;)
  {
    HAL_UART_Receive(&huart3,(uint8_t*)commands,sizeof(commands),3);

    if(!strcmp(commands,"LIGHT_ON"))
    {
      light_status=LIGHT_ON;
      HAL_UART_Transmit(&huart2,(uint8_t*)lable,sizeof(lable),2);
    }
    else if(!strcmp(commands,"LIGHT_OFF"))
    {
      light_status=LIGHT_OFF;
    }
    else
    {
      //HAL_UART_Transmit(&huart3,(uint8_t*)IllegalCommand,sizeof(IllegalCommand),3);
    }

    switch (light_status)
    {
      case LIGHT_ON : actuation=TURN_ON; break;
      case LIGHT_OFF : actuation-TURN_OFF; break;
    }

    osDelay(1);
  }
  /* USER CODE END StartBT_Transfer */
}

/* USER CODE BEGIN Header_Start_Servo_Drive */
/**
* @brief Function implementing the Servo_Drive thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_Servo_Drive */
void Start_Servo_Drive(void *argument)
{
  /* USER CODE BEGIN Start_Servo_Drive */
  /* Infinite loop */
  for(;;)
  {
    uint32_t duty=0;

    switch(actuation)
    {
      case TURN_ON : duty=0; break;
      case TURN_OFF : duty=0; break;
    }

    

    //__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,duty);
    osDelay(1);
  }
  /* USER CODE END Start_Servo_Drive */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */


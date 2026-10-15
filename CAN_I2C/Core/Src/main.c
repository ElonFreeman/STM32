/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "icm20948.h"
#include "stm32f1xx_hal_uart.h"
#include <stdio.h>
#include <string.h>
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
CAN_HandleTypeDef hcan;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_CAN_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void init_icm20948()
{
   //whoami
    uint8_t who_am_i=0x00;
    HAL_I2C_Mem_Read(
      &hi2c1,
      ICM20948_ADDR_HAL,
      ICM_REG_WHO_AM_I,
      I2C_MEMADD_SIZE_8BIT,
      &who_am_i,
      sizeof(who_am_i),
      2); //who_am_i will be 0xEA

    //reset
    uint8_t reset=ICM_PWR_DEVICE_RESET;
    HAL_I2C_Mem_Write(
      &hi2c1,
      ICM20948_ADDR_HAL,
      ICM_REG_PWR_MGMT_1,
      I2C_MEMADD_SIZE_8BIT,
      &reset,
      sizeof(reset),
      1);
    HAL_Delay(5);

    //wakeup&select tim
    uint8_t pwr=ICM_CLKSEL_AUTO;
    HAL_I2C_Mem_Write(
      &hi2c1,
      ICM20948_ADDR_HAL,
      ICM_REG_PWR_MGMT_1,
      I2C_MEMADD_SIZE_8BIT,
      &pwr,
      sizeof(pwr),
      1);
    HAL_Delay(5);

    //switch to bank2
    uint8_t bank2=ICM_BANK_2;
    HAL_I2C_Mem_Write(
      &hi2c1,
      ICM20948_ADDR_HAL,
      ICM_REG_REG_BANK_SEL,
      I2C_MEMADD_SIZE_8BIT,
      &bank2,sizeof(bank2),
      1);

    //setup gyroscope&accelerometer
    uint8_t gyro_cfg=ICM_GYRO_FS_2000DPS        // 量程 ±2000dps
         | ICM_GYRO_DLPF_BW_51HZ      // 带宽 51.2Hz
         | ICM_GYRO_FCHOICE_ENABLE;   // 启用 DLPF
    HAL_I2C_Mem_Write(
      &hi2c1,
      ICM20948_ADDR_HAL,
      ICM_REG_GYRO_CONFIG_1,
      I2C_MEMADD_SIZE_8BIT,
      &gyro_cfg,
      sizeof(gyro_cfg),
      1);
      HAL_Delay(1);
    uint8_t accel_cfg= ICM_ACCEL_FS_4G               // 量程 ±4g
          | ICM_ACCEL_DLPF_BW_50HZ        // 带宽 50.4Hz
          | ICM_ACCEL_FCHOICE_ENABLE;     // 启用 DLPF
    HAL_I2C_Mem_Write(
      &hi2c1,
      ICM20948_ADDR_HAL,
      ICM_REG_ACCEL_CONFIG,
      I2C_MEMADD_SIZE_8BIT,
      &accel_cfg,
      sizeof(accel_cfg),
      1);

    //switch back to bank0
    uint8_t bank0=ICM_BANK_0;
    HAL_I2C_Mem_Write(
      &hi2c1,
      ICM20948_ADDR_HAL,
      ICM_REG_REG_BANK_SEL,
      I2C_MEMADD_SIZE_8BIT,
      &bank0,
      sizeof(bank0),
      1);
}



void read_icm20948_data_arr(int16_t *arr)
{
  uint8_t raw[14];
  HAL_I2C_Mem_Read(
    &hi2c1,
    ICM20948_ADDR_HAL,
    ICM_REG_ACCEL_XOUT_H,
    I2C_MEMADD_SIZE_8BIT,
    raw,
    sizeof(raw),
    2);

    for (uint8_t i = 0; i < 7; i++)
    {
        arr[i] = (int16_t)((raw[2*i] << 8) | raw[2*i + 1]);
    }
}
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
  MX_DMA_Init();
  MX_CAN_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  init_icm20948();
  HAL_Delay(1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    int16_t data_raw[7];
    read_icm20948_data_arr(data_raw);

    float data[7];
    for(uint8_t i=0;i<7;i++)
    {
      if(i>=0 && i<=2)
      {
        data[i] = data_raw[i] / 8192.0f;
      }
      else if(i>2 && i<=5)
      {
        data[i] = data_raw[i] / 16.4f;
      }
      else
      {
        data[i] = data_raw[i] / 333.87f + 21.0f;
      }
    }

    printf("ax=%.3f ay=%.3f az=%.3f gx=%.2f gy=%.2f gz=%.2f T=%.2f\r\n",
      data[0],data[1],data[2],data[3],data[4],data[5],data[6]);

    HAL_Delay(20);
    // ±4g → 8192 LSB/g
    // ±2000dps → 16.4 LSB/(°/s)
    // 温度 → 温度(°C) = raw/333.87 + 21

    // float ax_g = ax / 8192.0f;
    // float ay_g = ay / 8192.0f;
    // float az_g = az / 8192.0f;

    // float gx_dps = gx / 16.4f;
    // float gy_dps = gy / 16.4f;
    // float gz_dps = gz / 16.4f;

    // float temp_c = temp / 333.87f + 21.0f;

    //float temp_c = 25.5f;

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

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
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int _write(int fd, char *ptr, int len)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
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

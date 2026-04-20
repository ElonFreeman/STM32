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
#include "i2c.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
  #include "i2c.h"
  #include "usart.h"
  #include "gpio.h"
  #include "stdio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ICM20948_ADDR         0x68
#define WHO_AM_I        0x00    // 读ID，应为0xEA
#define PWR_MGMT_1      0x06    // 电源管理1
#define ACCEL_XOUT_H    0x2D    // 加速度X轴高字节
#define GYRO_XOUT_H     0x33    // 陀螺仪X轴高字节
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t temp=-1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



// 向ICM20948的指定寄存器写入一个字节
void ICM20948_WriteReg(uint8_t reg, uint8_t data) {
    HAL_I2C_Mem_Write(&hi2c1, ICM20948_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}

// 从ICM20948的指定寄存器读出一个字节
uint8_t ICM20948_ReadReg(uint8_t reg) {
    uint8_t data = 0;
    HAL_I2C_Mem_Read(&hi2c1, ICM20948_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
    return data;
}


// 初始化ICM20948
uint8_t ICM20948_Init(void) {
    // 1. 唤醒设备：选择时钟源(推荐使用PLL X轴陀螺参考时钟，值设为1)
    ICM20948_WriteReg(PWR_MGMT_1, 0x01);
    HAL_Delay(100); // 等待时钟源稳定

    // 2. 验证设备：读取WHO_AM_I寄存器
    uint8_t whoami = ICM20948_ReadReg(WHO_AM_I);
    if (whoami != 0xEA) {
        printf("FAIL\r\n");
        return 1; // 设备ID错误，初始化失败
    }

    // 3. 配置加速度计：±2g量程，低噪声模式
    ICM20948_WriteReg(0x14, 0x02); // ACCEL_CONFIG: 量程配置为±2g
    ICM20948_WriteReg(0x15, 0x03); // ACCEL_CONFIG_2: 带宽218.1Hz

    // 4. 配置陀螺仪：±250dps量程
    ICM20948_WriteReg(0x12, 0x03); // GYRO_CONFIG_1: 量程配置为±250dps
    ICM20948_WriteReg(0x13, 0x03); // GYRO_CONFIG_2: 带宽218.1Hz

    return 0; // 初始化成功
}

void ICM20948_ReadAll(int16_t *ax, int16_t *ay, int16_t *az,
                      int16_t *gx, int16_t *gy, int16_t *gz,
                      int16_t *temp) {
    uint8_t data[20] = {0};
    
    // 从ACCEL_XOUT_H (0x2D) 开始连续读取14字节
    // 地址顺序: ACCEL_XOUT_H/L, ACCEL_YOUT_H/L, ACCEL_ZOUT_H/L, 
    //          TEMP_OUT_H/L, GYRO_XOUT_H/L, GYRO_YOUT_H/L, GYRO_ZOUT_H/L
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, ICM20948_ADDR << 1, 
                                                 0x2D, I2C_MEMADD_SIZE_8BIT, 
                                                 data, 14, 100);
    
    if(status != HAL_OK) {
        printf("I2C read error: %d\n", status);
        return;
    }
    
    // 合成16位数据（注意字节顺序：高字节在前）
    *ax = (int16_t)((data[0] << 8) | data[1]);
    *ay = (int16_t)((data[2] << 8) | data[3]);
    *az = (int16_t)((data[4] << 8) | data[5]);
    *temp = (int16_t)((data[6] << 8) | data[7]);
    *gx = (int16_t)((data[8] << 8) | data[9]);
    *gy = (int16_t)((data[10] << 8) | data[11]);
    *gz = (int16_t)((data[12] << 8) | data[13]);
}

uint8_t ICM20948_DataReady(void) {
    // 读取状态寄存器
    uint8_t status = ICM20948_ReadReg(0x1C);  // INT_STATUS
    return (status & 0x01);  // RAW_DATA_0_RDY_INT
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
  HAL_Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_I2C3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  /*char msg[]="Hello World!";
  printf("%s",msg);*/
  /* USER CODE END 2 */
 
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */


    if(ICM20948_DataReady())
    {
      ICM20948_ReadAll(&ax, &ay, &az, &gx, &gy, &gz, &temp);
      printf("ACCEL: %6d %6d %6d\r\n", ax, ay, az);
    }
    else
    {
      printf("Data not ready\r\n");
    }
    HAL_Delay(100);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

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

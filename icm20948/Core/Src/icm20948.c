#include "string.h"
#include "i2c.h"
#include "icm20948.h"

struct ICM20948DATA ICMDATA;

/* 静态辅助函数：切换USER BANK */
static void ICM20948_SetBank(uint8_t bank)
{
    uint8_t bank_val = bank;
    HAL_I2C_Mem_Write(&hi2c1, ICM20948_ADDR << 1, REG_USER_BANK,
        I2C_MEMADD_SIZE_8BIT, &bank_val, 1, HAL_MAX_DELAY);
}

/* 从指定寄存器读取一个字节 */
static uint8_t ICM20948_ReadReg(uint8_t reg)
{
    uint8_t data = 0;
    HAL_I2C_Mem_Read(&hi2c1, ICM20948_ADDR << 1, reg,
        I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
    return data;
}

/* 向指定寄存器写入一个字节 */
static void ICM20948_WriteReg(uint8_t reg, uint8_t value)
{
    HAL_I2C_Mem_Write(&hi2c1, ICM20948_ADDR << 1, reg,
        I2C_MEMADD_SIZE_8BIT, &value, 1, HAL_MAX_DELAY);
}

// 文件：icm20948.c
uint8_t ICM20948_Init(void)
{
    uint8_t id = 0;
    
    // 1. 确认芯片ID (检查连接)
    id = ICM20948_ReadReg(REG_WHO_AM_I);
    if (id != ICM20948_WHO_AM_I)
    {
        return 1; // 初始化失败，芯片未找到
    }
    
    // 2. 唤醒传感器
    ICM20948_WriteReg(REG_PWR_MGMT_1, 0x00); // 清除睡眠位
    HAL_Delay(10);
    
    // 3. 配置时钟源 (推荐选择内部振荡器)
    ICM20948_WriteReg(REG_PWR_MGMT_1, 0x01);
    
    // 4. (可选) 配置量程，这里先保持默认
    // 后续章节会继续完善...
    
    return 0; // 初始化成功
}

// 文件：icm20948.c
void ICM20948_ReadAll(struct ICM20948DATA *data) {
    uint8_t buf[14] = {0}; // 存放 加速度(6) + 温度(2) + 陀螺仪(6) = 14 字节
    
    // 1. 批量读取所有传感器数据
    ICM20948_SetBank(0); // 确保在Bank 0
    HAL_I2C_Mem_Read(&hi2c1, ICM20948_ADDR << 1, REG_ACCEL_XOUT_H,
        I2C_MEMADD_SIZE_8BIT, buf, 14, HAL_MAX_DELAY);
    
    // 2. 解析原始数据 (大端模式)
    data->ax = (int16_t)((buf[0] << 8) | buf[1]);
    data->ay = (int16_t)((buf[2] << 8) | buf[3]);
    data->az = (int16_t)((buf[4] << 8) | buf[5]);
    
    data->temp_ori = (int16_t)((buf[6] << 8) | buf[7]);
    
    data->gx = (int16_t)((buf[8] << 8) | buf[9]);
    data->gy = (int16_t)((buf[10] << 8) | buf[11]);
    data->gz = (int16_t)((buf[12] << 8) | buf[13]);
    
    // 3. 转换为物理单位
    // 加速度: ±2g量程 -> 分辨率 = 2/32768 ≈ 0.000061 g/LSB
    data->grav_accel[0] = data->ax * 0.000061f;
    data->grav_accel[1] = data->ay * 0.000061f;
    data->grav_accel[2] = data->az * 0.000061f;
    
    // 陀螺仪: ±250dps量程 -> 分辨率 = 250/32768 ≈ 0.00763 °/s/LSB
    data->pos_angle[0] = data->gx * 0.00763f;
    data->pos_angle[1] = data->gy * 0.00763f;
    data->pos_angle[2] = data->gz * 0.00763f;
    
    // 温度: 公式 = (原始值 - 21) / 333.87 + 21
    data->temp = ((data->temp_ori - 21.0f) / 333.87f) + 21.0f;
}
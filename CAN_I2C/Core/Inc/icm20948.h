/* icm20948.h */
#ifndef ICM20948_H
#define ICM20948_H

#include <stdint.h>

/* I2C 7位地址（AD0=0）*/
#define ICM20948_ADDR_7BIT   0x68
#define ICM20948_ADDR_HAL    (ICM20948_ADDR_7BIT << 1)   // 0xD0

/* ============ 1. 寄存器地址枚举（按 Bank 分类）============ */
typedef enum {
    /* ---- Bank 0 ---- */
    ICM_REG_WHO_AM_I        = 0x00,
    ICM_REG_USER_CTRL       = 0x03,
    ICM_REG_LP_CONFIG       = 0x05,
    ICM_REG_PWR_MGMT_1      = 0x06,
    ICM_REG_PWR_MGMT_2      = 0x07,
    ICM_REG_INT_PIN_CFG     = 0x0F,
    ICM_REG_INT_ENABLE      = 0x10,
    ICM_REG_INT_ENABLE_1    = 0x11,
    ICM_REG_INT_ENABLE_2    = 0x12,
    ICM_REG_INT_ENABLE_3    = 0x13,
    ICM_REG_I2C_MST_STATUS  = 0x17,
    ICM_REG_ACCEL_XOUT_H    = 0x2D,
    ICM_REG_ACCEL_XOUT_L    = 0x2E,
    ICM_REG_ACCEL_YOUT_H    = 0x2F,
    ICM_REG_ACCEL_YOUT_L    = 0x30,
    ICM_REG_ACCEL_ZOUT_H    = 0x31,
    ICM_REG_ACCEL_ZOUT_L    = 0x32,
    ICM_REG_GYRO_XOUT_H     = 0x33,
    ICM_REG_GYRO_XOUT_L     = 0x34,
    ICM_REG_GYRO_YOUT_H     = 0x35,
    ICM_REG_GYRO_YOUT_L     = 0x36,
    ICM_REG_GYRO_ZOUT_H     = 0x37,
    ICM_REG_GYRO_ZOUT_L     = 0x38,
    ICM_REG_TEMP_OUT_H      = 0x39,
    ICM_REG_TEMP_OUT_L      = 0x3A,
    ICM_REG_EXT_SLV_SENS_DATA_00 = 0x3B,

    /* ---- Bank 1 ---- */
    ICM_REG_XA_OFFS_H       = 0x14,
    ICM_REG_XA_OFFS_L       = 0x15,

    /* ---- Bank 2 ---- */
    ICM_REG_GYRO_SMPLRT_DIV = 0x00,
    ICM_REG_GYRO_CONFIG_1   = 0x01,
    ICM_REG_GYRO_CONFIG_2   = 0x02,
    ICM_REG_XG_OFFS_USRH    = 0x03,
    ICM_REG_XG_OFFS_USRL    = 0x04,
    ICM_REG_ACCEL_SMPLRT_DIV_1 = 0x10,
    ICM_REG_ACCEL_SMPLRT_DIV_2 = 0x11,
    ICM_REG_ACCEL_INTEL_CTRL   = 0x12,
    ICM_REG_ACCEL_WOM_THR      = 0x13,
    ICM_REG_ACCEL_CONFIG       = 0x14,
    ICM_REG_ACCEL_CONFIG_2     = 0x15,

    /* ---- Bank 3 ---- */
    ICM_REG_I2C_MST_ODR_CFG  = 0x00,
    ICM_REG_I2C_MST_CTRL     = 0x01,
    ICM_REG_I2C_SLV0_ADDR    = 0x03,
    ICM_REG_I2C_SLV0_REG     = 0x04,
    ICM_REG_I2C_SLV0_CTRL    = 0x05,
    ICM_REG_I2C_SLV4_ADDR    = 0x13,
    ICM_REG_I2C_SLV4_REG     = 0x14,
    ICM_REG_I2C_SLV4_CTRL    = 0x15,
    ICM_REG_I2C_SLV4_DO      = 0x16,
    ICM_REG_I2C_SLV4_DI      = 0x17,

    /* ---- 特殊：Bank 切换 ---- */
    ICM_REG_REG_BANK_SEL     = 0x7F,

} icm20948_reg_t;

/* ============ 2. Bank 选择值 ============ */
typedef enum {
    ICM_BANK_0 = 0x00,
    ICM_BANK_1 = 0x10,
    ICM_BANK_2 = 0x20,
    ICM_BANK_3 = 0x30,
} icm20948_bank_t;

/* ============ 3. PWR_MGMT_1 字段值 ============ */
typedef enum {
    ICM_PWR_DEVICE_RESET = (1 << 7),   // 0x80
    ICM_PWR_SLEEP        = (1 << 6),   // 0x40
    ICM_PWR_LP_EN        = (1 << 5),   // 0x20
    ICM_PWR_TEMP_DIS     = (1 << 3),   // 0x08

    ICM_CLKSEL_INTERNAL  = 0x00,
    ICM_CLKSEL_AUTO      = 0x01,       // 推荐
    ICM_CLKSEL_STOP      = 0x07,
} icm20948_pwr_t;

/* ============ 4. ACCEL_CONFIG 字段值 ============ */
typedef enum {
    ICM_ACCEL_FS_2G  = (0 << 1),   // 0x00
    ICM_ACCEL_FS_4G  = (1 << 1),   // 0x02
    ICM_ACCEL_FS_8G  = (2 << 1),   // 0x04
    ICM_ACCEL_FS_16G = (3 << 1),   // 0x06

    ICM_ACCEL_DLPF_BW_246HZ = (0 << 3),
    ICM_ACCEL_DLPF_BW_111HZ = (2 << 3),
    ICM_ACCEL_DLPF_BW_50HZ  = (3 << 3),
    ICM_ACCEL_DLPF_BW_23HZ  = (4 << 3),
    ICM_ACCEL_DLPF_BW_11HZ  = (5 << 3),
    ICM_ACCEL_DLPF_BW_5HZ   = (6 << 3),

    ICM_ACCEL_FCHOICE_BYPASS = (0 << 0),
    ICM_ACCEL_FCHOICE_ENABLE = (1 << 0),
} icm20948_accel_cfg_t;

/* ============ 5. GYRO_CONFIG_1 字段值 ============ */
typedef enum {
    ICM_GYRO_FS_250DPS  = (0 << 1),
    ICM_GYRO_FS_500DPS  = (1 << 1),
    ICM_GYRO_FS_1000DPS = (2 << 1),
    ICM_GYRO_FS_2000DPS = (3 << 1),

    ICM_GYRO_DLPF_BW_196HZ = (0 << 3),
    ICM_GYRO_DLPF_BW_151HZ = (1 << 3),
    ICM_GYRO_DLPF_BW_119HZ = (2 << 3),
    ICM_GYRO_DLPF_BW_51HZ  = (3 << 3),
    ICM_GYRO_DLPF_BW_23HZ  = (4 << 3),
    ICM_GYRO_DLPF_BW_11HZ  = (5 << 3),
    ICM_GYRO_DLPF_BW_5HZ   = (6 << 3),
    ICM_GYRO_DLPF_BW_361HZ = (7 << 3),

    ICM_GYRO_FCHOICE_BYPASS = (0 << 0),
    ICM_GYRO_FCHOICE_ENABLE = (1 << 0),
} icm20948_gyro_cfg_t;

#endif
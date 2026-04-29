#ifndef _ICM20948_H_
#define _ICM20948_H_

#include "main.h"
#include "stdint.h"

/*ICM-20948*/
#define ICM20948_ADDR 0x68
#define ICM20948_WHO_AM_I 0xEA
//reg_addrs
#define REG_WHO_AM_I     0x00
#define REG_PWR_MGMT_1   0x06 //电源管理
#define REG_ACCEL_XOUT_H 0x2D //加速度起始地址
#define REG_GYRO_XOUT_H  0x33 //姿态起始地址
#define REG_TEMP_OUT_H   0x39 //温度起始地址
#define REG_USER_BANK    0x76 //寄存器区划，分别用于存储加速度、角速度、温度等数据

struct ICM20948DATA
{
    int16_t gx,gy,gz;
    int16_t ax,ay,az;
    int16_t temp_ori;

    float grav_accel[3];
    float pos_angle[3];
    float temp;
};

extern struct ICM20948DATA ICMDATA;

#endif
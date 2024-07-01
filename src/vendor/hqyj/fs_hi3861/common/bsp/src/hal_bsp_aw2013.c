/*
 * Copyright (c) 2023 Beijing HuaQing YuanJian Education Technology Co., Ltd
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hal_bsp_aw2013.h"
#include <unistd.h>
#include <stdlib.h>

#include "hi_gpio.h"
#include "hi_io.h"
#include "hi_i2c.h"
#include "hi_errno.h"

#define RSTR_REG_ADDR 0x00
#define GCR_REG_ADDR 0x01
#define ISR_REG_ADDR 0x02
#define LCTR_REG_ADDR 0x30
#define LCFG0_REG_ADDR 0x31
#define LCFG1_REG_ADDR 0x32
#define LCFG2_REG_ADDR 0x33
#define PWM0_REG_ADDR 0x34
#define PWM1_REG_ADDR 0x35
#define PWM2_REG_ADDR 0x36
#define LED0T0_REG_ADDR 0x37
#define LED0T1_REG_ADDR 0x38
#define LED0T2_REG_ADDR 0x39
#define LED1T0_REG_ADDR 0x3A
#define LED1T1_REG_ADDR 0x3B
#define LED1T2_REG_ADDR 0x3C
#define LED2T0_REG_ADDR 0x3D
#define LED2T1_REG_ADDR 0x3E
#define LED2T2_REG_ADDR 0x3F
#define IADR_REG_ADDR 0x77

#define TIME_RESET  20000  // 20ms

// 向aw2013的寄存器写数据
static uint32_t aw2013_WiteByte(uint8_t regAddr, uint8_t byte)
{
    uint32_t result = 0;
    uint8_t buffer[] = {regAddr, byte};
    hi_i2c_data i2c_data = {0};
    i2c_data.send_buf = buffer;
    i2c_data.send_len = sizeof(buffer);

    result = hi_i2c_write(AW2013_I2C_IDX, AW2013_I2C_ADDR, &i2c_data);
    if (result != HI_ERR_SUCCESS) {
        printf("I2C AW2013 Write result is 0x%x!!!\r\n", result);
        return result;
    }
    return result;
}

// 寄存器PWMx控制RGB灯的红色亮度
uint32_t AW2013_Control_Red(uint8_t PWM_Data)
{
    uint32_t result = 0;
    result = aw2013_WiteByte(PWM1_REG_ADDR, PWM_Data); // R
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 Red PWM1_REG_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }
    return result;
}
// 寄存器PWMx控制RGB灯的绿色亮度
uint32_t AW2013_Control_Green(uint8_t PWM_Data)
{
    uint32_t result = 0;
    result = aw2013_WiteByte(PWM0_REG_ADDR, PWM_Data); // G
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 Green PWM0_REG_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }
    return result;
}
// 寄存器PWMx控制RGB灯的蓝色亮度
uint32_t AW2013_Control_Blue(uint8_t PWM_Data)
{
    uint32_t result = 0;
    result = aw2013_WiteByte(PWM2_REG_ADDR, PWM_Data); // B
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 Blue PWM2_REG_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }
    return result;
}
// 寄存器PWMx控制RGB灯的值
uint32_t AW2013_Control_RGB(uint8_t R_Value, uint8_t G_Value, uint8_t B_Value)
{
    uint32_t result = 0;

    result = aw2013_WiteByte(PWM1_REG_ADDR, R_Value); // R
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 Red PWM1_REG_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }

    result = aw2013_WiteByte(PWM0_REG_ADDR, G_Value); // G
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 Green PWM0_REG_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }

    result = aw2013_WiteByte(PWM2_REG_ADDR, B_Value); // B
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 Blue PWM2_REG_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }
    return result;
}
uint32_t AW2013_Init(void)
{
    uint32_t result;

    // gpio_9 复用为 I2C_SCL
    hi_io_set_pull(HI_IO_NAME_GPIO_9, HI_IO_PULL_UP);
    hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_I2C0_SCL);
    // gpio_10 复用为 I2C_SDA
    hi_io_set_pull(HI_IO_NAME_GPIO_10, HI_IO_PULL_UP);
    hi_io_set_func(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_I2C0_SDA);

    result = hi_i2c_init(AW2013_I2C_IDX, AW2013_I2C_SPEED);
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 Init status is 0x%x!!!\r\n", result);
        return result;
    }

    // 复位芯片
    result = aw2013_WiteByte(RSTR_REG_ADDR, 0x55);
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 RSTR_REG_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }

    usleep(TIME_RESET);

    // 使能全局控制器 设置为RUN模式
    result = aw2013_WiteByte(GCR_REG_ADDR, 0x01);
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 GCR_REG_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }

    // 设置打开RGB三路通道
    result = aw2013_WiteByte(LCTR_REG_ADDR, 0x07); // 4: B, 2: G, 1: R
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 LCTR_REG_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }

    // 设置RGB三路通道的工作模式
    result = aw2013_WiteByte(LCFG0_REG_ADDR, 0x63);
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 LCFG0_REG_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }
    result = aw2013_WiteByte(LCFG1_REG_ADDR, 0x63);
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 LCFG1_REG_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }
    result = aw2013_WiteByte(LCFG2_REG_ADDR, 0x63);
    if (result != HI_ERR_SUCCESS) {
        printf("I2C aw2013 LCFG2_REG_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }
    printf("I2C aw2013 Init is succeeded!!!\r\n");
    return HI_ERR_SUCCESS;
}

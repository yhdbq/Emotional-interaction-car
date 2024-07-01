/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GYRO_H
#define GYRO_H

#include <stdint.h>

#define LSM6DS_WRITE_ADDR     (0XD4)
#define LSM6DS_READ_ADDR    (0XD5)
#define WRITELEN  2

/* sensor output data */
#define LSM6DSL_OUTX_L_G    0X22
#define LSM6DSL_OUTX_H_G    0X23
#define LSM6DSL_OUTY_L_G    0X24
#define LSM6DSL_OUTY_H_G    0X25
#define LSM6DSL_OUTZ_L_G    0X26
#define LSM6DSL_OUTZ_H_G    0X27

#define LSM6DSL_OUTX_L_XL   0X28
#define LSM6DSL_OUTX_H_XL   0X29
#define LSM6DSL_OUTY_L_XL   0X2A
#define LSM6DSL_OUTY_H_XL   0X2B
#define LSM6DSL_OUTZ_L_XL   0X2C
#define LSM6DSL_OUTZ_H_XL   0X2D
/* sensor control reg */
#define LSM6DSL_CTRL1_XL    0X10
#define LSM6DSL_CTRL2_G    0X11
#define LSM6DSL_CTRL3_C    0X12
#define LSM6DSL_CTRL8_XL    0X17
#define LSM6DSL_CTRL9_XL    0X18
#define LSM6DSL_CTRL10_C    0X19
#define LSM6DSL_INT2_CTRL   0X0E
#define LSM6DSL_WHO_AM_I    0x0F  // get id
#define LSM6DSL_STATUS_REG  0x1E

#define LSM6DSL_SENSORHUB11_REG    0X38
#define LSM6DSL_TAP_CFG     0X58
#define LSM6DSL_TAP_THS_6D    0X59
#define LSM6DSL_INT_DUR2    0X5A
#define LSM6DSL_WAKE_UP_THS    0X5B
#define LSM6DSL_WAKE_UP_DUR    0X5C
#define LSM6DSL_FREE_FALL   0X5D
#define LSM6DSL_MD1_CFG    0X5E

typedef enum {
    MODE_ON_OFF = 0,
    MODE_SET_LEFT_FORWARD,
    MODE_SET_RIGHT_FORWARD,
    MODE_SET_TURN_LEFT,
    MODE_SET_YAW1,
    MODE_SET_YAW2,
    MODE_SET_YAW3,
    MODE_SET_YAW4,
    MODE_SET_DRIVE_TIME,
    MODE_END,
} ENUM_MODE;

typedef struct {
    uint32_t LeftForward;
    uint32_t RightForward;
    uint32_t TurnRight;
    float yaw1;
    float yaw2;
    float yaw3;
    float yaw4;
    uint32_t time;
} CAR_DRIVE;

float GetYaw(void);
void Lsm_Get_RawAcc(void);
void LSM6DS_Init(void);
void init_ctrl_algo(void);
void init_oled_mode(void);
void ButtonDesplay(ENUM_MODE mode);
void ButtonSet(ENUM_MODE mode, bool button_pressed);

#endif  // AHT20_H
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

#ifndef HAL_BSP_AP3216C_H
#define HAL_BSP_AP3216C_H

#include "cmsis_os2.h"

#define AP3216C_I2C_ADDR (0x3C)  // 器件的I2C从机地址
#define AP3216C_I2C_IDX 0        // 模块的I2C总线号
#define AP3216C_I2C_SPEED 100000 // 100KHz

/**
 * @brief AP3216C 读 接近传感器、光照强度传感器、人体红外传感器的数值
 * @param irData 人体红外传感器
 * @param alsData 光强传感器
 * @param psData 接近传感器
 * @return Returns {@link IOT_SUCCESS} 成功;
 *         Returns {@link IOT_FAILURE} 失败.
 */
uint32_t AP3216C_ReadData(uint16_t *irData, uint16_t *alsData, uint16_t *psData);

/**
 * @brief AP3216C 初始化
 * @return Returns {@link IOT_SUCCESS} 成功;
 *         Returns {@link IOT_FAILURE} 失败.
 */
uint32_t AP3216C_Init(void);

#endif // !__HAL_BSP_AP3216C_H__

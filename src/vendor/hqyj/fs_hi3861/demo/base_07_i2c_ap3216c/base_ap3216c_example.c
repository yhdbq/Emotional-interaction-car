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

#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hal_bsp_ap3216c.h"
#include "hal_bsp_pcf8574.h"

osThreadId_t Task1_ID; // 任务1设置为低优先级任务
#define TASK_STACK_SIZE 1024

void Task1(void)
{
    uint16_t ir = 0, als = 0, ps = 0; // 人体红外传感器 接近传感器 光照强度传感器

    while (1) {
        AP3216C_ReadData(&ir, &als, &ps);
        printf("ir = %d    als = %d    ps = %d\r\n", ir, als, ps);
        sleep(1); // 1s
    }
}
static void base_ap3216c_demo(void)
{
    printf("Enter base_ap3216c_demo()!\r\n");

    PCF8574_Init();
    AP3216C_Init(); // 三合一传感器初始化

    osThreadAttr_t options;
    options.name = "thread_1";
    options.attr_bits = 0;
    options.cb_mem = NULL;
    options.cb_size = 0;
    options.stack_mem = NULL;
    options.stack_size = TASK_STACK_SIZE;
    options.priority = osPriorityNormal;

    Task1_ID = osThreadNew((osThreadFunc_t)Task1, NULL, &options);
    if (Task1_ID != NULL) {
        printf("ID = %d, Create Task1_ID is OK!\r\n", Task1_ID);
    }
}
SYS_RUN(base_ap3216c_demo);
#include "hal_bsp_ap3216c.h"
#include "hal_bsp_log.h"
#include "hi_errno.h"
#include "hi_i2c.h"
#include "hi_io.h"
#include "hi_gpio.h"



#define AP3216C_SYSTEM_ADDR 0x00
#define AP3216C_IR_L_ADDR 0x0A
#define AP3216C_IR_H_ADDR 0x0B
#define AP3216C_ALS_L_ADDR 0x0C
#define AP3216C_ALS_H_ADDR 0x0D
#define AP3216C_PS_L_ADDR 0x0E
#define AP3216C_PS_H_ADDR 0x0F

// 向从机设备 发送数据
static uint32_t AP3216C_WiteByteData(uint8_t byte)
{
  hi_i2c_data i2cData = { 0};
  i2cData.send_buf = &byte;
  i2cData.send_len = 1;

  return hi_i2c_write(AP3216C_I2C_IDX, AP3216C_I2C_ADDR, &i2cData);
}
// 读从机设备数据
static uint32_t AP3216C_RecvData(uint8_t *data, size_t size)
{
  hi_i2c_data i2cData = {0};
  i2cData.receive_buf = data;
  i2cData.receive_len = size;

  return hi_i2c_read(AP3216C_I2C_IDX, AP3216C_I2C_ADDR, &i2cData);
}

// 向寄存器中写数据
static uint32_t AP3216C_WiteCmdByteData(uint8_t regAddr, uint8_t byte)
{
  uint8_t buffer[] = {regAddr, byte};
  hi_i2c_data i2cData = { 0};
  i2cData.send_buf = buffer;
  i2cData.send_len = 2;
  return hi_i2c_write(AP3216C_I2C_IDX, AP3216C_I2C_ADDR, &i2cData);
}

// 读寄存器中的数据   参数: regAddr 目标寄存器地址, byte: 取到的数据
static uint32_t AP3216C_ReadRegByteData(uint8_t regAddr, uint8_t *byte)
{
  uint32_t result = 0;
  uint8_t buffer[2] = {0};

  // 写命令
  result = AP3216C_WiteByteData(regAddr);
  if (result != HI_ERR_SUCCESS)
  {
    console_log_error("I2C AP3216C status = 0x%x!!!", result);
    return result;
  }

  // 读数据
  result = AP3216C_RecvData(buffer, 1);
  if (result != HI_ERR_SUCCESS)
  {
    console_log_error("I2C AP3216C status = 0x%x!!!", result);
    return result;
  }
  *byte = buffer[0];

  return HI_ERR_SUCCESS;
}

uint32_t AP3216C_ReadData(uint16_t *irData, uint16_t *alsData, uint16_t *psData)
{
  uint32_t result = 0;
  uint8_t data_H = 0, data_L = 0;

  // 读取IR传感器数据    10-bit
  result = AP3216C_ReadRegByteData(AP3216C_IR_L_ADDR, &data_L);
  if (result != HI_ERR_SUCCESS)
    return result;

  result = AP3216C_ReadRegByteData(AP3216C_IR_H_ADDR, &data_H);
  if (result != HI_ERR_SUCCESS)
    return result;

  if (data_L & 0x80) // IR_OF为1, 则数据无效
    *irData = 0;
  else
    *irData = ((uint16_t)data_H << 2) | (data_L & 0x03);

  // 读取ALS传感器数据    16-bit
  result = AP3216C_ReadRegByteData(AP3216C_ALS_L_ADDR, &data_L);
  if (result != HI_ERR_SUCCESS)
    return result;

  result = AP3216C_ReadRegByteData(AP3216C_ALS_H_ADDR, &data_H);
  if (result != HI_ERR_SUCCESS)
    return result;

  *alsData = ((uint16_t)data_H << 8) | (data_L);

  // 读取PS传感器数据    10-bit
  result = AP3216C_ReadRegByteData(AP3216C_PS_L_ADDR, &data_L);
  if (result != HI_ERR_SUCCESS)
    return result;

  result = AP3216C_ReadRegByteData(AP3216C_PS_H_ADDR, &data_H);
  if (result != HI_ERR_SUCCESS)
    return result;

  if (data_L & 0x40) // IR_OF为1, 则数据无效
    *psData = 0;
  else
    *psData = ((uint16_t)(data_H & 0x3F) << 4) | (data_L & 0x0F);

  return HI_ERR_SUCCESS;
}

uint32_t AP3216C_Init(void)
{
  uint32_t result;

  // gpio_9 复用为 I2C_SCL
  hi_io_set_pull(HI_IO_NAME_GPIO_9, HI_IO_PULL_UP);
  hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_I2C0_SCL);
  // gpio_10 复用为 I2C_SDA
  hi_io_set_pull(HI_IO_NAME_GPIO_10, HI_IO_PULL_UP);
  hi_io_set_func(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_I2C0_SDA);

  result = hi_i2c_init(AP3216C_I2C_IDX, AP3216C_I2C_SPEED);
  if (result != HI_ERR_SUCCESS)
  {
    console_log_error("I2C ap3216c Init status is 0x%x!!!", result);
    return result;
  }

  // 复位芯片
  result = AP3216C_WiteCmdByteData(AP3216C_SYSTEM_ADDR, 0x04);
  if (result != HI_ERR_SUCCESS)
  {
    console_log_error("I2C AP3216C AP3216C_SYSTEM_ADDR status = 0x%x!!!", result);
    return result;
  }

  usleep(50 * 1000);

  // 开启ALS\PS\IR
  result = AP3216C_WiteCmdByteData(AP3216C_SYSTEM_ADDR, 0x03);
  if (result != HI_ERR_SUCCESS)
  {
    console_log_error("I2C AP3216C AP3216C_SYSTEM_ADDR status = 0x%x!!!", result);
    return result;
  }
  console_log_success("I2C AP3216C Init is succeeded!!!");

  return HI_ERR_SUCCESS;
}

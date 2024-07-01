#include "hal_bsp_ssd1306.h"
#include "hal_bsp_ssd1306_fonts.h"
#include "hal_bsp_log.h"
#include "hi_errno.h"
#include "hi_i2c.h"
#include "hi_io.h"
#include "hi_gpio.h"

static uint32_t SSD1306_SendData(uint8_t *data, size_t size)
{
  hi_i2c_data i2cData = {0};
  i2cData.send_buf = data;
  i2cData.send_len = size;

  return hi_i2c_write(SSD1306_I2C_IDX, SSD1306_I2C_ADDR, &i2cData);
}

// 写命令
static uint32_t SSD1306_WriteCmd(uint8_t byte)
{
  uint8_t buffer[] = {0x00, byte};
  return SSD1306_SendData(buffer, sizeof(buffer));
}
// 写数据
static uint32_t SSD1306_WiteData(uint8_t byte)
{
  uint8_t buffer[] = {0x40, byte};
  return SSD1306_SendData(buffer, sizeof(buffer));
}

uint32_t SSD1306_Init(void)
{
  uint32_t result;

  // gpio_9 复用为 I2C_SCL
  hi_io_set_pull(HI_IO_NAME_GPIO_9, HI_IO_PULL_UP);
  hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_I2C0_SCL);
  // gpio_10 复用为 I2C_SDA
  hi_io_set_pull(HI_IO_NAME_GPIO_10, HI_IO_PULL_UP);
  hi_io_set_func(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_I2C0_SDA);

  result = hi_i2c_init(SSD1306_I2C_IDX, SSD1306_I2C_SPEED);
  if (result != HI_ERR_SUCCESS)
  {
    console_log_error("I2C SSD1306 Init status is 0x%x!!!\n", result);
    return result;
  }

  usleep(100 * 1000);

  SSD1306_WriteCmd(0xAE); // display off
  SSD1306_WriteCmd(0x20); // Set Memory Addressing Mode
  SSD1306_WriteCmd(0x10); // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
  SSD1306_WriteCmd(0xb0); // Set Page Start Address for Page Addressing Mode,0-7
  SSD1306_WriteCmd(0xc8); // Set COM Output Scan Direction
  SSD1306_WriteCmd(0x00); //---set low column address
  SSD1306_WriteCmd(0x10); //---set high column address
  SSD1306_WriteCmd(0x40); //--set start line address
  SSD1306_WriteCmd(0x81); //--set contrast control register
  SSD1306_WriteCmd(0xff); //亮度调节 0x00~0xff
  SSD1306_WriteCmd(0xa1); //--set segment re-map 0 to 127
  SSD1306_WriteCmd(0xa6); //--set normal display
  SSD1306_WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
  SSD1306_WriteCmd(0x3F); //
  SSD1306_WriteCmd(0xa4); // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
  SSD1306_WriteCmd(0xd3); //-set display offset
  SSD1306_WriteCmd(0x00); //-not offset
  SSD1306_WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
  SSD1306_WriteCmd(0xf0); //--set divide ratio
  SSD1306_WriteCmd(0xd9); //--set pre-charge period
  SSD1306_WriteCmd(0x22); //
  SSD1306_WriteCmd(0xda); //--set com pins hardware configuration
  SSD1306_WriteCmd(0x12);
  SSD1306_WriteCmd(0xdb); //--set vcomh
  SSD1306_WriteCmd(0x20); // 0x20,0.77xVcc
  SSD1306_WriteCmd(0x8d); //--set DC-DC enable
  SSD1306_WriteCmd(0x14); //
  SSD1306_WriteCmd(0xaf); //--turn on oled panel
  SSD1306_WriteCmd(0xAF); /*display ON*/
  SSD1306_SetPos(0, 0);

  console_log_success("I2C SSD1306 Init is succeeded!!!");

  return HI_ERR_SUCCESS;
}
/**
 * @brief  垂直滚动函数
 * @note
 * @retval None
 */
void OLED_Set_Vertical_Rol(void)
{
  SSD1306_WriteCmd(0x2e); // 停止滚动

  SSD1306_WriteCmd(0xa3); //
  SSD1306_WriteCmd(0x00);
  SSD1306_WriteCmd(0x40);

  SSD1306_WriteCmd(0x2a);
  SSD1306_WriteCmd(0x00);
  SSD1306_WriteCmd(0x00);
  SSD1306_WriteCmd(0x01);
  SSD1306_WriteCmd(0x00);
  SSD1306_WriteCmd(0x04);

  SSD1306_WriteCmd(0x2f);
}

void SSD1306_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{
  SSD1306_WriteCmd(0xb0 + y);
  SSD1306_WriteCmd(((x & 0xf0) >> 4) | 0x10);
  SSD1306_WriteCmd((x & 0x0f) | 0x01);
}

void SSD1306_Fill(unsigned char fill_Data) //全屏填充
{
  unsigned char m, n;
  for (m = 0; m < 8; m++)
  {
    SSD1306_WriteCmd(0xb0 + m); // page0-page1
    SSD1306_WriteCmd(0x00);     // low column start address
    SSD1306_WriteCmd(0x10);     // high column start address
    for (n = 0; n < 128; n++)
    {
      SSD1306_WiteData(fill_Data);
    }
  }
}

void SSD1306_CLS(void) //清屏
{
  SSD1306_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          :
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void SSD1306_ON(void)
{
  SSD1306_WriteCmd(0X8D); //设置电荷泵
  SSD1306_WriteCmd(0X14); //开启电荷泵
  SSD1306_WriteCmd(0XAF); // OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          :
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void SSD1306_OFF(void)
{
  SSD1306_WriteCmd(0X8D); //设置电荷泵
  SSD1306_WriteCmd(0X10); //关闭电荷泵
  SSD1306_WriteCmd(0XAE); // OLED休眠
}

/**
 * @brief  显示字符串
 * @note
 * @param  x:
 * @param  y:
 * @param  ch[]:
 * @param  TextSize:
 * @retval None
 */
void SSD1306_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
  unsigned char c = 0, i = 0, j = 0;
  switch (TextSize)
  {
  case 8:
  {
    while (ch[j] != '\0')
    {
      c = ch[j] - 32;
      if (x > 126)
      {
        x = 0;
        y++;
      }
      SSD1306_SetPos(x, y);
      for (i = 0; i < 6; i++)
        SSD1306_WiteData(F6x8[c][i]);
      x += 6;
      j++;
    }
  }
  break;
  case 16:
  {
    y *= 2;
    while (ch[j] != '\0')
    {
      c = ch[j] - 32;
      if (x > 120)
      {
        x = 0;
        y++;
      }
      SSD1306_SetPos(x, y);
      for (i = 0; i < 8; i++)
        SSD1306_WiteData(F8X16[c * 16 + i]);
      SSD1306_SetPos(x, y + 1);
      for (i = 0; i < 8; i++)
        SSD1306_WiteData(F8X16[c * 16 + i + 8]);
      x += 8;
      j++;
    }
  }
  break;
  }
}

/**
 * @brief  显示图片
 * @note
 * @param  x0:
 * @param  y0:
 * @param  x1:
 * @param  y1:
 * @param  BMP[]:
 * @retval None
 */
#if 0
void SSD1306_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		SSD1306_SetPos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	SSD1306_WiteData(BMP[j++]);	    	
	    }
	}
}
#endif

/**
 * @brief  显示一张图片
 * @note   取模方式： 阳码、列行式、逆向、十六进制数、输出索引格、C51格式
 * @param  xMove: x坐标
 * @param  yMove: y坐标
 * @param  width: 宽
 * @param  height: 高
 * @param  *BMP: 
 * @retval None
 */
void SSD1306_DrawBMP(unsigned char xMove, unsigned char yMove, unsigned char width, unsigned char height, unsigned char *BMP)
{
  unsigned short j = 0, x=0, y=0;

  unsigned char x0 = xMove;
  unsigned char y0 = yMove / 8;
  unsigned char x1 = xMove + width;
  unsigned char y1 = height / 8 + y0;

  for (y = y0; y < y1; y++)
  {
    SSD1306_SetPos(x0, y);
    for (x = x0; x < x1; x++)
    {
      SSD1306_WiteData(BMP[j]);
      j++;
    }
  }
}
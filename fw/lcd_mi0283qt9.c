#include <stdint.h>
#include "cmsis/LPC11xx.h"
#include "settings.h"
#include "main.h"
#include "lcd.h"
#include "lcd_mi0283qt9.h"

#ifdef LCD_MI0283QT9


#define LCD_PORT        2
#define DATA_PINS       0xFF
#define DATA_WR(x)      GPIO_SETPORT(LCD_PORT, DATA_PINS, x)
#define RST_PIN         8
#define CS_PIN          11
#define RS_PIN          10
#define WR_PIN          9
#define RD_PIN          5 //GPIO3[5]

#define INIT_PINS()     { GPIO_PORT(LCD_PORT)->DIR  |= (1<<RST_PIN) | (1<<CS_PIN) | (1<<RS_PIN) | (1<<WR_PIN) | DATA_PINS; \
                          GPIO_PORT(LCD_PORT)->DATA |= (1<<RST_PIN) | (1<<CS_PIN) | (1<<RS_PIN) | (1<<WR_PIN) | DATA_PINS; \
                          GPIO_PORT(3)->DIR  |= (1<<RD_PIN); \
                          GPIO_PORT(3)->DATA |= (1<<RD_PIN); }

/*
__attribute__((always_inline)) __INLINE void lcd_drawstop(void)
{
  //lcd_disable();

  return;
}
*/

__attribute__((always_inline)) __INLINE void lcd_draw(uint_least16_t color)
{
  return lcd_wrdata16(color);
}


__attribute__((always_inline)) __INLINE void lcd_drawstart(void)
{
  //lcd_enable();

  lcd_wrcmd8(LCD_CMD_WRITE);

  return;
}


__attribute__((always_inline)) __INLINE void lcd_setarea(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1)
{
  //lcd_enable();

  lcd_wrcmd8(LCD_CMD_COLUMN);
  lcd_wrdata16(y0);
  lcd_wrdata16(y1);

  lcd_wrcmd8(LCD_CMD_PAGE);
  lcd_wrdata16(x0);
  lcd_wrdata16(x1);

  //lcd_disable();

  return;
}


uint_least32_t lcd_setbias(uint_least16_t o)
{
  uint_least32_t w, h;
  uint_least8_t param;

  #define MEM_Y   (7) //MY row address order
  #define MEM_X   (6) //MX column address order 
  #define MEM_V   (5) //MV row / column exchange 
  #define MEM_L   (4) //ML vertical refresh order
  #define MEM_H   (2) //MH horizontal refresh order
  #define MEM_BGR (3) //RGB-BGR Order 

  switch(o)
  {
    default:
    case 0:
    //case 36:
    //case 360:
      w     = LCD_WIDTH;
      h     = LCD_HEIGHT;
      param = (1<<MEM_BGR) | (1<<MEM_X) | (1<<MEM_Y);
      break;
    case 9:
    case 90:
      w     = LCD_HEIGHT;
      h     = LCD_WIDTH;
      param = (1<<MEM_BGR) | (1<<MEM_X) | (1<<MEM_V);
      break;
    case 18:
    case 180:
      w     = LCD_WIDTH;
      h     = LCD_HEIGHT;
      param = (1<<MEM_BGR) | (1<<MEM_L);
      break;
    case 27:
    case 270:
    case 14: //270&0xFF
      w     = LCD_HEIGHT;
      h     = LCD_WIDTH;
      param = (1<<MEM_BGR) | (1<<MEM_Y) | (1<<MEM_V);
      break;
  }

  lcd_enable();

  lcd_wrcmd8(LCD_CMD_MEMACCESS_CTRL);
  lcd_wrdata8(param);

  lcd_setarea(0, 0, w-1, h-1);

  lcd_disable();

  return (w<<16) | (h<<0);
}


void lcd_invert(uint_least8_t on)
{
  lcd_enable();

  if(on == 0)
  {
     lcd_wrcmd8(LCD_CMD_INV_OFF);
  }
  else
  {
     lcd_wrcmd8(LCD_CMD_INV_ON);
  }

  lcd_disable();

  return;
}


void lcd_power(uint_least8_t on)
{
  lcd_enable();

  if(on == 0)
  {
    lcd_wrcmd8(LCD_CMD_DISPLAY_OFF);
    delay_ms(20);
    lcd_wrcmd8(LCD_CMD_SLEEPIN);
    delay_ms(120);
  }
  else
  {
    lcd_wrcmd8(LCD_CMD_SLEEPOUT);
    delay_ms(120);
    lcd_wrcmd8(LCD_CMD_DISPLAY_ON);
    delay_ms(20);
  }

  lcd_disable();

  return;
}


void lcd_reset(void)
{
  uint_least8_t c;
  uint_least16_t i, j;
  uint_least32_t s;

  uint8_t initdata[] = 
  {
    //0x40| 1, LCD_CMD_RESET,
    //0xC0|60,
    //0xC0|60,
    0x40| 1, LCD_CMD_DISPLAY_OFF,
    0xC0|20,
    0x40| 1, LCD_CMD_POWER_CTRLB,
    0x80| 3, 0x00, 0x83, 0x30, //0x83 0x81 0xAA
    0x40| 1, LCD_CMD_POWERON_SEQ_CTRL,
    0x80| 4, 0x64, 0x03, 0x12, 0x81, //0x64 0x67
    0x40| 1, LCD_CMD_DRV_TIMING_CTRLA,
    0x80| 3, 0x85, 0x01, 0x79, //0x79 0x78
    0x40| 1, LCD_CMD_POWER_CTRLA,
    0x80| 5, 0x39, 0X2C, 0x00, 0x34, 0x02,
    0x40| 1, LCD_CMD_PUMP_RATIO_CTRL,
    0x80| 1, 0x20,
    0x40| 1, LCD_CMD_DRV_TIMING_CTRLB,
    0x80| 2, 0x00, 0x00,
    0x40| 1, LCD_CMD_POWER_CTRL1,
    0x80| 1, 0x26, //0x26 0x25
    0x40| 1, LCD_CMD_POWER_CTRL2,
    0x80| 1, 0x11,
    0x40| 1, LCD_CMD_VCOM_CTRL1,
    0x80| 2, 0x35, 0x3E,
    0x40| 1, LCD_CMD_VCOM_CTRL2,
    0x80| 1, 0xBE, //0xBE 0x94
    0x40| 1, LCD_CMD_FRAME_CTRL,
    0x80| 2, 0x00, 0x1B, //0x1B 0x70
    0x40| 1, LCD_CMD_ENABLE_3G,
    0x80| 1, 0x08, //0x08 0x00
    0x40| 1, LCD_CMD_GAMMA,
    0x80| 1, 0x01, //G2.2
    0x40| 1, LCD_CMD_POS_GAMMA,
    0x80|15, 0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0x87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00,
  //0x80|15, 0x0F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0x87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00,
    0x40| 1, LCD_CMD_NEG_GAMMA,
    0x80|15, 0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F,
  //0x80|15, 0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x0F,
    0x40| 1, LCD_CMD_DISPLAY_CTRL,
    0x80| 4, 0x0A, 0x82, 0x27, 0x00,
    0x40| 1, LCD_CMD_ENTRY_MODE,
    0x80| 1, 0x07,
    0x40| 1, LCD_CMD_PIXEL_FORMAT,
    0x80| 1, 0x55, //16bit
    0x40| 1, LCD_CMD_MEMACCESS_CTRL,
    0x80| 1, (1<<MEM_BGR) | (1<<MEM_X) | (1<<MEM_Y),
    0x40| 1, LCD_CMD_COLUMN,
    0x80| 2, 0x00, 0x00,
    0x80| 2, ((LCD_HEIGHT-1)>>8)&0xFF, (LCD_HEIGHT-1)&0xFF,
    0x40| 1, LCD_CMD_PAGE,
    0x80| 2, 0x00, 0x00,
    0x80| 2, ((LCD_WIDTH-1)>>8)&0xFF, (LCD_WIDTH-1)&0xFF,
    0x40| 1, LCD_CMD_SLEEPOUT,
    0xC0|60,
    0xC0|60,
    0x40| 1, LCD_CMD_DISPLAY_ON,
    0xC0|20,
  };

  //init pins
  INIT_PINS();

  //hardware reset
  GPIO_CLRPIN(LCD_PORT, RST_PIN);
  delay_ms(20);
  GPIO_SETPIN(LCD_PORT, RST_PIN);
  delay_ms(120);

  lcd_enable();

  //send init commands and data
  for(i=0; i<sizeof(initdata);)
  {
    c = initdata[i++];
    switch(c&0xC0)
    {
      case 0x40: //command
        for(j=c&0x3F; j!=0; j--)
        {
          c = initdata[i++];
          lcd_wrcmd8(c);
        }
        break;
      case 0x80: //data
        for(j=c&0x3F; j!=0; j--)
        {
          c = initdata[i++];
          lcd_wrdata8(c);
        }
        break;
      case 0xC0: //delay
        delay_ms(c&0x3F);
        break;
    }
  }

  //clear display buffer
  lcd_drawstart();
  for(s=(LCD_WIDTH*LCD_HEIGHT); s!=0; s--)
  {
    lcd_draw(0);
  }
  lcd_drawstop();

  lcd_disable();

  return;
}


__attribute__((always_inline)) __INLINE void lcd_wrdata16(uint_least16_t data)
{
  //GPIO_SETPIN(LCD_PORT, RS_PIN); //data

  GPIO_CLRPIN(LCD_PORT, WR_PIN);
  DATA_WR(data>>8);
  GPIO_SETPIN(LCD_PORT, WR_PIN);

  //NOP();

  GPIO_CLRPIN(LCD_PORT, WR_PIN);
  DATA_WR(data);
  GPIO_SETPIN(LCD_PORT, WR_PIN);

  return;
}


__attribute__((always_inline)) __INLINE void lcd_wrdata8(uint_least8_t data)
{
  //GPIO_SETPIN(LCD_PORT, RS_PIN); //data

  GPIO_CLRPIN(LCD_PORT, WR_PIN);
  DATA_WR(data);
  GPIO_SETPIN(LCD_PORT, WR_PIN);

  return;
}


__attribute__((always_inline)) __INLINE void lcd_wrcmd8(uint_least8_t cmd)
{
  GPIO_CLRPIN(LCD_PORT, RS_PIN); //cmd

  GPIO_CLRPIN(LCD_PORT, WR_PIN);
  DATA_WR(cmd);
  GPIO_SETPIN(LCD_PORT, WR_PIN);

  GPIO_SETPIN(LCD_PORT, RS_PIN); //data

  return;
}


__attribute__((always_inline)) __INLINE void lcd_disable(void)
{
  GPIO_SETPIN(LCD_PORT, CS_PIN);

  return;
}


__attribute__((always_inline)) __INLINE void lcd_enable(void)
{
  GPIO_CLRPIN(LCD_PORT, CS_PIN);

  return;
}


#endif //LCD_MI0283QT9

#include <stdint.h>
#include "cmsis/LPC11xx.h"
#include "settings.h"
#include "main.h"
#include "lcd.h"
#include "lcd_ug12d228aa.h"

#ifdef LCD_UG12D228AA


#define LCD_PORT        2
#define DATA_PINS       0xFF
#define DATA_WR(x)      GPIO_SETPORT(LCD_PORT, DATA_PINS, x)
#define RST_PIN         8
#define CS_PIN          11
#define RS_PIN          10
#define WR_PIN          9

#define INIT_PINS()     { GPIO_PORT(LCD_PORT)->DIR  |= (1<<RST_PIN) | (1<<CS_PIN) | (1<<RS_PIN) | (1<<WR_PIN) | DATA_PINS; \
                          GPIO_PORT(LCD_PORT)->DATA |= (1<<RST_PIN) | (1<<CS_PIN) | (1<<RS_PIN) | (1<<WR_PIN) | DATA_PINS; }

uint_least16_t lcd_mode=0;


/*__attribute__((always_inline)) __INLINE void lcd_drawstop(void)
{
  //lcd_disable();

  return;
}*/


__attribute__((always_inline)) __INLINE void lcd_draw(uint_least16_t color)
{
  return lcd_wrdata16(color);
}


/*__attribute__((always_inline)) __INLINE void lcd_drawstart(void)
{
  //lcd_enable();

  return;
}*/

__attribute__((always_inline)) __INLINE void lcd_setarea(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1)
{
  //lcd_enable();

  switch(lcd_mode)
  {
    default:
    case 0:
      lcd_wrcmd8(LCD_CMD_X_ADDR_AREA);
      lcd_wrcmd8(0x02+LCD_WIDTH-1-x1);
      lcd_wrcmd8(0x02+LCD_WIDTH-1-x0);
      lcd_wrcmd8(LCD_CMD_Y_ADDR_AREA);
      lcd_wrcmd8(0x02+y0);
      lcd_wrcmd8(0x02+y1);
      break;
    case 90:
      lcd_wrcmd8(LCD_CMD_X_ADDR_AREA);
      lcd_wrcmd8(0x02+y0);
      lcd_wrcmd8(0x02+y1);
      lcd_wrcmd8(LCD_CMD_Y_ADDR_AREA);
      lcd_wrcmd8(0x02+x0);
      lcd_wrcmd8(0x02+x1);
      break;
    case 180:
      lcd_wrcmd8(LCD_CMD_X_ADDR_AREA);
      lcd_wrcmd8(0x02+x0);
      lcd_wrcmd8(0x02+x1);
      lcd_wrcmd8(LCD_CMD_Y_ADDR_AREA);
      lcd_wrcmd8(0x02+LCD_HEIGHT-1-y1);
      lcd_wrcmd8(0x02+LCD_HEIGHT-1-y0);
      break;
    case 270:
      lcd_wrcmd8(LCD_CMD_X_ADDR_AREA);
      lcd_wrcmd8(0x02+LCD_HEIGHT-1-y1);
      lcd_wrcmd8(0x02+LCD_HEIGHT-1-y0);
      lcd_wrcmd8(LCD_CMD_Y_ADDR_AREA);
      lcd_wrcmd8(0x02+LCD_WIDTH-1-x1);
      lcd_wrcmd8(0x02+LCD_WIDTH-1-x0);
      break;
  }

  //lcd_disable();

  return;
}


uint_least32_t lcd_setbias(uint_least16_t o)
{
  uint_least32_t w, h;

  switch(o)
  {
    default:
    case 0:
    //case 36:
    //case 360:
      o     = 0;
      w     = LCD_WIDTH;
      h     = LCD_HEIGHT;
      break;
    case 90:
      o     = 90;
      w     = LCD_HEIGHT;
      h     = LCD_WIDTH;
      break;
    case 180:
      o     = 180;
      w     = LCD_WIDTH;
      h     = LCD_HEIGHT;
      break;
    case 270:
      o     = 270;
      w     = LCD_HEIGHT;
      h     = LCD_WIDTH;
      break;
  }

  lcd_mode = o;

  lcd_enable();

  lcd_setarea(0, 0, w-1, h-1);

  lcd_disable();

  return (w<<16) | (h<<0);
}


void lcd_invert(uint_least8_t on)
{
  lcd_enable();
/*
  if(on == 0)
  {
     lcd_wrcmd8(LCD_CMD_INV_OFF);
  }
  else
  {
     lcd_wrcmd8(LCD_CMD_INV_ON);
  }
*/
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
    lcd_wrcmd8(LCD_CMD_STANDBY_ON);
    delay_ms(120);
  }
  else
  {
    lcd_wrcmd8(LCD_CMD_STANDBY_OFF);
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
    0x40| 1, LCD_CMD_DISPLAY_OFF,
    0xC0|20,
    0x40| 1, LCD_CMD_STANDBY_OFF,
    0xC0|20,
    0x40| 2, LCD_CMD_OSC, 0x01,
    0xC0|20,
    0x40| 2, LCD_CMD_DCDC_AMP, 0x01,
    0xC0|20,
    0x40| 2, LCD_CMD_DCDC_AMP, 0x09,
    0xC0|20,
    0x40| 2, LCD_CMD_DCDC_AMP, 0x0B,
    0xC0|20,
    0x40| 2, LCD_CMD_DCDC_AMP, 0x0F,
    0xC0|20,
    0x40| 2, LCD_CMD_DRIVER_OUTPUT, 0x01, //0x01 0x04 0x07
    0x40| 2, LCD_CMD_DCDC, 0x02, //0x02 0x03
    0x40| 2, LCD_CMD_DCDC_CLOCKDIV, 0x02,  //0x02 0x03
    0x40| 2, LCD_CMD_TEMP_COMP, 0x01,
    0x40| 2, LCD_CMD_CONTRAST_CTRL1, 0xD0, //0xD0 0x55
    0x40| 2, LCD_CMD_ADDR_MODE, 0x00, //0x00 0x10
    0x40| 2, LCD_CMD_ROW_VECTOR_MODE, 0x0E,
    0x40| 2, LCD_CMD_LINE_INVERSION, 0x0D,
    0x40| 2, LCD_CMD_FRAME_FREQ_CTRL, 0x00,
    0x40| 1, LCD_CMD_RGB565,
    0x40| 2, LCD_CMD_ENTRY_MODE, 0x00,
    0x40| 3, LCD_CMD_X_ADDR_AREA, 0x02+0, 0x02+LCD_WIDTH-1,
    0x40| 3, LCD_CMD_Y_ADDR_AREA, 0x02+0, 0x02+LCD_HEIGHT-1,
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


#endif //LCD_UG12D228AA

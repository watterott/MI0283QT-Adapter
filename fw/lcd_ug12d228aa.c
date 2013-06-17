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

uint32_t lcd_mode=0;


void lcd_drawstop(void)
{
  //lcd_disable();

  return;
}


void lcd_draw(uint32_t color)
{
  return lcd_wrdata16(color);
}


void lcd_drawstart(void)
{
  //lcd_enable();

  //lcd_wrcmd8(LCD_CMD_WRITE);

  return;
}


void lcd_setarea(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1)
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


uint32_t lcd_setbias(uint32_t o)
{
  uint32_t w, h;

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
    case 9:
    case 90:
      o     = 90;
      w     = LCD_HEIGHT;
      h     = LCD_WIDTH;
      break;
    case 18:
    case 180:
      o     = 180;
      w     = LCD_WIDTH;
      h     = LCD_HEIGHT;
      break;
    case 27:
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


void __attribute__((optimize("Os"))) lcd_invert(uint32_t on)
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


void __attribute__((optimize("Os"))) lcd_power(uint32_t on)
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


void __attribute__((optimize("Os"))) lcd_reset(void)
{
  uint32_t i;

  //init pins
  INIT_PINS();

  //hardware reset
  GPIO_CLRPIN(LCD_PORT, RST_PIN);
  delay_ms(20);
  GPIO_SETPIN(LCD_PORT, RST_PIN);
  delay_ms(120);

  lcd_enable();

  lcd_wrcmd8(LCD_CMD_DISPLAY_OFF);
  delay_ms(20);
  lcd_wrcmd8(LCD_CMD_STANDBY_OFF);
  delay_ms(20);

  //send init commands
  lcd_wrcmd8(LCD_CMD_OSC);
  lcd_wrcmd8(0x01);
  delay_ms(20);

  lcd_wrcmd8(LCD_CMD_DCDC_AMP);
  lcd_wrcmd8(0x01);
  delay_ms(20);
  lcd_wrcmd8(LCD_CMD_DCDC_AMP);
  lcd_wrcmd8(0x09);
  delay_ms(20);
  lcd_wrcmd8(LCD_CMD_DCDC_AMP);
  lcd_wrcmd8(0x0B);
  delay_ms(20);
  lcd_wrcmd8(LCD_CMD_DCDC_AMP);
  lcd_wrcmd8(0x0F);
  delay_ms(20);
  lcd_wrcmd8(LCD_CMD_DRIVER_OUTPUT);
  lcd_wrcmd8(0x01); //0x01 0x04 0x07
  lcd_wrcmd8(LCD_CMD_DCDC);
  lcd_wrcmd8(0x02); //0x02 0x03
  lcd_wrcmd8(LCD_CMD_DCDC_CLOCKDIV);
  lcd_wrcmd8(0x02); //0x02 0x03
  lcd_wrcmd8(LCD_CMD_TEMP_COMP);
  lcd_wrcmd8(0x01);
  lcd_wrcmd8(LCD_CMD_CONTRAST_CTRL1);
  lcd_wrcmd8(0xD0); //0xD0 0x55
  lcd_wrcmd8(LCD_CMD_ADDR_MODE);
  lcd_wrcmd8(0x00); //0x10 0x10
  lcd_wrcmd8(LCD_CMD_ROW_VECTOR_MODE);
  lcd_wrcmd8(0x0E);
  lcd_wrcmd8(LCD_CMD_LINE_INVERSION);
  lcd_wrcmd8(0x0D);
  lcd_wrcmd8(LCD_CMD_FRAME_FREQ_CTRL);
  lcd_wrcmd8(0x00);
  lcd_wrcmd8(LCD_CMD_RGB565);
  lcd_wrcmd8(LCD_CMD_ENTRY_MODE);
  lcd_wrcmd8(0x00);

  lcd_disable();

  //set orientation
  lcd_setbias(0);

  lcd_enable();

  //clear display buffer
  lcd_drawstart();
  for(i=(LCD_WIDTH*LCD_HEIGHT); i!=0; i--)
  {
    lcd_draw(0);
  }
  lcd_drawstop();

  //display on
  lcd_wrcmd8(LCD_CMD_DISPLAY_ON);
  delay_ms(20);

  lcd_disable();

  return;
}


void lcd_wrdata16(uint32_t data)
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


void lcd_wrdata8(uint32_t data)
{
  //GPIO_SETPIN(LCD_PORT, RS_PIN); //data

  GPIO_CLRPIN(LCD_PORT, WR_PIN);
  DATA_WR(data);
  GPIO_SETPIN(LCD_PORT, WR_PIN);

  return;
}


void lcd_wrcmd8(uint32_t cmd)
{
  GPIO_CLRPIN(LCD_PORT, RS_PIN); //cmd

  GPIO_CLRPIN(LCD_PORT, WR_PIN);
  DATA_WR(cmd);
  GPIO_SETPIN(LCD_PORT, WR_PIN);

  GPIO_SETPIN(LCD_PORT, RS_PIN); //data

  return;
}


void lcd_disable(void)
{
  GPIO_SETPIN(LCD_PORT, CS_PIN);

  return;
}


void lcd_enable(void)
{
  GPIO_CLRPIN(LCD_PORT, CS_PIN);

  return;
}


#endif //LCD_UG12D228AA

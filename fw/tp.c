#include <stdint.h>
#include "cmsis/LPC11xx.h"
#include "main.h"
#include "settings.h"
#include "lcd.h"
#include "lcd_mi0283qt9.h"
#include "lcd_ug12d228aa.h"
#include "tp.h"

#ifdef TP_SUPPORT


#define SET_DATA(port, pin, d)    { IOCON_SETRPIN(port, pin, IOCON_PIO | IOCON_NOPULL | IOCON_DIGITAL); \
                                    GPIO_PORT(port)->DIR |= (1<<pin); \
                                    GPIO_PORT(port)->MASKED_ACCESS[1<<pin] = d<<pin; }
#define SET_HIZ(port, pin)        { IOCON_SETRPIN(port, pin, IOCON_PIO | IOCON_NOPULL | IOCON_DIGITAL); \
                                    GPIO_PORT(port)->DIR     &= ~(1<<pin); \
                                    GPIO_PORT(port)->DATA    &= ~(1<<pin); }
#define SET_HIZUP(port, pin)      { IOCON_SETRPIN(port, pin, IOCON_PIO | IOCON_PULLUP | IOCON_DIGITAL); \
                                    GPIO_PORT(port)->DIR     &= ~(1<<pin); \
                                    GPIO_PORT(port)->DATA    &= ~(1<<pin); }
#define SET_HIZDOWN(port, pin)    { IOCON_SETRPIN(port, pin, IOCON_PIO | IOCON_PULLDOWN | IOCON_DIGITAL); \
                                    GPIO_PORT(port)->DIR     &= ~(1<<pin); \
                                    GPIO_PORT(port)->DATA    &= ~(1<<pin); }
#define SET_ADC(port, pin)        { IOCON_SETRPIN(port, pin, IOCON_ADC | IOCON_NOPULL | IOCON_ANALOG); \
                                    GPIO_PORT(port)->DIR     &= ~(1<<pin); \
                                    GPIO_PORT(port)->DATA    &= ~(1<<pin); }

#define ADC_READ(chn, x)          { LPC_ADC->CR |= (1<<chn) | (1<<24);    \
                                    while(!(LPC_ADC->DR[chn] & (1<<31))); \
                                    x = ((LPC_ADC->DR[chn]>>6) & 0x3FE);  \
                                    LPC_ADC->CR &= ~((1<<chn) | (1<<24)); } //read ADC channel and remove LSB (0x3FE)

#define XP_DATA(x)      SET_DATA( XP_PORT, XP_PIN, x)
#define XP_HIZUP()      SET_HIZUP(XP_PORT, XP_PIN)
#define XP_ADC()        SET_ADC(  XP_PORT, XP_PIN)

#define XM_DATA(x)      SET_DATA( XM_PORT, XM_PIN, x)
#define XM_HIZUP()      SET_HIZUP(XM_PORT, XM_PIN)
#define XM_ADC()        SET_ADC(  XM_PORT, XM_PIN)

#define YP_DATA(x)      SET_DATA( YP_PORT, YP_PIN, x)
#define YP_HIZUP()      SET_HIZUP(YP_PORT, YP_PIN)
#define YP_ADC()        SET_ADC(  YP_PORT, YP_PIN)

#define YM_DATA(x)      SET_DATA( YM_PORT, YM_PIN, x)
#define YM_HIZUP()      SET_HIZUP(YM_PORT, YM_PIN)
#define YM_ADC()        SET_ADC(  YM_PORT, YM_PIN)


uint32_t raw_x, raw_y, raw_z;
uint32_t last_x, last_y;
uint32_t cal_x, cal_y, cal_z;
CAL_MATRIX matrix;


uint32_t tp_getz(void)
{
  return cal_z; 
}


uint32_t tp_gety(void)
{
  uint32_t o, x, y;

  if(raw_x != last_x)
  {
    last_x = raw_x;
    x = raw_x;
    y = raw_y;
    x = ((matrix.a * x) + (matrix.b * y) + matrix.c) / matrix.div;
    if(x >= LCD_WIDTH) { x = LCD_WIDTH-1; }
    cal_x = x;
  }

  if(raw_y != last_y)
  {
    last_y = raw_y;
    x = raw_x;
    y = raw_y;
    y = ((matrix.d * x) + (matrix.e * y) + matrix.f) / matrix.div;
    if(y >= LCD_HEIGHT) { y = LCD_HEIGHT-1; }
    cal_y = y;
  }

  o = lcd_getorientation();
  if(o == 270)
  {
    return cal_x;
  }
  else if(o == 180)
  {
    return LCD_HEIGHT-1-cal_y;
  }
  else if(o ==  90)
  {
    return LCD_WIDTH-1-cal_x;
  }
  else //if(o == 0)
  {
    return cal_y;
  }
}


uint32_t tp_getx(void)
{
  uint32_t o, x, y;

  if(raw_x != last_x)
  {
    last_x = raw_x;
    x = raw_x;
    y = raw_y;
    x = ((matrix.a * x) + (matrix.b * y) + matrix.c) / matrix.div;
    if(x >= LCD_WIDTH) { x = LCD_WIDTH-1; }
    cal_x = x;
  }

  if(raw_y != last_y)
  {
    last_y = raw_y;
    x = raw_x;
    y = raw_y;
    y = ((matrix.d * x) + (matrix.e * y) + matrix.f) / matrix.div;
    if(y >= LCD_HEIGHT) { y = LCD_HEIGHT-1; }
    cal_y = y;
  }

  o = lcd_getorientation();
  if(o == 270)
  {
    return LCD_HEIGHT-1-cal_y;
  }
  else if(o == 180)
  {
    return LCD_WIDTH-1-cal_x;
  }
  else if(o == 90)
  {
    return cal_y;
  }
  else //if(o == 0)
  {
    return cal_x;
  }
}


uint32_t __attribute__((optimize("Os"))) tp_calmatrix(CAL_POINT *lcd, CAL_POINT *tp)
{
  matrix.div = ((tp[0].x - tp[2].x) * (tp[1].y - tp[2].y)) -
               ((tp[1].x - tp[2].x) * (tp[0].y - tp[2].y));

  if(matrix.div == 0)
  {
    return 1;
  }

  matrix.a = ((lcd[0].x - lcd[2].x) * (tp[1].y - tp[2].y)) -
             ((lcd[1].x - lcd[2].x) * (tp[0].y - tp[2].y));

  matrix.b = (( tp[0].x -  tp[2].x) * (lcd[1].x - lcd[2].x)) -
             ((lcd[0].x - lcd[2].x) * ( tp[1].x -  tp[2].x));

  matrix.c = (tp[2].x * lcd[1].x - tp[1].x * lcd[2].x) * tp[0].y +
             (tp[0].x * lcd[2].x - tp[2].x * lcd[0].x) * tp[1].y +
             (tp[1].x * lcd[0].x - tp[0].x * lcd[1].x) * tp[2].y;

  matrix.d = ((lcd[0].y - lcd[2].y) * (tp[1].y - tp[2].y)) -
             ((lcd[1].y - lcd[2].y) * (tp[0].y - tp[2].y));

  matrix.e = (( tp[0].x -  tp[2].x) * (lcd[1].y - lcd[2].y)) -
             ((lcd[0].y - lcd[2].y) * ( tp[1].x -  tp[2].x));

  matrix.f = (tp[2].x * lcd[1].y - tp[1].x * lcd[2].y) * tp[0].y +
             (tp[0].x * lcd[2].y - tp[2].x * lcd[0].y) * tp[1].y +
             (tp[1].x * lcd[0].y - tp[0].x * lcd[1].y) * tp[2].y;

  return 0;
}


CAL_MATRIX* tp_getmatrix(void)
{
  return &matrix;
}


void tp_setmatrix(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t div)
{
  matrix.a = a;
  matrix.b = b;
  matrix.c = c;
  matrix.d = d;
  matrix.e = e;
  matrix.f = f;
  matrix.div = div;

  return;
}


uint32_t tp_rawz(void)
{
  return raw_z; 
}


uint32_t tp_rawy(void)
{
  return raw_y; 
}


uint32_t tp_rawx(void)
{
  return raw_x; 
}


void tp_read(void)
{
  uint32_t adc_cr, x1, x2, y1, y2, z, z1, z2;
  
  //save adc settings
  adc_cr = LPC_ADC->CR;

  //get z1 & z2
  XM_ADC();   //adc
  YP_ADC();   //adc
  XP_DATA(1); //Vcc
  YM_DATA(0); //GND
  ADC_READ(XM_AD, z1);
  ADC_READ(YP_AD, z2);
  z = (1023-z1) + z2;

  if(z > MIN_PRESSURE) //valid touch?
  {
    raw_z = z;
    //cal_z = 0;

    //save values
    // middle of two values
    //  raw_x = (x1+x2)/2;
    //  raw_y = (y1+y2)/2;
    //  cal_z = z;
    // accept only if both readings are equal

    //get x
    YP_ADC();   //hiZ
    YM_ADC();   //adc
    XP_DATA(1); //Vcc
    XM_DATA(0); //GND
    ADC_READ(YM_AD, x1);
    ADC_READ(YM_AD, x2);

    if(x1 && (x1 == x2))
    {
      //get y
      XP_ADC();   //hiZ
      XM_ADC();   //adc
      YP_DATA(1); //Vcc
      YM_DATA(0); //GND
      ADC_READ(XM_AD, y1);
      ADC_READ(XM_AD, y2);

      if(y1 && (y1 == y2))
      {
        raw_x = x1;
        raw_y = y1;
        cal_z = z;
      }
    }
  }
  else
  {
    raw_z = 0;
    cal_z = 0;
  }

  //stop adc
  LPC_ADC->CR = adc_cr;

  //set standby
  XP_HIZUP(); //hiZ + pull-up
  XM_HIZUP(); //hiZ + pull-up
  YP_HIZUP(); //hiZ + pull-up
  YM_HIZUP(); //hiZ + pull-up

  return;
}


void __attribute__((optimize("Os"))) tp_init(void)
{
  //init vars
  raw_x  = raw_y  = raw_z  = 0;
  last_x = last_y = 0;
  cal_x  = cal_y  = cal_z  = 0;

  //set pins
  XP_HIZUP(); //hiZ + pull-up
  XM_HIZUP(); //hiZ + pull-up
  YP_HIZUP(); //hiZ + pull-up
  YM_HIZUP(); //hiZ + pull-up

  //init adc
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<13);
  LPC_ADC->CR = (((sysclock(0)/4000000UL)-1)<< 8) | //4MHz
                                          (0<<16) | //burst off
                                        (0x0<<17) | //10bit
                                        (0x0<<24);  //stop

  return;
}


#endif //TP_SUPPORT

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "cmsis/LPC11xx.h"
#include "settings.h"
#include "main.h"
#include "iap.h"
#include "cmd.h"
#include "interface.h"
#include "lcd_mi0283qt9.h"
#include "lcd_ug12d228aa.h"
#include "lcd.h"
#include "lcd_font8x8.h"
#include "tp.h"


//set read/write byte count for position data
#if ((LCD_WIDTH > 255) || (LCD_HEIGHT > 255))
# define if_read  if_read16
# define if_write if_write16
#else
# define if_read  if_read8
# define if_write if_write8
#endif


uint32_t global_buffer[FLASH_SECTOR_BYTES/4]; //see iap.h
volatile uint32_t ms_ticks=0;
uint32_t features=0;
volatile uint32_t enc_sw=0;
volatile int32_t enc_delta=0;
uint32_t enc_last=0;
volatile uint32_t nav_sw=0;
volatile int32_t nav_hdelta=0; //left-right
volatile int32_t nav_vdelta=0; //up-down
uint32_t nav_last=0;

typedef struct
{
  uint32_t magic; //0xDEADBEEF
  uint8_t sysclock;
  uint8_t power;
  uint8_t interface;
  uint32_t baudrate;
  uint8_t address;
  uint8_t byteorder;
  uint32_t fgcolor;
  uint32_t bgcolor;
  CAL_MATRIX tp;
  uint32_t padding;
} SETTINGS;
SETTINGS *usersettings = (SETTINGS*)(FLASH_BYTES-128); //last 128 bytes are for settings


void SysTick_Handler(void) //1ms
{
  ms_ticks++;

#ifndef TP_SUPPORT

  static uint32_t enc_t=0, enc_sw_t=0;
  static uint32_t nav_t=0, nav_sw_t=0, inc_h=0, inc_v=0;
  uint32_t pin, n, dif;

  if(features & FEATURE_ENC)
  {
    if(++enc_t == 5) //5ms
    {
      enc_t = 0;

      //get pin state
      pin = GPIO_GETPORT(ENC_PORT, (1<<ENC_SW)|(1<<ENC_A)|(1<<ENC_B));
 
      //check encoder phases
      n = 0;
      if(pin & (1<<ENC_A))
      {
        n = 3;
      }
      if(pin & (1<<ENC_B))
      {
        n ^= 1; //convert gray to binary
      }
      dif = enc_last - n; //difference last - new
      if(dif & 1) //bit 0 = value (1)
      {               
        enc_last = n; // store new as next last
        enc_delta += (dif & 2) - 1; //bit 1 = direction (+/-)
      }

      //check encoder switch
      if((pin & (1<<ENC_SW)) == 0)
      {
        enc_sw_t++;
        if(enc_sw_t >= (1000/5)) //1000ms
        {
          enc_sw |= 0x02;
        }
        if(enc_sw_t >= (100/5)) //100ms
        {
          enc_sw |= 0x01;
        }
      }
      else
      {
        enc_sw_t = 0;
      }
    }
  }

  if(features & FEATURE_NAV)
  {
    if(++nav_t == 25) //25ms
    {
      nav_t = 0;

      //get pin state
      n        = GPIO_GETPORT(NAV_PORT, (1<<NAV_SW)|(1<<NAV_A)|(1<<NAV_B)|(1<<NAV_C)|(1<<NAV_D));
      pin      = n & nav_last;
      nav_last = n;

      //check nav switches
      if((pin & (1<<NAV_C)) == 0) //right
      {
        nav_sw |= 0x10;
        nav_hdelta += inc_h;
        inc_h = 0;
      }
      else if((pin & (1<<NAV_A)) == 0) //left
      {
        nav_sw |= 0x20;
        nav_hdelta -= inc_h;
        inc_h = 0;
      }
      else
      {
        inc_h = 1;
      }

      if((pin & (1<<NAV_D)) == 0) //up
      {
        nav_sw |= 0x40;
        nav_vdelta += inc_v;
        inc_v = 0;
      }
      else if((pin & (1<<NAV_B)) == 0) //down
      {
        nav_sw |= 0x80;
        nav_vdelta -= inc_v;
        inc_v = 0;
      }
      else
      {
        inc_v = 1;
      }

      if((pin & (1<<NAV_SW)) == 0)
      {
        nav_sw_t++;
        if(nav_sw_t >= (1000/25)) //1000ms
        {
          nav_sw |= 0x02;
        }
        if(nav_sw_t >= (100/25)) //100ms
        {
          nav_sw |= 0x01;
        }
      }
      else
      {
        nav_sw_t = 0;
      }
    }
  }

#endif

  return;
}


//Rotary Encoder routine by Peter Dannegger
//http://www.mikrocontroller.net/articles/Drehgeber

int32_t enc_getdelta(void)
{
  int32_t val;

  DISABLE_IRQ();

  val = enc_delta;

  //read single step encoders
  // enc_delta = 0;
  //read two step encoders
  // enc_delta = val & 1;
  // val >>= 1;
  //read four step encoders
   enc_delta = val & 3;
   val >>= 2;

  ENABLE_IRQ();

  return val;
}


uint32_t enc_getsw(void)
{
  uint32_t sw;

  sw = enc_sw;
  enc_sw = 0;

  return sw;
}


void enc_init(void)
{
  //GPIO_PORT(ENC_PORT)->DIR &= (1<<ENC_SW)|(1<<ENC_A)|(1<<ENC_B);
  IOCON_SETRPIN(ENC_PORT, ENC_SW, IOCON_R_PIO | IOCON_PULLUP | IOCON_DIGITAL); //GPIO with pull-up
  IOCON_SETRPIN(ENC_PORT, ENC_A,  IOCON_R_PIO | IOCON_PULLUP | IOCON_DIGITAL); //GPIO with pull-up
  IOCON_SETRPIN(ENC_PORT, ENC_B,  IOCON_R_PIO | IOCON_PULLUP | IOCON_DIGITAL); //GPIO with pull-up

  if(GPIO_GETPIN(ENC_PORT, ENC_A))
  {
    enc_last = 3;
  }
  if(GPIO_GETPIN(ENC_PORT, ENC_B))
  {
    enc_last ^= 1; //convert gray to binary
  }
  enc_delta = 0;
  enc_sw = 0;

  return;
}


int32_t nav_gethdelta(void)
{
  int32_t val;

  val = nav_hdelta;
  nav_hdelta = 0;

  return val;
}


int32_t nav_getvdelta(void)
{
  int32_t val;

  val = nav_vdelta;
  nav_vdelta = 0;

  return val;
}


uint32_t nav_getsw(void)
{
  uint32_t sw;

  sw = nav_sw;
  nav_sw = 0;

  return sw;
}


void nav_init(void)
{
  //GPIO_PORT(NAV_PORT)->DIR &= (1<<NAV_SW)|(1<<NAV_A)|(1<<NAV_B)|(1<<NAV_C)|(1<<NAV_D);
  IOCON_SETRPIN(NAV_PORT, NAV_SW, IOCON_R_PIO | IOCON_PULLUP | IOCON_DIGITAL); //GPIO with pull-up
  IOCON_SETRPIN(NAV_PORT, NAV_A,  IOCON_R_PIO | IOCON_PULLUP | IOCON_DIGITAL); //GPIO with pull-up
  IOCON_SETRPIN(NAV_PORT, NAV_B,  IOCON_R_PIO | IOCON_PULLUP | IOCON_DIGITAL); //GPIO with pull-up
  IOCON_SETPIN(NAV_PORT,  NAV_C,  IOCON_PIO   | IOCON_PULLUP | IOCON_DIGITAL); //GPIO with pull-up
  IOCON_SETRPIN(NAV_PORT, NAV_D,  IOCON_R_PIO | IOCON_PULLUP | IOCON_DIGITAL); //GPIO with pull-up

  nav_sw = 0;
  nav_hdelta = 0;
  nav_vdelta = 0;
  nav_last = 0;

  return;
}


uint32_t ldr_read(void)
{
  uint32_t v1, v2;

  //set AD4 to output and high
  IOCON_SETRPIN(ADC_PORT, ADC_4, IOCON_R_PIO | IOCON_NOPULL | IOCON_DIGITAL);
  GPIO_SETPIN(ADC_PORT, ADC_4);
  GPIO_PORT(ADC_PORT)->DIR |= (1<<ADC_4);

  //read AD5
  IOCON_SETPIN(ADC_PORT, ADC_5, IOCON_ADC | IOCON_NOPULL | IOCON_ANALOG);
  ADC_READ(5, v1);
  ADC_READ(5, v2);
  //IOCON_SETPIN(ADC_PORT, ADC_5, IOCON_PIO | IOCON_NOPULL | IOCON_DIGITAL);

  //set AD4 to low
  GPIO_CLRPIN(ADC_PORT, ADC_4);

  if(v1 == v2)
  {
         if(v1 > 800) v1 = 100;
    else if(v1 > 100) v1 /= 8;
    else              v1 = 10;
    set_pwm(v1);
  }
  else
  {
    v1 = 0;
  }

  return v1;
}


uint32_t adc_read(uint32_t chn)
{
  uint32_t v=0;

  switch(chn)
  {
    case 4:
      IOCON_SETRPIN(ADC_PORT, ADC_4, IOCON_R_ADC | IOCON_NOPULL | IOCON_ANALOG);
      ADC_READ(chn, v);
      IOCON_SETRPIN(ADC_PORT, ADC_4, IOCON_R_PIO | IOCON_PULLUP | IOCON_DIGITAL);
      break;
    case 5:
      IOCON_SETPIN(ADC_PORT, ADC_5, IOCON_ADC | IOCON_NOPULL | IOCON_ANALOG);
      ADC_READ(chn, v);
      IOCON_SETPIN(ADC_PORT, ADC_5, IOCON_PIO | IOCON_PULLUP | IOCON_DIGITAL);
      break;
    case 7:
      IOCON_SETPIN(ADC_PORT, ADC_7, IOCON_ADC | IOCON_NOPULL | IOCON_ANALOG);
      ADC_READ(chn, v);
      IOCON_SETPIN(ADC_PORT, ADC_7, IOCON_PIO | IOCON_PULLUP | IOCON_DIGITAL);
      break;
    case 255:
      //set AD4 to output and high
      IOCON_SETRPIN(ADC_PORT, ADC_4, IOCON_R_PIO | IOCON_NOPULL | IOCON_DIGITAL);
      GPIO_SETPIN(ADC_PORT, ADC_4);
      GPIO_PORT(ADC_PORT)->DIR |= (1<<ADC_4);
      //read AD5
      IOCON_SETPIN(ADC_PORT, ADC_5, IOCON_ADC | IOCON_NOPULL | IOCON_ANALOG);
      ADC_READ(5, v);
      //IOCON_SETPIN(ADC_PORT, ADC_5, IOCON_PIO | IOCON_NOPULL | IOCON_DIGITAL);
      //set AD4 to low
      GPIO_CLRPIN(ADC_PORT, ADC_4);
      break;
  }

  return v;
}


uint32_t get_ms(void)
{
  return ms_ticks;
}


void delay_ms(uint32_t delay)
{
  uint32_t current = get_ms();

  while((get_ms() - current) < delay);

  return;
}


void delay(uint32_t delay)
{
  for( ; delay!=0; delay--)
  {
    NOP();
  }

  return;
}


uint32_t set_pwm(uint32_t power)
{
  if(power > 100)
  {
    power = 100;
  }

  LPC_TMR16B1->MR0 = ~((0xFFFF*power)/100);

  return power;
}


uint32_t sysclock(uint32_t clock)
{
  static uint32_t current_clock=12000000UL;
  uint32_t i, pllclksel=0, pllctrl=0;

  //get clock
  if(clock == 0)
  {
    return current_clock;
  }

  //set clock
  //Fclkout = M * Fclkin = FCCO / (2 * P)
  //FCCO = 2 * P * Fclkout
  //  Fclkin  Fclkout  MSEL   Mdiv PSEL Pdiv FCCO
  //  12 MHz  24 MHz   00001  2    10   4    192 MHz
  //  12 MHz  36 MHz   00010  3    10   4    288 MHz
  //  12 MHz  48 MHz   00011  4    01   2    192 MHz
  //  16 MHz  32 MHz   00001  2    10   4    256 MHz
  //  16 MHz  48 MHz   00010  3    01   2    192 MHz
  switch(clock)
  {
    default:
    case 12000000UL:
      current_clock = clock;
      pllclksel = (0x0<<0); //IRC osc
      pllctrl   = 0;
      break;
    case 16000000UL:
      current_clock = clock;
      pllclksel = (0x1<<0); //SYS osc
      pllctrl   = 0;
      break;
    case 24000000UL:
      current_clock = clock;
      pllclksel = (0x0<<0); //IRC osc
      pllctrl   = (0x1<<0) | (0x2<<5);
      break;
    case 32000000UL:
      current_clock = clock;
      pllclksel = (0x1<<0); //SYS osc
      pllctrl   = (0x1<<0) | (0x2<<5);
      break;
    case 36000000UL:
      current_clock = clock;
      pllclksel = (0x0<<0); //IRC osc
      pllctrl   = (0x2<<0) | (0x2<<5);
      break;
    case 48000000UL:
      current_clock = clock;
      pllclksel = (0x1<<0); //SYS osc
      pllctrl   = (0x2<<0) | (0x1<<5);
      break;
  }

  DISABLE_IRQ();

  if(pllclksel != 0) //SYS osc
  {
    LPC_SYSCON->PDRUNCFG &= ~(1<<5); //enable SYS osc
    LPC_SYSCON->SYSOSCCTRL = (0<<0); //0-20MHz
  }
  else //IRC osc
  {
    LPC_SYSCON->PDRUNCFG &= ~((1<<0)|(1<<1)); //enable IRC osc
  }
  for(i=0; i<200; i++){ NOP(); }
  LPC_SYSCON->SYSPLLCLKSEL = pllclksel;
  LPC_SYSCON->SYSPLLCLKUEN = (1<<0); //update clock source
  LPC_SYSCON->SYSPLLCLKUEN = (0<<0); //toggle update register
  LPC_SYSCON->SYSPLLCLKUEN = (1<<0); //update clock source again
  while(!(LPC_SYSCON->SYSPLLCLKUEN & (1<<0))); //wait until updated

  if(pllctrl != 0) // PLL on
  {
    LPC_SYSCON->SYSPLLCTRL = pllctrl;
    LPC_SYSCON->PDRUNCFG &= ~(1<<7); //enable PLL
    while(!(LPC_SYSCON->SYSPLLSTAT & (1<<0))); //wait for PLL to lock
    LPC_SYSCON->MAINCLKSEL = (0x3<<0); //select PLL
    LPC_SYSCON->MAINCLKUEN = (1<<0); //update clock source
    LPC_SYSCON->MAINCLKUEN = (0<<0); //toggle update reg
    LPC_SYSCON->MAINCLKUEN = (1<<0); //update clock source again
    while(!(LPC_SYSCON->MAINCLKUEN & (1<<0))); //wait until updated
  }
  else
  {
    LPC_SYSCON->MAINCLKSEL = pllclksel; //select IRC osc
    LPC_SYSCON->MAINCLKUEN = (1<<0); //update clock source
    LPC_SYSCON->MAINCLKUEN = (0<<0); //toggle update reg
    LPC_SYSCON->MAINCLKUEN = (1<<0); //update clock source again
    while(!(LPC_SYSCON->MAINCLKUEN & (1<<0))); //wait until updated
    LPC_SYSCON->PDRUNCFG |= (1<<7); //disable PLL
  }

  if(pllclksel != 0) //SYS osc
  {
    LPC_SYSCON->PDRUNCFG |= ((1<<0)|(1<<1)); //disable IRC osc
  }
  else //IRC osc
  {
    LPC_SYSCON->PDRUNCFG |= (1<<5); //disable SYS osc
  }

  //set prescaler for backlight PWM
  if(current_clock >= 40000000UL)
  {
    LPC_TMR16B1->PR = 2; //div 4
  }
  else if(current_clock >= 20000000UL)
  {
    LPC_TMR16B1->PR = 1; //div 2
  }
  else
  {
    LPC_TMR16B1->PR = 0; //no prescaler
  }
  LPC_TMR16B1->PC = 0; //reset prescaler

  //set prescaler for adc
  LPC_ADC->CR = (((current_clock/4000000UL)-1)<<8) | (LPC_ADC->CR&0xFFFF00FF);  //stop

  //re-init systick timer
  SysTick_Config(current_clock / 1000); //1000 Hz

  //re-init interface
  if_init(INTERFACE_RESET);

  ENABLE_IRQ();

  return current_clock;
}


void init(void)
{
  //disable interrupts
  DISABLE_IRQ();

  //init BOD 
  LPC_SYSCON->BODCTRL = (2<<0)|(1<<4); //2.35V - 2.43V

  //set power down config
  LPC_SYSCON->PDRUNCFG &= ~(1<<4); //enable ADC

  //init system AHB
  LPC_SYSCON->SYSAHBCLKDIV = 0x00000001; //clock divider
  #define AHB_WDT    (1<<15)
  #define AHB_CT16B0 (1<< 7)
  #define AHB_CT16B1 (1<< 8)
  #define AHB_CT32B0 (1<< 9)
  #define AHB_CT32B1 (1<<10)
  #define AHB_GPIO   (1<< 6)
  #define AHB_IOCON  (1<<16)
  #define AHB_SSP0   (1<<11)
  #define AHB_SSP1   (1<<18)
  #define AHB_I2C    (1<< 5)
  #define AHB_UART   (1<<12)
  #define AHB_ADC    (1<<13)
  LPC_SYSCON->SYSAHBCLKCTRL = 0x1F | AHB_CT16B1 | AHB_GPIO | AHB_IOCON | AHB_ADC; //CT16 = PWM for backlight

  //init pins
  GPIO_PORT(SS_PORT)->DIR &= (1<<SS_PIN);
  IOCON_SETPIN(SS_PORT, SS_PIN, IOCON_PULLUP);
  IOCON_SETPIN(UART_PORT, RX_PIN, IOCON_PULLUP);
  IOCON_SETPIN(SPI_PORT, MOSI_PIN, IOCON_PULLUP);
  GPIO_PORT(IO_PORT)->DIR &= (1<<IO_PIN);
  IOCON_SETPIN(IO_PORT, IO_PIN, IOCON_PULLUP);
  GPIO_PORT(LED_PORT)->DIR |= (1<<LED_PIN);
  GPIO_CLRPIN(LED_PORT, LED_PIN);
  GPIO_PORT(PWM_PORT)->DIR |= (1<<PWM_PIN);
  GPIO_CLRPIN(PWM_PORT, PWM_PIN);

  //init PWM for backlight
  LPC_IOCON->PIO1_9 = (0x1<<0); //PIO1_9/CT16B1MAT0 -> PWM
  LPC_TMR16B1->TC   = 0;
  LPC_TMR16B1->PR   = 0; //no prescale
  LPC_TMR16B1->PC   = 0;
  LPC_TMR16B1->CTCR = 0;
  LPC_TMR16B1->MCR  = 0;
  LPC_TMR16B1->MR0  = ~((0xFFFF*0)/100); //0%
  LPC_TMR16B1->PWMC = (1<<0); //PWM chn 0 on
  LPC_TMR16B1->EMR  = (1<<0)|(2<<4); //enable PIO1_9/CT16B1MAT0
  LPC_TMR16B1->TCR  = (1<<0); //enable timer

  //init adc
  LPC_ADC->CR = (((sysclock(0)/4000000UL)-1)<< 8) | //4MHz
                                          (0<<16) | //burst off
                                        (0x0<<17) | //10bit
                                        (0x0<<24);  //stop

  //init systick timer
  ms_ticks = 0;
  SysTick_Config(sysclock(0) / 1000); //1000 Hz

  //enable interrupts
  ENABLE_IRQ();

  return;
}


void cmd_ctrl_save(uint8_t interface, uint32_t baudrate, uint8_t address, uint8_t clock, uint8_t power, uint8_t byteorder, uint32_t fgcolor, uint32_t bgcolor)
{
  SETTINGS s;

#ifdef TP_SUPPORT
  CAL_MATRIX *tp;
  tp = tp_getmatrix();
  s.tp.a = tp->a;
  s.tp.b = tp->b;
  s.tp.c = tp->c;
  s.tp.d = tp->d;
  s.tp.e = tp->e;
  s.tp.f = tp->f;
  s.tp.div = tp->div;
#endif

  s.bgcolor   = bgcolor;
  s.fgcolor   = fgcolor; 
  s.byteorder = byteorder;
  s.address   = address;
  s.baudrate  = baudrate;
  s.interface = interface;
  s.power     = power;
  s.sysclock  = clock;
  s.magic     = 0xDEADBEEFUL;

  iap_write((uint32_t*)&s, sizeof(s)/4, (uint32_t*)global_buffer); //size in 32bit words

  return;
}


#ifdef TP_SUPPORT

void cmd_tp_calibrate(uint32_t fgcolor, uint32_t bgcolor)
{
  uint32_t i;
  CAL_POINT lcd_points[3] = {CAL_POINT1, CAL_POINT2, CAL_POINT3}; //calibration point postions
  CAL_POINT tp_points[3];

  tp_init(); //enable/reset touch

  tp_read();
  if(tp_rawz())
  {
     lcd_clear(bgcolor);
     lcd_drawtext(10, 10, "Release Touchpanel.", 1, fgcolor, 0, 0);
     while(tp_rawz() > MIN_PRESSURE){ tp_read(); };
  }

  do
  {
    //clear screen and wait for touch release
    lcd_clear(bgcolor);
    lcd_setorientation(180);
    lcd_drawtext(LCD_CENTER, (LCD_HEIGHT/2)+10, "Calibration", 2, RGB(128,128,128), 0, 0);
    lcd_setorientation(0);
    lcd_drawtext(LCD_CENTER, (LCD_HEIGHT/2)+10, "Calibration", 2, RGB(128,128,128), 0, 0);

    for(i=0; i<3; )
    {
      //draw point
      lcd_drawcircle(lcd_points[i].x, lcd_points[i].y, 15, fgcolor);
      lcd_drawline(lcd_points[i].x-10, lcd_points[i].y,    lcd_points[i].x-3,  lcd_points[i].y,    fgcolor);
      lcd_drawline(lcd_points[i].x+3,  lcd_points[i].y,    lcd_points[i].x+10, lcd_points[i].y,    fgcolor);
      lcd_drawline(lcd_points[i].x,    lcd_points[i].y-10, lcd_points[i].x,    lcd_points[i].y-3,  fgcolor);
      lcd_drawline(lcd_points[i].x,    lcd_points[i].y+3,  lcd_points[i].x,    lcd_points[i].y+10, fgcolor);

      //touch detected? -> save point
      tp_read();
      if(tp_getz())
      {
        //mark point
        lcd_fillcircle(lcd_points[i].x, lcd_points[i].y, 4, RGB(255,0,0));
        //wait and clear point
        delay_ms(400);
        lcd_fillcircle(lcd_points[i].x, lcd_points[i].y, 15, bgcolor);
        //save point
        tp_points[i].x = tp_rawx();
        tp_points[i].y = tp_rawy();
        i++;
        //wait till press is over
        while(tp_rawz() > MIN_PRESSURE){ tp_read(); };
      }

      //data available
      if(if_available())
      {
        if(if_read8() == 0)
        {
          i = 666;
          break;
        }
      }
    }
  }
  while((i!=666) && (tp_calmatrix(lcd_points, tp_points)!=0)); //calculate calibration matrix

  tp_init(); //reset touch

  lcd_clear(bgcolor);

  return;
}

#endif


void cmd_lcd_test(uint32_t fgcolor, uint32_t bgcolor)
{
  uint32_t c=1, f_save=features;
  char tmp[32];
#ifdef TP_SUPPORT
  uint32_t x, y, z, last_x=0, last_y=0;

  tp_init();
#else
  uint32_t sw;
  int32_t pos=0, hpos=0, vpos=0;

  enc_init();
  features = FEATURE_ENC;
#endif

  lcd_fillrect(0,                   0, (LCD_WIDTH-1)/3,     LCD_HEIGHT-1, RGB(255,0,0));
  lcd_fillrect((LCD_WIDTH-1)/3,     0, ((LCD_WIDTH-1)/3)*2, LCD_HEIGHT-1, RGB(0,255,0));
  lcd_fillrect(((LCD_WIDTH-1)/3)*2, 0, LCD_WIDTH-1,         LCD_HEIGHT-1, RGB(0,0,255));

  delay_ms(1500);
  lcd_clear(bgcolor);

  lcd_setorientation(  0); lcd_drawrect(10, 20, 40, 40, RGB(200,  0,  0)); lcd_drawtext(15, 25, "0  ", 0, RGB(200,  0,  0), 0, 0);
  lcd_setorientation( 90); lcd_drawrect(10, 20, 40, 40, RGB(  0,200,  0)); lcd_drawtext(15, 25, "90 ", 0, RGB(  0,200,  0), 0, 0);
  lcd_setorientation(180); lcd_drawrect(10, 20, 40, 40, RGB(  0,  0,200)); lcd_drawtext(15, 25, "180", 0, RGB(  0,  0,200), 0, 0);
  lcd_setorientation(270); lcd_drawrect(10, 20, 40, 40, RGB(200,  0,200)); lcd_drawtext(15, 25, "270", 0, RGB(200,  0,200), 0, 0);

  lcd_setorientation(0);
  lcd_drawline(0, LCD_WIDTH/4*1, LCD_WIDTH-1, LCD_WIDTH/4*1, RGB(120,120,120));
  lcd_drawline(0, LCD_WIDTH/4*2, LCD_WIDTH-1, LCD_WIDTH/4*2, RGB(120,120,120));
  lcd_drawline(0, LCD_WIDTH/4*3, LCD_WIDTH-1, LCD_WIDTH/4*3, RGB(120,120,120));
  lcd_drawline(LCD_WIDTH/4*1, 0, LCD_WIDTH/4*1, LCD_HEIGHT-1, RGB(120,120,120));
  lcd_drawline(LCD_WIDTH/4*2, 0, LCD_WIDTH/4*2, LCD_HEIGHT-1, RGB(120,120,120));
  lcd_drawline(LCD_WIDTH/4*3, 0, LCD_WIDTH/4*3, LCD_HEIGHT-1, RGB(120,120,120));
  lcd_drawcircle(LCD_WIDTH/2, LCD_HEIGHT/2, 40, RGB(120,120,120));
  lcd_drawtext(2, 2, "FW "VERSION, 0, RGB(120,120,120), 0, 0);
  lcd_drawtext(LCD_CENTER, LCD_CENTER, "Test", 2, RGB(120,120,120), 0, 0);

  do
  {
#ifdef TP_SUPPORT
    tp_read();
    z = tp_getz();
    if(z)
    {
      x = tp_getx();
      y = tp_gety();
      if((x!=last_x) || (y!=last_y))
      {
        last_x = x;
        last_y = y;
        sprintf(tmp, " X %03i Y %03i Z %03i", x, y, z);
        lcd_drawtext(80, 2, tmp, 0, fgcolor, bgcolor, 1);
        lcd_fillcircle(x, y, 4, RGB(255,0,0)); //lcd_drawpixel(x, y);
      }
      GPIO_SETPIN(LED_PORT, LED_PIN); //LED on
    }
    else
    {
      GPIO_CLRPIN(LED_PORT, LED_PIN); //LED off
    }

    if(features & FEATURE_LDR)
    {
      x = ldr_read();
      if(x)
      {
        sprintf(tmp, "LDR %03i", x);
        lcd_drawtext(12, 12, tmp, 0, fgcolor, bgcolor, 1);
      }
    }

#else //TP_SUPPORT
    if(features == FEATURE_ENC)
    {
      pos += enc_getdelta();
      sprintf(tmp, "%03i", pos);
      sw = enc_getsw();
    }
    else //if(features == FEATURE_NAV)
    {
      hpos += nav_gethdelta();
      vpos += nav_getvdelta();
      sprintf(tmp, "%03i %03i", hpos, vpos);
      sw = nav_getsw();
    }
    lcd_drawtext(5, 12, tmp, 0, fgcolor, bgcolor, 1);
    if(sw)
    {
      GPIO_SETPIN(LED_PORT, LED_PIN); //LED on
      if(sw & 0x02)
      {
        if(features == FEATURE_ENC)
        {
          lcd_drawtext(5, 32, "NAV", 0, fgcolor, bgcolor, 1);
          nav_init();
          features = FEATURE_NAV;
        }
        else //if(features == FEATURE_NAV)
        {
          lcd_drawtext(5, 32, "ENC", 0, fgcolor, bgcolor, 1);
          enc_init();
          features = FEATURE_ENC;
        }
        delay_ms(500);
        while(enc_getsw() || nav_getsw());
      }
      else if(sw & 0x01)
      {
        lcd_drawtext(5, 22, "press", 0, fgcolor, bgcolor, 1);
      }
    }
    else
    {
      GPIO_CLRPIN(LED_PORT, LED_PIN); //LED off
    }
#endif
    if(if_available())
    {
      c = if_read8();
    }
  }while(c != 0);

  GPIO_CLRPIN(LED_PORT, LED_PIN); //LED off

  lcd_clear(bgcolor);

  features = f_save;

  return;
}


void cmd_lcd_terminal(uint32_t fgcolor, uint32_t bgcolor, uint32_t size)
{
  uint32_t x=2, y=2, c;

  lcd_clear(bgcolor);

  do
  {
    if(if_available() == 0)
    {
      continue;
    }

    c = if_read8();
    switch(c)
    {
      case '\n':
      case '\r':
        if(x > 0)
        {
          x  = 2;
          y += FONT_HEIGHT*size;
        }
        break;
      case '\t':
        c = ' ';
      default:
        x = lcd_drawchar(x, y, c, size, fgcolor, bgcolor, 1);
        break;
    }
    if(x >= lcd_getwidth())
    {
      x = 2;
      y += FONT_HEIGHT*size;
    }
    if((y+FONT_HEIGHT*size) >= lcd_getheight())
    {
      y = 2;
      lcd_clear(bgcolor);
    }
  }while(c != 0);

  lcd_clear(bgcolor);

  return;
}


void cmd_lcd_drawimage(uint32_t fgcolor, uint32_t bgcolor)
{
  uint32_t a, b, c, e, i, n, x0, y0, x1, y1, w, h, ms;

  x0 = if_read(); //x0
  y0 = if_read(); //y0
  w  = if_read(); //w
  h  = if_read(); //h
  x1 = x0+w-1;
  y1 = y0+h-1;
  c  = if_read8(); //color mode + encoding
  e  = c & COLOR_RLE;
  c  = c & ~COLOR_RLE;

  lcd_enable();
  lcd_setarea(x0, y0, x1, y1);
  lcd_drawstart();

  switch(c)
  {
    case COLOR_BW: //black/white
      fgcolor = RGB(0,0,0);
      bgcolor = RGB(255,255,255);
      break;
    case COLOR_WB: //white/black
      fgcolor = RGB(255,255,255);
      bgcolor = RGB(0,0,0);
      break;
    case COLOR_FG: //fg/bg
      fgcolor = fgcolor;
      bgcolor = bgcolor;
      break;
    case COLOR_BG: //bg/fg
      i       = fgcolor;
      fgcolor = bgcolor;
      bgcolor = i;
      break;
  }

  ms = get_ms();
  if(e == 0) //no encoding
  {
    switch(c)
    {
      case COLOR_BW: //black/white
      case COLOR_WB: //white/black
      case COLOR_FG: //fg/bg
      case COLOR_BG: //bg/fg
        for(i=w*h; i!=0;)
        {
          if(if_available())
          {
            a = if_read8();
            for(b=0x80; (b!=0) && (i!=0); b>>=1)
            {
              if(a & b) { lcd_draw(fgcolor); }
              else      { lcd_draw(bgcolor); }
              i--;
            }
          }
          else if((get_ms()-ms) > 2000) //2s
          {
            break;
          }
        }
        break;
      case COLOR_RGB323:
      case COLOR_RGB332:
      case COLOR_RGB233:
        for(i=w*h; i!=0;)
        {
          if(if_available())
          {
            a = if_read8();
                 if(c == COLOR_RGB323){ a = RGB323toRGB565(a); }
            else if(c == COLOR_RGB332){ a = RGB332toRGB565(a); }
            else                      { a = RGB233toRGB565(a); }
            lcd_draw(a);
            i--;
          }
          else if((get_ms()-ms) > 2000) //2s
          {
            break;
          }
        }
        break;
      default:
      case COLOR_RGB565:
        for(i=w*h; i!=0;)
        {
          if(if_available())
          {
            a = if_read16();
            lcd_draw(a);
            i--;
          }
          else if((get_ms()-ms) > 2000) //2s
          {
            break;
          }
        }
        break;
      case COLOR_RGB888:
        for(i=w*h; i!=0;)
        {
          if(if_available())
          {
            a  = (if_read8()&0xF8)<<8; //R (5 bits)
            a |= (if_read8()&0xFC)<<3; //G (6 bits)
            a |= (if_read8()&0xF8)>>3; //B (5 bits)
            lcd_draw(a);
            i--;
          }
          else if((get_ms()-ms) > 2000) //2s
          {
            break;
          }
        }
        break;
    }
  }
  else if(e == COLOR_RLE)
  {
    switch(c)
    {
      case COLOR_BW: //black/white
      case COLOR_WB: //white/black
      case COLOR_FG: //fg/bg
      case COLOR_BG: //bg/fg
        for(i=w*h; i!=0;)
        {
          if(if_available())
          {
            a = if_read8();
            if(a == 0xAA)
            {
              n = if_read8(); //n
              if(n != 0)
              {
                a = if_read8();
                for(; (n!=0) && (i!=0); n--)
                {
                  for(b=0x80; (b!=0) && (i!=0); b>>=1)
                  {
                    if(a & b) { lcd_draw(fgcolor); }
                    else      { lcd_draw(bgcolor); }
                    i--;
                  }
                }
              }
              else //n = 0
              {
                break;
              }
            }
            else
            {
              for(b=0x80; (b!=0) && (i!=0); b>>=1)
              {
                if(a & b) { lcd_draw(fgcolor); }
                else      { lcd_draw(bgcolor); }
                i--;
              }
            }
          }
          else if((get_ms()-ms) > 2000) //2s
          {
            break;
          }
        }
        break;
      case COLOR_RGB323:
      case COLOR_RGB332:
      case COLOR_RGB233:
        for(i=w*h; i!=0;)
        {
          if(if_available())
          {
            a = if_read8();
            if(a == 0xAA)
            {
              n = if_read8(); //n
              if(n != 0)
              {
                a = if_read8();
                     if(c == COLOR_RGB323){ a = RGB323toRGB565(a); }
                else if(c == COLOR_RGB332){ a = RGB332toRGB565(a); }
                else                      { a = RGB233toRGB565(a); }
                for(; (n!=0) && (i!=0); n--)
                {
                  lcd_draw(a);
                  i--;
                }
              }
              else //n = 0
              {
                break;
              }
            }
            else
            {
                   if(c == COLOR_RGB323){ a = RGB323toRGB565(a); }
              else if(c == COLOR_RGB332){ a = RGB332toRGB565(a); }
              else                      { a = RGB233toRGB565(a); }
              lcd_draw(a);
              i--;
            }
          }
          else if((get_ms()-ms) > 2000) //2s
          {
            break;
          }
        }
        break;
      default:
      case COLOR_RGB565:
        for(i=w*h; i!=0;)
        {
          if(if_available())
          {
            a = if_read16();
            if(a == 0xAAAA)
            {
              n = if_read8(); //n
              if(n != 0)
              {
                a = if_read16();
                for(; (n!=0) && (i!=0); n--)
                {
                  lcd_draw(a);
                  i--;
                }
              }
              else //n = 0
              {
                break;
              }
            }
            else
            {
              lcd_draw(a);
              i--;
            }
          }
          else if((get_ms()-ms) > 2000) //2s
          {
            break;
          }
        }
        break;
      case COLOR_RGB888:
        for(i=w*h; i!=0;)
        {
          if(if_available())
          {
            a = if_read16();
            if(a == 0xAA)
            {
              c = if_read8(); //n
              if(c != 0)
              {
                a  = (if_read8()&0xF8)<<8; //R (5 bits)
                a |= (if_read8()&0xFC)<<3; //G (6 bits)
                a |= (if_read8()&0xF8)>>3; //B (5 bits)
                for(; (c!=0) && (i!=0); c--)
                {
                  lcd_draw(a);
                  i--;
                }
              }
              else //n = 0
              {
                break;
              }
            }
            else
            {
              a  =          (a&0xF8)<<8; //R (5 bits)
              a |= (if_read8()&0xFC)<<3; //G (6 bits)
              a |= (if_read8()&0xF8)>>3; //B (5 bits)
              lcd_draw(a);
              i--;
            }
          }
          else if((get_ms()-ms) > 2000) //2s
          {
            break;
          }
        }
        break;
    }
  }

  lcd_drawstop();
  lcd_disable();

  return;
}


void cmd_lcd_drawtext(uint32_t fgcolor, uint32_t bgcolor, uint32_t string)
{
  uint32_t a, c, l, s, x, y;
  uint32_t ms;

  x = if_read();  //x
  y = if_read();  //y
  s = if_read8(); //size_clear
  c = (s & 0x80) ? 0 : 1; //clear (highest bit 0x80)
  s = (s & 0x7F); //size
  s = ((s==0) || (s>(LCD_HEIGHT/FONT_HEIGHT))) ? 1 : s; //check size
  if(string == 0)
  {
    l = if_read8(); //length
  }

  if(x == LCD_CENTER) //x center
  {
    x = (lcd_getwidth()/2)-((FONT_WIDTH*s*l)/2);
    if(x >= lcd_getwidth()){ x = 0; }
  }
  if(y == LCD_CENTER) //y center
  {
    y = (lcd_getheight()/2)-((FONT_HEIGHT*s)/2);
    if(y >= lcd_getheight()){ y = 0; }
  }

  ms = get_ms();
  if(string == 0)
  {
    for(;l!=0;)
    {
      if(if_available())
      {
        a = if_read8();
        if(x < lcd_getwidth())
        {
          x = lcd_drawchar(x, y, a, s, fgcolor, bgcolor, c);
        }
        l--;
      }
      else if((get_ms()-ms) > 2000) //2s
      {
        break;
      }
    }
  }
  else
  {
    while(1)
    {
      if(if_available())
      {
        a = if_read8();
        if(a == 0)
        {
          break;
        }
        else if(x < lcd_getwidth())
        {
          x = lcd_drawchar(x, y, a, s, fgcolor, bgcolor, c);
        }
      }
      else if((get_ms()-ms) > 2000) //2s
      {
        break;
      }
    }
  }

  return;
}


int main(void)
{
  uint32_t a, b, c, d, e, color, fgcolor=RGB(0,0,0), bgcolor=RGB(255,255,255), led_power=DEFAULT_POWER;
#ifdef TP_SUPPORT
  uint32_t tp_t=0, tp_int=0, ldr_t=0;
#endif

  //init periphery
  init();

  //set interface
  a = usersettings->interface;
  b = get_ms();
  while((GPIO_GETPIN(SS_PORT, SS_PIN) == 0) && ((get_ms()-b) < 100)) //CS low (100ms timeout)
  {
    if(GPIO_GETPIN(SPI_PORT, MOSI_PIN) == 0) //MOSI low
    {
      a = INTERFACE_SPI;
    }
    else if(GPIO_GETPIN(UART_PORT, RX_PIN) == 0) //Rx low
    {
      a = INTERFACE_UART;
    }
    else
    {
      a = INTERFACE_I2C;
    }
  }

  //init lcd
  lcd_init();

  //check for config data
  if(usersettings->magic != 0xDEADBEEFUL)
  {
    sysclock(DEFAULT_CLOCK); //set system clock to default value
    lcd_clear(bgcolor);
    lcd_drawtext(10, 10, "No config data.", 1, fgcolor, 0, 0);
#if DEFAULT_POWER == 0
    set_pwm(50);
#else
    set_pwm(DEFAULT_POWER);
#endif
    delay_ms(1000);
#ifdef TP_SUPPORT
    cmd_tp_calibrate(fgcolor, bgcolor);
#endif
    cmd_ctrl_save(DEFAULT_INTERF, DEFAULT_BAUD, DEFAULT_ADDR, DEFAULT_CLOCK/1000000UL, DEFAULT_POWER, DEFAULT_ORDER, fgcolor, bgcolor);
    cmd_lcd_test(fgcolor, bgcolor);
    a = DEFAULT_INTERF;
  }

  //load config data and init touch panel, encoder, nav switch, interface
  sysclock(usersettings->sysclock*1000000UL);
#ifdef TP_SUPPORT
  if(features & FEATURE_TP)
  {
    tp_init();
  }
  tp_setmatrix(usersettings->tp.a, usersettings->tp.b, usersettings->tp.c, usersettings->tp.d, usersettings->tp.e, usersettings->tp.f, usersettings->tp.div);
#else
  if(features & FEATURE_ENC)
  {
    enc_init();
  }
  if(features & FEATURE_NAV)
  {
    nav_init();
  }
#endif
  fgcolor = usersettings->fgcolor;
  bgcolor = usersettings->bgcolor;
  lcd_clear(bgcolor);
  set_pwm(usersettings->power);
  uart_setbaudrate(usersettings->baudrate);
  i2c_setaddress(usersettings->address);
  if_setbyteorder(usersettings->byteorder);
  if_init(a);


//set_pwm(50);
//cmd_lcd_test(fgcolor, bgcolor);


  //cmd loop
  while(1)
  {
#ifdef TP_SUPPORT
    //read touch panel
    if(features & FEATURE_TP)
    {
      if((get_ms() - tp_t) >= tp_int)
      {
        tp_t = get_ms();
        tp_read();
      }
      if(if_available() == 0)
      {
        tp_int = 10; //10ms
        continue;
      }
      else
      {
        tp_int = 50; //50ms (slow down tp read when data is available)
      }
    }

    if(features & FEATURE_LDR)
    {
     if(if_available() == 0)
     {
       if((get_ms() - ldr_t) >= 1000) //every second
        {
          ldr_t = get_ms();
          ldr_read();
        }
      }
    }
#endif

    c = if_read8();
    switch(c)
    {
      default:
      case CMD_NOP1:
      case CMD_NOP2:
        if_flush();
        break;
      case CMD_VERSION:
        if_writestr(VERSION);
        if_flush();
        break;
      case CMD_TEST:
        if(led_power == 0)
        {
          set_pwm(50);
        }
        cmd_lcd_test(fgcolor, bgcolor);
        if_write8(CMD_TEST);
        if_flush();
        break;
      case CMD_FEATURES:
        a = FEATURE_LCD;
#ifdef TP_SUPPORT
        a |= FEATURE_TP;
        a |= FEATURE_LDR;
#else
        a |= FEATURE_ENC;
        a |= FEATURE_NAV;
#endif
        if_write8(a);
        if_flush();
        break;
      case CMD_CTRL:
        switch(if_read8())
        {
          case CMD_CTRL_SAVE:      cmd_ctrl_save(if_getinterface(), if_getbaudrate(), if_getaddress(), sysclock(0)/1000000, led_power, if_getbyteorder(), fgcolor, bgcolor);  break;
          case CMD_CTRL_INTERFACE: if_init(if_read8());            break;
          case CMD_CTRL_BAUDRATE:  uart_setbaudrate(if_read32());  break;
          case CMD_CTRL_ADDRESS:   i2c_setaddress(if_read8());     break;
          case CMD_CTRL_BYTEORDER: if_setbyteorder(if_read8());    break;
          case CMD_CTRL_SYSCLOCK:  sysclock(if_read8()*1000000UL); break;
          case CMD_CTRL_FEATURES:
            a = if_read8();
#ifdef TP_SUPPORT
            if(a & FEATURE_TP) { tp_init();  };
#else
            if(a & FEATURE_ENC){ enc_init(); };
            if(a & FEATURE_NAV){ nav_init(); };
#endif
            features = a;
            break;
        }
        break;
      case CMD_PIN:
        break;
      case CMD_ADC:
        if_write16(adc_read(if_read8()));
        break;

      case CMD_LCD_LED:
        a = if_read8();
        if(a == 255)
        {
          if_write8(led_power);
        }
        else
        {
          led_power = set_pwm(a);
        }
        break;
      case CMD_LCD_RESET:
        lcd_init();
        break;
      case CMD_LCD_POWER:
        lcd_power((if_read8()));
        break;
      case CMD_LCD_RAWCMD:
        lcd_wrcmd8(if_read8());
         break;
      case CMD_LCD_RAWDAT:
        lcd_wrdata8(if_read8());
        break;

      case CMD_LCD_ORIENTATION:
        lcd_setorientation(if_read8());
        break;
      case CMD_LCD_WIDTH:
        if_write16(lcd_getwidth());
        if_flush();
        break;
      case CMD_LCD_HEIGHT:
        if_write16(lcd_getheight());
        if_flush();
        break;
      case CMD_LCD_INVERT:
        lcd_invert(if_read8());
        break;
      case CMD_LCD_FGCOLOR:
        fgcolor = if_read16();
        break;
      case CMD_LCD_BGCOLOR:
        bgcolor = if_read16();
        break;
      case CMD_LCD_TERMINAL:
        cmd_lcd_terminal(fgcolor, bgcolor, 1);
        if_write8(CMD_LCD_TERMINAL);
        if_flush();
        break;
      case CMD_LCD_DRAWIMAGE:
        cmd_lcd_drawimage(fgcolor, bgcolor);
        break;

      case CMD_LCD_CLEAR:
        color = if_read16(); //color
        lcd_clear(color);
        break;
      case CMD_LCD_CLEARFG:
        lcd_clear(fgcolor);
        break;
      case CMD_LCD_CLEARBG:
        lcd_clear(bgcolor);
        break;

      case CMD_LCD_DRAWPIXEL:
        a = if_read(); //x
        b = if_read(); //y
        color = if_read16(); //color
        lcd_drawpixel(a, b, color);
        break;
      case CMD_LCD_DRAWPIXELFG:
        a = if_read(); //x
        b = if_read(); //y
        lcd_drawpixel(a, b, fgcolor);
        break;
      case CMD_LCD_DRAWPIXELBG:
        a = if_read(); //x
        b = if_read(); //y
        lcd_drawpixel(a, b, bgcolor);
        break;

      case CMD_LCD_DRAWLINE:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //x1
        d = if_read(); //y1
        color = if_read16(); //color
        lcd_drawline(a, b, c, d, color);
        break;
      case CMD_LCD_DRAWLINEFG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //x1
        d = if_read(); //y1
        lcd_drawline(a, b, c, d, fgcolor);
        break;
      case CMD_LCD_DRAWLINEBG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //x1
        d = if_read(); //y1
        lcd_drawline(a, b, c, d, bgcolor);
        break;

      case CMD_LCD_DRAWLINES:
        color = if_read16(); //color
        a = if_read8(); //n
        b = if_read(); //x0
        c = if_read(); //y0
        for(;a!=0; a--)
        {
          d = if_read(); //xn
          e = if_read(); //yn
          lcd_drawline(b, c, d, e, color);
          b = d; c = e;
        }
        break;
      case CMD_LCD_DRAWLINESFG:
        a = if_read8(); //n
        b = if_read(); //x0
        c = if_read(); //y0
        for(;a!=0; a--)
        {
          d = if_read(); //xn
          e = if_read(); //yn
          lcd_drawline(b, c, d, e, fgcolor);
          b = d; c = e;
        }
        break;
      case CMD_LCD_DRAWLINESBG:
        a = if_read8(); //n
        b = if_read(); //x0
        c = if_read(); //y0
        for(;a!=0; a--)
        {
          d = if_read(); //xn
          e = if_read(); //yn
          lcd_drawline(b, c, d, e, bgcolor);
          b = d; c = e;
        }
        break;

      case CMD_LCD_DRAWRECT:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read()+a-1; //w
        d = if_read()+b-1; //h
        color = if_read16(); //color
        lcd_drawrect(a, b, c, d, color);
        break;
      case CMD_LCD_DRAWRECTFG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read()+a-1; //w
        d = if_read()+b-1; //h
        lcd_drawrect(a, b, c, d, fgcolor);
        break;
      case CMD_LCD_DRAWRECTBG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read()+a-1; //w
        d = if_read()+b-1; //h
        lcd_drawrect(a, b, c, d, bgcolor);
        break;

      case CMD_LCD_FILLRECT:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read()+a-1; //w
        d = if_read()+b-1; //h
        color = if_read16(); //color
        lcd_fillrect(a, b, c, d, color);
        break;
      case CMD_LCD_FILLRECTFG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read()+a-1; //w
        d = if_read()+b-1; //h
        lcd_fillrect(a, b, c, d, fgcolor);
        break;
      case CMD_LCD_FILLRECTBG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read()+a-1; //w
        d = if_read()+b-1; //h
        lcd_fillrect(a, b, c, d, bgcolor);
        break;

      case CMD_LCD_DRAWRNDRECT:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read()+a-1; //w
        d = if_read()+b-1; //h
        e = if_read(); //r
        color = if_read16(); //color
        lcd_drawrndrect(a, b, c, d, e, color);
        break;
      case CMD_LCD_DRAWRNDRECTFG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read()+a-1; //w
        d = if_read()+b-1; //h
        e = if_read(); //r
        lcd_drawrndrect(a, b, c, d, e, fgcolor);
        break;
      case CMD_LCD_DRAWRNDRECTBG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read()+a-1; //w
        d = if_read()+b-1; //h
        e = if_read(); //r
        lcd_drawrndrect(a, b, c, d, e, bgcolor);
        break;

      case CMD_LCD_FILLRNDRECT:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read()+a-1; //w
        d = if_read()+b-1; //h
        e = if_read(); //r
        color = if_read16(); //color
        lcd_fillrndrect(a, b, c, d, e, color);
        break;
      case CMD_LCD_FILLRNDRECTFG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read()+a-1; //w
        d = if_read()+b-1; //h
        e = if_read(); //r
        lcd_fillrndrect(a, b, c, d, e, fgcolor);
        break;
      case CMD_LCD_FILLRNDRECTBG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read()+a-1; //w
        d = if_read()+b-1; //h
        e = if_read(); //r
        lcd_fillrndrect(a, b, c, d, e, bgcolor);
        break;

      case CMD_LCD_DRAWCIRCLE:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //r
        color = if_read16(); //color
        lcd_drawcircle(a, b, c, color);
        break;
      case CMD_LCD_DRAWCIRCLEFG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //r
        lcd_drawcircle(a, b, c, fgcolor);
        break;
      case CMD_LCD_DRAWCIRCLEBG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //r
        lcd_drawcircle(a, b, c, bgcolor);
        break;

      case CMD_LCD_FILLCIRCLE:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //r
        color = if_read16(); //color
        lcd_fillcircle(a, b, c, color);
        break;
      case CMD_LCD_FILLCIRCLEFG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //r
        lcd_fillcircle(a, b, c, fgcolor);
        break;
      case CMD_LCD_FILLCIRCLEBG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //r
        lcd_fillcircle(a, b, c, bgcolor);
        break;

      case CMD_LCD_DRAWELLIPSE:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //r_x
        d = if_read(); //r_y
        color = if_read16(); //color
        lcd_drawellipse(a, b, c, d, color);
        break;
      case CMD_LCD_DRAWELLIPSEFG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //r_x
        d = if_read(); //r_y
        lcd_drawellipse(a, b, c, d, fgcolor);
        break;
      case CMD_LCD_DRAWELLIPSEBG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //r_x
        d = if_read(); //r_y
        lcd_drawellipse(a, b, c, d, bgcolor);
        break;

      case CMD_LCD_FILLELLIPSE:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //r_x
        d = if_read(); //r_y
        color = if_read16(); //color
        lcd_fillellipse(a, b, c, d, color);
        break;
      case CMD_LCD_FILLELLIPSEFG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //r_x
        d = if_read(); //r_y
        lcd_fillellipse(a, b, c, d, fgcolor);
        break;
      case CMD_LCD_FILLELLIPSEBG:
        a = if_read(); //x0
        b = if_read(); //y0
        c = if_read(); //r_x
        d = if_read(); //r_y
        lcd_fillellipse(a, b, c, d, bgcolor);
        break;

      case CMD_LCD_DRAWTEXT:
        a = if_read(); //fg_color
        b = if_read(); //bg_color
        cmd_lcd_drawtext(a, b, 0);
        break;
      case CMD_LCD_DRAWTEXTFG:
        cmd_lcd_drawtext(fgcolor, bgcolor, 0);
        break;
      case CMD_LCD_DRAWTEXTBG:
        cmd_lcd_drawtext(bgcolor, fgcolor, 0);
        break;

      case CMD_LCD_DRAWSTRING:
        a = if_read(); //fg_color
        b = if_read(); //bg_color
        cmd_lcd_drawtext(a, b, 1);
        break;
      case CMD_LCD_DRAWSTRINGFG:
        cmd_lcd_drawtext(fgcolor, bgcolor, 1);
        break;
      case CMD_LCD_DRAWSTRINGBG:
        cmd_lcd_drawtext(bgcolor, fgcolor, 1);
        break;

#ifdef TP_SUPPORT

      case CMD_TP_POS:
        if_write(tp_getx());
        if_write(tp_gety());
        if_write(tp_getz()); //tp_getz() tp_rawz()
        if_flush();
        break;
      case CMD_TP_X:
        if_write(tp_getx());
        if_flush();
        break;
      case CMD_TP_Y:
        if_write(tp_gety());
        if_flush();
        break;
      case CMD_TP_Z:
        if_write(tp_rawz()); //tp_rawz() tp_getz()
        if_flush();
        break;
      case CMD_TP_WAITPRESS:
        while(tp_getz() == 0){ tp_read(); delay_ms(5); }
        if_write(tp_getx());
        if_write(tp_gety());
        if_flush();
        break;
      case CMD_TP_WAITRELEASE:
        while(tp_rawz() != 0){ tp_read(); delay_ms(5); }
        if_write(tp_getx());
        if_write(tp_gety());
        if_flush();
        break;
      case CMD_TP_WAITMOVE:
        while(tp_rawz() == 0){ tp_read(); delay_ms(5); }
        a = tp_getx();
        b = tp_gety();
        while(tp_rawz() != 0){ tp_read(); delay_ms(5); }
        c = tp_getx();
        d = tp_gety();
        e = 0;
             if((a > c) && ((a-c) >= 20)) { e |= 0x01; } //x-
        else if((a < c) && ((c-a) >= 20)) { e |= 0x02; } //x+
             if((b > d) && ((b-d) >= 20)) { e |= 0x04; } //y-
        else if((b < d) && ((d-b) >= 20)) { e |= 0x08; } //y+
        if_write8(e);
        if_flush();
        break;
      case CMD_TP_CALIBRATE:
        if(led_power == 0)
        {
          set_pwm(50);
        }
        cmd_tp_calibrate(fgcolor, bgcolor);
        if_write8(CMD_TP_CALIBRATE);
        if_flush();
        break;

#else //TP_SUPPORT

      case CMD_ENC_POS:
        if_write8(enc_getdelta());
        if_write8(enc_getsw());
        if_flush();
        break;
      case CMD_ENC_SW:
        if_write8(enc_getsw());
        if_flush();
        break;
      case CMD_ENC_WAITPRESS:
        while(enc_getsw() == 0);
        if_write8(enc_getdelta());
        if_write8(enc_getsw());
        if_flush();
        break;
      case CMD_ENC_WAITRELEASE:
        while(GPIO_GETPIN(ENC_PORT, ENC_SW) == 0);
        if_write8(enc_getdelta());
        if_write8(enc_getsw());
        if_flush();
        break;

      case CMD_NAV_POS:
        if_write8(nav_gethdelta());
        if_write8(nav_getvdelta());
        if_write8(nav_getsw());
        if_flush();
        break;
      case CMD_NAV_SW:
        if_write8(nav_getsw());
        if_flush();
        break;
      case CMD_NAV_WAITPRESS:
        while(nav_getsw() == 0);
        if_write8(nav_gethdelta());
        if_write8(nav_getvdelta());
        if_write8(nav_getsw());
        if_flush();
        break;
      case CMD_NAV_WAITRELEASE:
        while(GPIO_GETPIN(NAV_PORT, NAV_SW) == 0);
        if_write8(nav_gethdelta());
        if_write8(nav_getvdelta());
        if_write8(nav_getsw());
        if_flush();
        break;

#endif
    }
  }

  return 0;
}

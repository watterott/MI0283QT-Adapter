#ifndef _MAIN_H_
#define _MAIN_H_


//----- DEFINES -----
#define CONCATx(a, b)                  a##b
#define CONCATx3(a, b, c)              a##b##c
#define CONCATx4(a, b, c, d)           a##b##c##d
#define CONCAT(a, b)                   CONCATx(a, b)
#define CONCAT3(a, b, c)               CONCATx3(a, b, c)
#define CONCAT4(a, b, c, d)            CONCATx4(a, b, c, d)

#define GPIO_PORT(port)                CONCAT(LPC_GPIO, port)
#define GPIO_SETPIN(port, pin)         CONCAT(LPC_GPIO, port)->MASKED_ACCESS[(1<<pin)] = (1<<pin)
#define GPIO_CLRPIN(port, pin)         CONCAT(LPC_GPIO, port)->MASKED_ACCESS[(1<<pin)] = 0
#define GPIO_SETPORT(port, pins, val)  CONCAT(LPC_GPIO, port)->MASKED_ACCESS[pins] = val
#define GPIO_GETPIN(port, pin)         CONCAT(LPC_GPIO, port)->MASKED_ACCESS[(1<<pin)]
#define GPIO_GETPORT(port, pins)       CONCAT(LPC_GPIO, port)->MASKED_ACCESS[pins]

#define IOCON_PIO                      (0x01<<0) //pio (reserved pins)
#define IOCON_ADC                      (0x02<<0) //adc
#define IOCON_NOPULL                   (0x00<<3) //no pull-down/pull-up
#define IOCON_PULLDOWN                 (0x01<<3) //pull-down
#define IOCON_PULLUP                   (0x02<<3) //pull-up
#define IOCON_ANALOG                   (0x00<<7) //analog (adc pins)
#define IOCON_DIGITAL                  (0x01<<7) //digital (adc pins)
#define IOCON_SETRPIN(port, pin, val)  LPC_IOCON->CONCAT4(R_PIO, port, _, pin) = val
#define IOCON_SETPIN(port, pin, val)   LPC_IOCON->CONCAT4(PIO, port, _, pin) = val


//----- PROTOTYPES -----
void                                   SysTick_Handler(void);

#ifdef ENC_SUPPORT
int32_t                                enc_getdelta(void);
uint32_t                               enc_getsw(void);
void                                   enc_init(void);
#endif

#ifdef NAV_SUPPORT
int32_t                                nav_gethdelta(void);
int32_t                                nav_getvdelta(void);
uint32_t                               nav_getsw(void);
void                                   nav_init(void);
#endif

uint32_t                               get_ms(void);
void                                   delay_ms(uint32_t delay);
void                                   delay(uint32_t delay);
void                                   set_pwm(uint32_t power); //0-100
uint32_t                               sysclock(uint32_t clock);
void                                   init(void);

void                                   cmd_save_settings(uint8_t interface, uint32_t baudrate, uint8_t address, uint8_t sysclock, uint8_t power, uint32_t fgcolor, uint32_t bgcolor);
#ifdef TP_SUPPORT
void                                   cmd_tp_calibrate(uint32_t fgcolor, uint32_t bgcolor);
#endif
void                                   cmd_lcd_test(uint32_t fgcolor, uint32_t bgcolor);
void                                   cmd_lcd_terminal(uint32_t fgcolor, uint32_t bgcolor, uint32_t size);
void                                   cmd_lcd_drawimage(uint32_t fgcolor, uint32_t bgcolor);
void                                   cmd_lcd_drawtext(uint32_t fgcolor, uint32_t bgcolor, uint32_t string);


#endif //_MAIN_H_

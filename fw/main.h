#ifndef _MAIN_H_
#define _MAIN_H_


//----- PROTOTYPES -----
void                                   SysTick_Handler(void);

int_least8_t                           enc_getdelta(void);
uint_least8_t                          enc_getsw(void);
void                                   enc_init(void);

int_least8_t                           nav_gethdelta(void);
int_least8_t                           nav_getvdelta(void);
uint_least8_t                          nav_getsw(void);
void                                   nav_init(void);

uint_least8_t                          ldr_service(uint_least8_t power_max);
void                                   ldr_init(void);
uint_least16_t                         adc_read(uint_least8_t chn);

void                                   io_set(uint_least8_t source);
void                                   io_clr(uint_least8_t source);

uint_least32_t                         get_ms(void);
void                                   delay_ms(uint_least32_t delay);
void                                   delay(uint_least32_t delay);
uint_least8_t                          set_pwm(uint_least8_t power);
uint_least32_t                         sysclock(uint_least32_t clock);
void                                   init(void);

void                                   cmd_ctrl_save(uint_least8_t interface, uint_least32_t baudrate, uint_least8_t address, uint_least8_t sysclock, uint_least8_t power, uint_least8_t byteorder, uint_least16_t fgcolor, uint_least16_t bgcolor);
#ifdef TP_SUPPORT
void                                   cmd_tp_calibrate(uint_least16_t fgcolor, uint_least16_t bgcolor);
#endif
void                                   cmd_lcd_test(uint_least16_t fgcolor, uint_least16_t bgcolor);
void                                   cmd_lcd_terminal(uint_least16_t fgcolor, uint_least16_t bgcolor, uint_least8_t size);
void                                   cmd_lcd_drawimage(uint_least16_t fgcolor, uint_least16_t bgcolor);
void                                   cmd_lcd_drawtext(uint_least16_t fgcolor, uint_least16_t bgcolor, uint_least8_t string);


#endif //_MAIN_H_

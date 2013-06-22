#ifndef _MAIN_H_
#define _MAIN_H_


//----- PROTOTYPES -----
void                                   SysTick_Handler(void);

int32_t                                enc_getdelta(void);
uint32_t                               enc_getsw(void);
void                                   enc_init(void);

int32_t                                nav_gethdelta(void);
int32_t                                nav_getvdelta(void);
uint32_t                               nav_getsw(void);
void                                   nav_init(void);

uint32_t                               ldr_read(void);
uint32_t                               adc_read(uint32_t chn);

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

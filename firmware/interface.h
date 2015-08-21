#ifndef _INTERFACE_H_
#define _INTERFACE_H_


//----- PROTOTYPES -----
void                                   i2c_write(uint_least8_t c);
uint_least8_t                          i2c_read(void);
uint_least16_t                         i2c_available(void);
void                                   i2c_off(void);
void                                   i2c_setaddress(uint_least8_t adr);
void                                   i2c_init(void);

void                                   spi_write(uint_least8_t c);
uint_least8_t                          spi_read(void);
uint_least16_t                         spi_available(void);
void                                   spi_off(void);
void                                   spi_init(void);

void                                   uart_write(uint_least8_t c);
uint_least8_t                          uart_read(void);
uint_least16_t                         uart_available(void);
void                                   uart_off(void);
void                                   uart_setbaudrate(uint_least32_t br);
void                                   uart_init(void);

void                                   if_writestr(char *s);
void                                   if_writebuf(uint8_t *ptr, uint_least16_t size);
void                                   if_write32_little(uint_least32_t c);
void                                   if_write24_little(uint_least32_t c);
void                                   if_write16_little(uint_least16_t c);
void                                   if_write32_big(uint_least32_t c);
void                                   if_write24_big(uint_least32_t c);
void                                   if_write16_big(uint_least16_t c);
extern void                            (*if_write32)(uint_least32_t c);
extern void                            (*if_write24)(uint_least32_t c);
extern void                            (*if_write16)(uint_least16_t c);
extern void                            (*if_write8)(uint_least8_t c);
uint_least32_t                         if_read32_little(void);
uint_least32_t                         if_read24_little(void);
uint_least16_t                         if_read16_little(void);
uint_least32_t                         if_read32_big(void);
uint_least32_t                         if_read24_big(void);
uint_least16_t                         if_read16_big(void);
extern uint_least32_t                  (*if_read32)(void);
extern uint_least32_t                  (*if_read24)(void);
extern uint_least16_t                  (*if_read16)(void);
uint_least8_t                          if_read8(void);
void                                   if_flush(void);
uint_least16_t                         if_available(void);
void                                   if_setbyteorder(uint_least8_t byteorder);
uint_least8_t                          if_getbyteorder(void);
uint_least8_t                          if_getaddress(void);
uint_least32_t                         if_getbaudrate(void);
uint_least8_t                          if_getinterface(void);
uint_least8_t                          if_init(uint_least8_t interf);


#endif //_INTERFACE_H_

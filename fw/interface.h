#ifndef _INTERFACE_H_
#define _INTERFACE_H_


//----- PROTOTYPES -----
void                                   i2c_write(uint32_t c);
uint32_t                               i2c_read(void);
uint32_t                               i2c_available(void);
void                                   i2c_off(void);
void                                   i2c_setaddress(uint32_t adr);
void                                   i2c_init(void);

void                                   spi_write(uint32_t c);
uint32_t                               spi_read(void);
uint32_t                               spi_available(void);
void                                   spi_off(void);
void                                   spi_init(void);

void                                   uart_write(uint32_t c);
uint32_t                               uart_read(void);
uint32_t                               uart_available(void);
void                                   uart_off(void);
void                                   uart_setbaudrate(uint32_t br);
void                                   uart_init(void);

void                                   if_writestr(char *s);
void                                   if_writebuf(uint8_t *ptr, uint32_t size);
void                                   if_write32_little(uint32_t c);
void                                   if_write24_little(uint32_t c);
void                                   if_write16_little(uint32_t c);
void                                   if_write32_big(uint32_t c);
void                                   if_write24_big(uint32_t c);
void                                   if_write16_big(uint32_t c);
extern void                            (*if_write32)(uint32_t c);
extern void                            (*if_write24)(uint32_t c);
extern void                            (*if_write16)(uint32_t c);
extern void                            (*if_write8)(uint32_t c);
uint32_t                               if_read32_little(void);
uint32_t                               if_read24_little(void);
uint32_t                               if_read16_little(void);
uint32_t                               if_read32_big(void);
uint32_t                               if_read24_big(void);
uint32_t                               if_read16_big(void);
extern uint32_t                        (*if_read32)(void);
extern uint32_t                        (*if_read24)(void);
extern uint32_t                        (*if_read16)(void);
uint32_t                               if_read8(void);
void                                   if_flush(void);
uint32_t                               if_available(void);
void                                   if_setbyteorder(uint32_t byteorder);
uint32_t                               if_getbyteorder(void);
uint32_t                               if_getaddress(void);
uint32_t                               if_getbaudrate(void);
uint32_t                               if_getinterface(void);
uint32_t                               if_init(uint32_t interf);


#endif //_INTERFACE_H_

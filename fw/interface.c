#include <stdint.h>
#include "cmsis/LPC11xx.h"
#include "settings.h"
#include "main.h"
#include "cmd.h"
#include "interface.h"


#define RCV_BUFSIZE     2048 //bytes (8,16,32... size of input fifo)
#define SND_BUFSIZE       32 //bytes (8,16,32... size of output fifo)


uint32_t if_hw=0, if_state=0, if_addr=DEFAULT_ADDR, if_baud=DEFAULT_BAUD, if_byteorder=DEFAULT_ORDER;
uint8_t rcv_buf[RCV_BUFSIZE], snd_buf[SND_BUFSIZE];
volatile uint32_t rcv_head=0, rcv_tail=0, snd_head=0, snd_tail=0;


void i2c_write(uint32_t c)
{
  uint32_t head;

  head = snd_head;
  snd_buf[head++] = c;
  head = head & (SND_BUFSIZE-1);
  snd_head = head;

  return;
}


void I2C_IRQHandler(void)
{
  uint32_t state, head, tail;

  //if_state: 1=write, 2=read
  state = LPC_I2C->STAT;
  switch(state)
  {
    case 0x60:
    case 0x70: //general call
      if_state = 1; //write started
      LPC_I2C->CONSET = 0x04; //set AA
      LPC_I2C->CONCLR = 0x08; //clear SI
      break;

    case 0x80:
    case 0x90: //general call
      if(if_state == 1) //write
      {
        head = rcv_head;
        rcv_buf[head++] = LPC_I2C->DAT;
        head = head & (RCV_BUFSIZE-1);
        rcv_head = head;
        LPC_I2C->CONSET = 0x04; //set AA
      }
      else
      {
        LPC_I2C->CONCLR = 0x04; //clear AA
      }
      LPC_I2C->CONCLR = 0x08; //clear SI
      break;

    case 0xA8:
      if_state = 2; //read started
      head = snd_head;
      tail = snd_tail;
      if(head != tail)
      {
        LPC_I2C->DAT = snd_buf[tail++];
        tail = tail & (SND_BUFSIZE-1);
        snd_tail = tail;
      }
      LPC_I2C->CONSET = 0x04; //set AA
      LPC_I2C->CONCLR = 0x08; //clear SI
      break;

    case 0xB8:
      if(if_state == 2) //read
      {
        head = snd_head;
        tail = snd_tail;
        if(head != tail)
        {
          LPC_I2C->DAT = snd_buf[tail++];
          tail = tail & (SND_BUFSIZE-1);
          snd_tail = tail;
        }
        LPC_I2C->CONSET = 0x04; //set AA
      }
      else
      {
        LPC_I2C->CONCLR = 0x04; //clear AA
      }
      LPC_I2C->CONCLR = 0x08; //clear SI
      break;

    //case 0xA0: //stop
    //case 0xC0:
    //case 0xC8:
    default:
      if_state = 0;
      LPC_I2C->CONSET = 0x04; //set AA
      LPC_I2C->CONCLR = 0x08; //clear SI
      break;
  }

  return;
}


void i2c_off(void)
{
  NVIC_DisableIRQ(I2C_IRQn);

  LPC_I2C->CONCLR = 0x6C; //clear AA, IC, STA, I2EN

  IOCON_SETPIN(I2C_PORT, SCL_PIN, 0);
  IOCON_SETPIN(I2C_PORT, SDA_PIN, 0);

  LPC_SYSCON->PRESETCTRL    &= ~(1<<1); //I2C reset
  LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<5); //disable I2C clock

  return;
}


void i2c_setaddress(uint32_t adr)
{
  if_addr = adr;

  if(if_hw & INTERFACE_I2C)
  {
    i2c_init();
  }

  return;
}


void i2c_init(void)
{
  uint32_t sysdiv;

  if_state = 0;

  i2c_off();

  //pins and config
  sysdiv = LPC_SYSCON->SYSAHBCLKDIV;
  // fast mode plus (1MHz)
  // IOCON_SETPIN(I2C_PORT, SCL_PIN, (2<<8)|(1<<0));
  // IOCON_SETPIN(I2C_PORT, SDA_PIN, (2<<8)|(1<<0));
  // LPC_I2C->SCLL   = SYSCLOCK / sysdiv / 1000000 / 2;
  // LPC_I2C->SCLH   = SYSCLOCK / sysdiv / 1000000 / 2;
  // fast mode (400kHz)
  IOCON_SETPIN(I2C_PORT, SCL_PIN, (0<<8)|(1<<0));
  IOCON_SETPIN(I2C_PORT, SDA_PIN, (0<<8)|(1<<0));
  LPC_I2C->SCLL   = sysclock(0) / sysdiv / 400000 / 2;
  LPC_I2C->SCLH   = sysclock(0) / sysdiv / 400000 / 2;

  //syscon
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5); //enable I2C clock
  LPC_SYSCON->PRESETCTRL    |= (1<<1); //disable I2C reset

  //addr and interrupt
  LPC_I2C->ADR0   = (if_addr<<1)&0xFF; //set slave addr
  NVIC_EnableIRQ(I2C_IRQn); //enable interrupt
  LPC_I2C->CONSET = 0x44; //enable I2C + AA

  return;
}


void spi_write(uint32_t c)
{
  while((LPC_SSP0->SR & 0x02) != 0x02); //TNF
  LPC_SSP0->DR = c;

  return;
}


void SSP0_IRQHandler(void)
{
  uint32_t state, head, c;

  state = LPC_SSP0->MIS;

  if(state & 0x03) //RTIC
  {
    LPC_SSP0->ICR |= state&0x03;
  }

  head = rcv_head;
  while(LPC_SSP0->SR & 0x04) //RNE
  {
    c = LPC_SSP0->DR; //read data
    rcv_buf[head++] = c; //write data to receive fifo
    head = head & (RCV_BUFSIZE-1);
  }
  rcv_head = head;

  return;
}


void spi_off(void)
{
  NVIC_DisableIRQ(SSP0_IRQn); //disable interrupt

  LPC_SSP0->CR1 |= (1<<3); //Slave Output Disable SOD
  LPC_SSP0->CR1 &= ~(1<<1); //disable SPI

  IOCON_SETPIN(SS_PORT, SS_PIN, IOCON_PULLUP); //GPIO with pull-up
  IOCON_SETPIN(SPI_PORT, MISO_PIN, IOCON_PULLUP); //GPIO with pull-up
  IOCON_SETPIN(SPI_PORT, MOSI_PIN, IOCON_PULLUP); //GPIO with pull-up
  IOCON_SETRPIN(SPI_PORT, SCK_PIN, (1<<0)|IOCON_PULLUP); //GPIO with pull-up

  LPC_SYSCON->PRESETCTRL    &= ~(1<<0); //disable SPI0 reset
  LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<11); //disable AHB_SSP0=(1<<11) and AHB_SSP1=(1<<18)

  return;
}


void spi_init(void)
{
  uint32_t c;

  spi_off();

  //pins
  IOCON_SETPIN(SS_PORT, SS_PIN, (1<<0)|IOCON_PULLUP);
  IOCON_SETPIN(SPI_PORT, MISO_PIN, (1<<0)|IOCON_PULLUP);
  IOCON_SETPIN(SPI_PORT, MOSI_PIN, (1<<0));
  IOCON_SETRPIN(SPI_PORT, SCK_PIN, (2<<0));
  //LPC_IOCON->SCK_LOC = (0x0<<0);

  //syscon
  LPC_SYSCON->PRESETCTRL    |= (1<<0); //disable SPI0 reset
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<11); //enable AHB_SSP0=(1<<11) and AHB_SSP1=(1<<18)
  LPC_SYSCON->SSP0CLKDIV     = 1; //div = 1

  //config
  LPC_SSP0->CPSR = 2; //prescale (not used in slave mode)
  LPC_SSP0->CR0  = (0<<8)|(1<<7)|(1<<6)|(0x7<<0); //8bit, CPOL=1, CPHA=1, SCR=0 (CPHA=1 because of SSEL behavior)
  LPC_SSP0->CR1  = (0<<3)|(1<<2); //SPI slave, Slave Output Disable SOD = off
  LPC_SSP0->IMSC = (1<<2)|(1<<1); //RXIM RTIM
  //LPC_SSP0->IMSC = 0; //no interrupt

  //clear rx fifo
  while(LPC_SSP0->SR & 0x04){ c = LPC_SSP0->DR; } //RNE
  LPC_SSP0->ICR = 0x03; //clear RORIC + RTIC

  //enable SPI
  LPC_SSP0->CR1 |= (1<<1); //SPI on

  NVIC_EnableIRQ(SSP0_IRQn); //enable interrupt

  return;
}


void uart_write(uint32_t c)
{
  if(GPIO_GETPIN(SS_PORT, SS_PIN) == 0)
  {
    while(!(LPC_UART->LSR&(1<<5))); //wait for THRE
    LPC_UART->THR = c;
  }

  return;
}


void UART_IRQHandler(void)
{
  uint32_t state, head, c;

  state = LPC_UART->IIR;
  state = (state>>1)&0x07; //skip pending bit and mask identification bits

  c = LPC_UART->RBR; //read to clear interrupt flags

  if(state == 0x02) //2a - Receive Data Available (RDA)
  {
    head = rcv_head;
    rcv_buf[head++] = c;
    head = head & (RCV_BUFSIZE-1);
    rcv_head = head;
  }

  return;
}


void PIOINT0_IRQHandler(void)
{
  uint32_t c;

  GPIO_PORT(SS_PORT)->IC |= (1<<SS_PIN); //clear interrupt

  if(GPIO_GETPIN(SS_PORT, SS_PIN) == 0) //low -> active
  {
    IOCON_SETPIN(UART_PORT, TX_PIN, (1<<0)); //Tx
    while(LPC_UART->LSR&(1<<0)){ c = LPC_UART->RBR; } //RDR
    NVIC_EnableIRQ(UART_IRQn);
  }
  else
  {
    NVIC_DisableIRQ(UART_IRQn);
    IOCON_SETPIN(UART_PORT, TX_PIN, 0x00); //GPIO
  }

  return;
}


void uart_off(void)
{
  NVIC_DisableIRQ(EINT0_IRQn); //disable external interrupt
  NVIC_DisableIRQ(UART_IRQn); //disable interrupt

  LPC_UART->IER = 0; //disable IRQs

  IOCON_SETPIN(SS_PORT, SS_PIN, IOCON_PULLUP); //GPIO with pull-up
  IOCON_SETPIN(UART_PORT, RX_PIN, IOCON_PULLUP); //GPIO with pull-up
  IOCON_SETPIN(UART_PORT, TX_PIN, IOCON_PULLUP); //GPIO with pull-up

  LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<12); //disable UART clock

  return;
}


void uart_setbaudrate(uint32_t br)
{
  uint32_t div, sysdiv, uartdiv;

  if((br < 9600) || (br > 1000000))
  {
    return;
  }

  if_baud = br;

  if(if_hw & INTERFACE_UART)
  {
    sysdiv  = LPC_SYSCON->SYSAHBCLKDIV;
    uartdiv = LPC_SYSCON->UARTCLKDIV;
    div     = (((sysclock(0)/sysdiv)/uartdiv)/16)/br; //SYSAHBCLOCKDIV=1, UARTCLKDIV=1

    LPC_UART->LCR |= (1<<7); //DLAB=1
    LPC_UART->DLM  = div / 256; //divisor MSB
    LPC_UART->DLL  = div % 256; //divisor LSB
    LPC_UART->LCR &= ~(1<<7); //DLAB=0
  }

  return;
}


void uart_init(void)
{
  uint32_t c;

  uart_off();

  //pins
  IOCON_SETPIN(SS_PORT, SS_PIN, IOCON_PULLUP); //pull-up
  GPIO_PORT(SS_PORT)->IS  &= ~(1<<SS_PIN); //edge sensitive
  GPIO_PORT(SS_PORT)->IBE |=  (1<<SS_PIN); //both edges
  //GPIO_PORT(SS_PORT)->IEV &= ~(1<<SS_PIN); //high or low level/edge
  GPIO_PORT(SS_PORT)->IE  |=  (1<<SS_PIN); //interrupt mask
  GPIO_PORT(SS_PORT)->IC  |=  (1<<SS_PIN); //clear interrupt
  IOCON_SETPIN(UART_PORT, RX_PIN, (1<<0|IOCON_PULLUP)); //Rx + Pull-Up
  //IOCON_SETPIN(UART_PORT, TX_PIN, (1<<0)); //Tx
  IOCON_SETPIN(UART_PORT, TX_PIN, (0<<0)); //GPIO
  //LPC_IOCON->RXD_LOC = (0<<0);

  //syscon
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12); //enable UART clock
  LPC_SYSCON->UARTCLKDIV     = 1; //div = 1

  //config
  uart_setbaudrate(if_baud);
  LPC_UART->LCR = (0x3<<0); //8N1
  LPC_UART->IER = (1<<0); //RBRIE = RDA interrupt
  LPC_UART->FCR = (1<<2)|(1<<1)|(1<<0); //FiFo enable and reset

  //clear status
  while(!(LPC_UART->LSR&(1<<5))); //THRE
  while(LPC_UART->LSR&(1<<0)){ c = LPC_UART->RBR; } //RDR

  //enable interrupt
  if(GPIO_GETPIN(SS_PORT, SS_PIN) == 0) //low -> active
  {
    IOCON_SETPIN(UART_PORT, TX_PIN, (1<<0)); //Tx
    NVIC_EnableIRQ(UART_IRQn);
  }
  NVIC_EnableIRQ(EINT0_IRQn); //enable external interrupt

  return;
}


void if_writestr(char *s)
{
  while(*s)
  {
    if_write8(*s++);
  }

  return;
}


void if_writebuf(uint8_t *ptr, uint32_t size)
{
  for( ;size!=0; size--)
  {
    if_write8(*ptr++);
  }

  return;
}


void if_write32_little(uint32_t c)
{
  if_write8((c&0x000000FF)>>0);
  if_write8((c&0x0000FF00)>>8);
  if_write8((c&0x00FF0000)>>16);
  if_write8((c&0xFF000000)>>24);

  return;
}


void if_write24_little(uint32_t c)
{
  if_write8((c&0x000000FF)>>0);
  if_write8((c&0x0000FF00)>>8);
  if_write8((c&0x00FF0000)>>16);

  return;
}


void if_write16_little(uint32_t c)
{
  if_write8((c&0x000000FF)>>0);
  if_write8((c&0x0000FF00)>>8);

  return;
}


void if_write32_big(uint32_t c)
{
  if_write8((c&0xFF000000)>>24);
  if_write8((c&0x00FF0000)>>16);
  if_write8((c&0x0000FF00)>>8);
  if_write8((c&0x000000FF)>>0);

  return;
}


void if_write24_big(uint32_t c)
{
  if_write8((c&0x00FF0000)>>16);
  if_write8((c&0x0000FF00)>>8);
  if_write8((c&0x000000FF)>>0);

  return;
}


void if_write16_big(uint32_t c)
{
  if_write8((c&0x0000FF00)>>8);
  if_write8((c&0x000000FF)>>0);

  return;
}


void (*if_write32)(uint32_t c) = if_write32_big;
void (*if_write24)(uint32_t c) = if_write24_big;
void (*if_write16)(uint32_t c) = if_write16_big;
void (*if_write8) (uint32_t c) = i2c_write;


uint32_t if_read32_little(void)
{
  uint32_t c;

  c  = if_read8()<<0;
  c |= if_read8()<<8;
  c |= if_read8()<<16;
  c |= if_read8()<<24;

  return c;
}


uint32_t if_read24_little(void)
{
  uint32_t c;

  c  = if_read8()<<0;
  c |= if_read8()<<8;
  c |= if_read8()<<16;

  return c;
}


uint32_t if_read16_little(void)
{
  uint32_t c;

  c  = if_read8()<<0;
  c |= if_read8()<<8;

  return c;
}


uint32_t if_read32_big(void)
{
  uint32_t c;

  c  = if_read8()<<24;
  c |= if_read8()<<16;
  c |= if_read8()<<8;
  c |= if_read8()<<0;

  return c;
}


uint32_t if_read24_big(void)
{
  uint32_t c;

  c  = if_read8()<<16;
  c |= if_read8()<<8;
  c |= if_read8()<<0;

  return c;
}


uint32_t if_read16_big(void)
{
  uint32_t c;

  c  = if_read8()<<8;
  c |= if_read8()<<0;

  return c;
}


uint32_t (*if_read32)(void) = if_read32_big;
uint32_t (*if_read24)(void) = if_read24_big;
uint32_t (*if_read16)(void) = if_read16_big;


__attribute__((always_inline)) __INLINE uint32_t if_read8(void)
{
  uint32_t tail, c;

  tail = rcv_tail;
  while(tail == rcv_head); //wait for data
  c = rcv_buf[tail++];
  tail = tail & (RCV_BUFSIZE-1);
  rcv_tail = tail;

  return c;
}


void if_flush(void)
{
  uint32_t ms;

  if(if_hw & INTERFACE_I2C)
  {
    ms = get_ms();
    while(snd_head != snd_tail)
    {
      if((get_ms()-ms) > 250) //250ms
      {
        DISABLE_IRQ();
        rcv_head = rcv_tail = 0;
        snd_head = snd_tail = 0;
        if_state = 0;
        ENABLE_IRQ();
        break;
      }
    }
  }

  return;
}


__attribute__((always_inline)) __INLINE uint32_t if_available(void)
{
  uint32_t head, tail;

  if(if_hw)
  {
    head = rcv_head;
    tail = rcv_tail;

    if(head > tail)
    {
      return (head-tail);
    }
    else if(head < tail)
    {
      return (RCV_BUFSIZE-(tail-head));
    }
  }

  return 0;
}


void if_setbyteorder(uint32_t byteorder)
{
  //set byte order
  if(byteorder == 0) //big
  {
    if_byteorder = 0;
    if_read16  = if_read16_big;
    if_read24  = if_read24_big;
    if_read32  = if_read32_big;
    if_write16 = if_write16_big;
    if_write24 = if_write24_big;
    if_write32 = if_write32_big;
  }
  else
  {
    if_byteorder = 1;
    if_read16  = if_read16_little;
    if_read24  = if_read24_little;
    if_read32  = if_read32_little;
    if_write16 = if_write16_little;
    if_write24 = if_write24_little;
    if_write32 = if_write32_little;
  }

  return;
}


uint32_t if_getbyteorder(void)
{
  return if_byteorder;
}


uint32_t if_getaddress(void)
{
  return if_addr;
}


uint32_t if_getbaudrate(void)
{
  return if_baud;
}


uint32_t if_getinterface(void)
{
  return if_hw;
}


uint32_t if_init(uint32_t interf)
{
  //disable interface
  if_flush();
  if(if_hw & INTERFACE_I2C) { i2c_off();  }
  if(if_hw & INTERFACE_SPI) { spi_off();  }
  if(if_hw & INTERFACE_UART){ uart_off(); }

  //set byte order
  if_setbyteorder(if_byteorder);

  //reset buffers
  rcv_head = rcv_tail = 0;
  snd_head = snd_tail = 0;
  if_state = 0;

  //reset current interface ?
  if(interf == INTERFACE_RESET)
  {
    if(if_hw != 0)
    {
      interf = if_hw;
    }
    else
    {
      return 0;
    }
  }

  //init interface
  switch(interf)
  {
    default:
    case INTERFACE_I2C:
      if_hw     = INTERFACE_I2C;
      if_write8 = i2c_write;
      i2c_init();
      break;

    case INTERFACE_SPI:
      if_hw     = INTERFACE_SPI;
      if_write8 = spi_write;
      spi_init();
      break;

    case INTERFACE_UART:
      if_hw     = INTERFACE_UART;
      if_write8 = uart_write;
      uart_init();
      break;
  }

  return if_hw;
}

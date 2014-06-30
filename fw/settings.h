#ifndef _SETTINGS_H_
#define _SETTINGS_H_


//----- DEFINES -----
#define VERSION         "0.07"

//LCD support (define only one)
#define LCD_MI0283QT9
//#define LCD_UG12D228AA

#ifdef LCD_MI0283QT9
#define TP_SUPPORT //touch panel support
#endif

#define DEFAULT_CLOCK   48000000UL    //Hz, 12 16* 24 32* 36 48* MHz (*crystal)
#define DEFAULT_POWER   0             //0-100 power for backlight
#define DEFAULT_LDRTIME 500           //check LDR every 500 milliseconds
#define DEFAULT_INTERF  INTERFACE_I2C //INTERFACE_I2C INTERFACE_SPI INTERFACE_UART
#define DEFAULT_BAUD    9600          //UART baud rate
#define DEFAULT_ADDR    0x20          //I2C address
#define DEFAULT_ORDER   0             //byte order (0=big, 1=little)


//----- PINS -----
//GPIO
#define IO_PORT         0
#define IO_PIN          3
//ext. LED
#define LED_PORT        3
#define LED_PIN         0
//PWM for backlight
#define PWM_PORT        1
#define PWM_PIN         9 //CT16B1MAT0
//Rotary Encoder
#define ENC_PORT        1
#define ENC_SW          2
#define ENC_A           1
#define ENC_B           0
//Navigation Switch
#define NAV_PORT        1
#define NAV_SW          2
#define NAV_A           0
#define NAV_B           1
#define NAV_C           4
#define NAV_D           3
//ADC
#define ADC_PORT        1
#define ADC_4           3  //R_PIO1_3 = AD4
#define ADC_5           4  //PIO1_4 = AD5
#define ADC_7           11 //PIO1_11 = AD11
//Interface
#define SS_PORT         0
#define SS_PIN          2
#define I2C_PORT        0
#define SCL_PIN         4
#define SDA_PIN         5
#define SPI_PORT        0
#define MISO_PIN        8
#define MOSI_PIN        9
#define SCK_PIN         10
#define UART_PORT       1
#define RX_PIN          6
#define TX_PIN          7
//Touch-Panel
#define XP_PORT         0
#define XP_PIN          11 //R_PIO0_11 = AD0
#define XP_AD           0
#define XM_PORT         1
#define XM_PIN          1  //R_PIO1_1 = AD2
#define XM_AD           2
#define YP_PORT         1
#define YP_PIN          0  //R_PIO1_0 = AD1
#define YP_AD           1
#define YM_PORT         1
#define YM_PIN          2  //R_PIO1_2 = AD3
#define YM_AD           3


//----- GLOBAL MACROS -----
#define NOP()                          __asm__ volatile ("nop")
#define ENABLE_IRQ()                   __asm__ volatile ("cpsie i")
#define DISABLE_IRQ()                  __asm__ volatile ("cpsid i")

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

#define IOCON_PIO                      (0x00<<0) //pio
#define IOCON_R_PIO                    (0x01<<0) //pio (reserved pins)
#define IOCON_ADC                      (0x01<<0) //adc
#define IOCON_R_ADC                    (0x02<<0) //adc (reserved pins)
#define IOCON_NOPULL                   (0x00<<3) //no pull-down/pull-up
#define IOCON_PULLDOWN                 (0x01<<3) //pull-down
#define IOCON_PULLUP                   (0x02<<3) //pull-up
#define IOCON_ANALOG                   (0x00<<7) //analog (adc pins)
#define IOCON_DIGITAL                  (0x01<<7) //digital (adc pins)
#define IOCON_SETRPIN(port, pin, val)  LPC_IOCON->CONCAT4(R_PIO, port, _, pin) = val
#define IOCON_SETPIN(port, pin, val)   LPC_IOCON->CONCAT4(PIO, port, _, pin) = val

#define ADC_READ(chn, x)             { LPC_ADC->CR |= (1<<chn);              \
                                       LPC_ADC->CR |= (1<<24);               \
                                       while(!(LPC_ADC->DR[chn] & (1<<31))); \
                                       LPC_ADC->CR &= 0xF8FFFF00;            \
                                       x = ((LPC_ADC->DR[chn]>>6) & 0x3FF);  }


#endif //_SETTINGS_H_

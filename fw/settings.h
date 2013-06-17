#ifndef _SETTINGS_H_
#define _SETTINGS_H_


//----- DEFINES -----
#define VERSION         "0.01"

#define DEFAULT_CLOCK   48000000UL    //Hz, 12 16* 24 32* 36 48* MHz (*crystal)
#define DEFAULT_POWER   0             //0-100 power for backlight
#define DEFAULT_INTERF  INTERFACE_I2C //INTERFACE_I2C INTERFACE_SPI INTERFACE_UART
#define DEFAULT_BAUD    9600          //UART baud rate
#define DEFAULT_ADDR    0xA0          //I2C address

//LCD support (define only one)
//#define LCD_MI0283QT9
//#define LCD_UG12D228AA

//#define TP_SUPPORT //touch panel support
//#define ENC_SUPPORT //rotary encoder support
//#define NAV_SUPPORT //navigation switch support


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
#define ADC_1           0
#define ADC_2           1
#define ADC_3           2
#define ADC_4           3
#define ADC_5           4
#define ADC_6           10
#define ADC_7           11
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
#define XP_PIN          11 //PIO0_11 = AD0
#define XP_AD           0
#define XM_PORT         1
#define XM_PIN          1 //PIO1_1 = AD2
#define XM_AD           2
#define YP_PORT         1
#define YP_PIN          0 //PIO1_0 = AD1
#define YP_AD           1
#define YM_PORT         1
#define YM_PIN          2 //PIO1_2 = AD3
#define YM_AD           3


//----- GLOBAL MACROS -----
#define NOP()                          __ASM volatile ("nop")
#define ENABLE_IRQ()                   __ASM volatile ("cpsie i")
#define DISABLE_IRQ()                  __ASM volatile ("cpsid i")


#endif //_SETTINGS_H_

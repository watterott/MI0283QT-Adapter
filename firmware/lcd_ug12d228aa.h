#ifndef _LCD_UG12D228AA_H_
#define _LCD_UG12D228AA_H_

#ifdef LCD_UG12D228AA


//----- DEFINES -----
#define LCD_WIDTH                      128
#define LCD_HEIGHT                     128

#define LCD_CMD_NOP                    0x00
#define LCD_CMD_OSC                    0x02
#define LCD_CMD_DRIVER_OUTPUT          0x10
#define LCD_CMD_DCDC                   0x20
#define LCD_CMD_BIAS                   0x22
#define LCD_CMD_DCDC_CLOCKDIV          0x24
#define LCD_CMD_DCDC_AMP               0x26
#define LCD_CMD_TEMP_COMP              0x28
#define LCD_CMD_CONTRAST_CTRL1         0x2A
#define LCD_CMD_CONTRAST_CTRL2         0x2B
#define LCD_CMD_STANDBY_OFF            0x2C
#define LCD_CMD_STANDBY_ON             0x2D
#define LCD_CMD_ADDR_MODE              0x30
#define LCD_CMD_ROW_VECTOR_MODE        0x32
#define LCD_CMD_LINE_INVERSION         0x34
#define LCD_CMD_FRAME_FREQ_CTRL        0x36
#define LCD_CMD_ENTRY_MODE             0x40
#define LCD_CMD_X_ADDR_AREA            0x42
#define LCD_CMD_Y_ADDR_AREA            0x43
#define LCD_CMD_RAM_SKIP_AREA          0x45
#define LCD_CMD_DISPLAY_OFF            0x50
#define LCD_CMD_DISPLAY_ON             0x51
#define LCD_CMD_SPEC_DISPLAY_PATTERN   0x53
#define LCD_CMD_PARTIAL_DISPLAY_MODE   0x55
#define LCD_CMD_PARTIAL_START_LINE     0x56
#define LCD_CMD_PARTIAL_END_LINE       0x57
#define LCD_CMD_AREA_SCROLL_MODE       0x59
#define LCD_CMD_SCROLL_START_LINE      0x5A
#define LCD_CMD_DATA_FORMAT            0x60
#define LCD_CMD_RGB565                 0x61


//----- PROTOTYPES -----
#define                                lcd_drawstop()
//void                                   lcd_drawstop(void); //empty function
void                                   lcd_draw(uint_least16_t color);
#define                                lcd_drawstart()
//void                                   lcd_drawstart(void); //empty function
void                                   lcd_setarea(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1);
uint_least32_t                         lcd_setbias(uint_least16_t o);
void                                   lcd_invert(uint_least8_t on);
void                                   lcd_power(uint_least8_t on);
void                                   lcd_reset(void);
void                                   lcd_wrdata16(uint_least16_t data);
void                                   lcd_wrdata8(uint_least8_t data);
void                                   lcd_wrcmd8(uint_least8_t cmd);
void                                   lcd_disable(void);
void                                   lcd_enable(void);


#endif //LCD_UG12D228AA

#endif //_LCD_UG12D228AA_H_

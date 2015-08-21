#ifndef _LCD_H_
#define _LCD_H_


//----- DEFINES -----
#define RGB(r,g,b)                     (((r&0xF8)<<8) | ((g&0xFC)<<3) | ((b&0xF8)>>3)) //RGB888 -> RGB565: 5 red | 6 green | 5 blue

#define RGB565toRGB323(c)              (((c&0xE000)>>8) | ((((c&0x07E0)/21)>>2)&0x18) | ((c&0x001C)>>2)) //RGB565 -> RGB323
#define RGB323toRGB565(c)              ((((c&0xE0))<<8) | (((c&0x18)*21)<<2)          | ((c&0x07)<<2))   //RGB323 -> RGB565

#define RGB565toRGB332(c)              (((c&0xE000)>>8) | ((((c&0x07E0)/9)>>3)&0x1C) | ((c&0x001F)/10)) //RGB565 -> RGB332
#define RGB332toRGB565(c)              (((c&0xE0)<<8)   | (((c&0x1C)*9)<<3)          | ((c&0x03)*10))   //RGB332 -> RGB565

#define RGB565toRGB233(c)              (((((c&0xE000)/10)>>5)&0xC0) | ((((c&0x07E0)/9)>>2)&0x38) | ((c&0x001C)>>2)) //RGB565 -> RGB233
#define RGB233toRGB565(c)              ((((c&0xC0)*10)<<5)          | (((c&0x38)*9)<<2)          | ((c&0x07)<<2))   //RGB233 -> RGB565

#define RGB565toGRAY(c)                (((c&0xF800)>>8+1) + ((c&0x07E0)>>3+0) + ((c&0x1F)<<3-2)) //RGB565 to gray scale
#define GRAYtoRGB565(c)                (((c&0xF8)<<8)     | ((c&0xFC)<<3)     | ((c&0xF8)>>3))   //gray scale to RGB565

#define LCD_CENTER                     0x8000


//----- PROTOTYPES -----
uint_least16_t                         lcd_drawtext(uint_least16_t x, uint_least16_t y, char *s, uint_least8_t size, uint_least16_t fgcolor, uint_least16_t bgcolor, uint_least8_t clear_bg);
uint_least16_t                         lcd_drawchar(uint_least16_t x, uint_least16_t y, char c, uint_least8_t size, uint_least16_t fgcolor, uint_least16_t bgcolor, uint_least8_t clear_bg);
void                                   lcd_fillellipse(uint_least16_t x0, uint_least16_t y0, uint_least16_t r_x, uint_least16_t r_y, uint_least16_t color);
void                                   lcd_drawellipse(uint_least16_t x0, uint_least16_t y0, uint_least16_t r_x, uint_least16_t r_y, uint_least16_t color);
void                                   lcd_fillcircle(uint_least16_t x0, uint_least16_t y0, uint_least16_t r, uint_least16_t color);
void                                   lcd_drawcircle(uint_least16_t x0, uint_least16_t y0, uint_least16_t r, uint_least16_t color);
void                                   lcd_fillrndrect(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1, uint_least16_t r, uint_least16_t color);
void                                   lcd_drawrndrect(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1, uint_least16_t r, uint_least16_t color);
void                                   lcd_fillrect(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1, uint_least16_t color);
void                                   lcd_drawrect(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1, uint_least16_t color);
void                                   lcd_drawline(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1, uint_least16_t color);
void                                   lcd_drawpixel(uint_least16_t x0, uint_least16_t y0, uint_least16_t color);
void                                   lcd_clear(uint_least16_t color);
uint_least16_t                         lcd_getorientation(void);
void                                   lcd_setorientation(uint_least16_t o);
uint_least16_t                         lcd_getwidth(void);
uint_least16_t                         lcd_getheight(void);
void                                   lcd_init(void);


#endif //_LCD_H_

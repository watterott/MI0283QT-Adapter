#ifndef _LCD_H_
#define _LCD_H_


//----- DEFINES -----
#define RGB(r,g,b)                     (((r&0xF8)<<8) | ((g&0xFC)<<3) | ((b&0xF8)>>3)) //RGB565: 5 red | 6 green | 5 blue
#define RGB565toRGB323(c)              (((c&0xE000)>>8) | ((c&0600)>>6) | ((c&0x001C)>>2)) //RGB565 -> RGB323
#define RGB565toRGB332(c)              (((c&0xE000)>>8) | ((c&0700)>>6) | ((c&0x0018)>>3)) //RGB565 -> RGB332
#define RGB565toRGB233(c)              (((c&0xC000)>>8) | ((c&0700)>>5) | ((c&0x001C)>>2)) //RGB565 -> RGB233
#define RGB323toRGB565(c)              ((((c&0xE0)* 4)<<6) | (((c&0x18)*21)<<2) | (((c&0x07)* 4)<<0)) //RGB323 -> RGB565
#define RGB332toRGB565(c)              ((((c&0xE0)* 4)<<6) | (((c&0x1C)* 9)<<3) | (((c&0x03)*10)<<0)) //RGB332 -> RGB565
#define RGB233toRGB565(c)              ((((c&0xC0)*10)<<5) | (((c&0x38)* 9)<<2) | (((c&0x07)* 4)<<0)) //RGB233 -> RGB565
#define LCD_CENTER                     0x8000


//----- PROTOTYPES -----
uint32_t                               lcd_drawtext(uint32_t x, uint32_t y, char *s, uint32_t size, uint32_t fgcolor, uint32_t bgcolor, uint32_t clear_bg);
uint32_t                               lcd_drawchar(uint32_t x, uint32_t y, char c, uint32_t size, uint32_t fgcolor, uint32_t bgcolor, uint32_t clear_bg);
void                                   lcd_fillellipse(uint32_t x0, uint32_t y0, uint32_t r_x, uint32_t r_y, uint32_t color);
void                                   lcd_drawellipse(uint32_t x0, uint32_t y0, uint32_t r_x, uint32_t r_y, uint32_t color);
void                                   lcd_fillcircle(uint32_t x0, uint32_t y0, uint32_t r, uint32_t color);
void                                   lcd_drawcircle(uint32_t x0, uint32_t y0, uint32_t r, uint32_t color);
void                                   lcd_fillrndrect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t r, uint32_t color);
void                                   lcd_drawrndrect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t r, uint32_t color);
void                                   lcd_fillrect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);
void                                   lcd_drawrect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);
void                                   lcd_drawline(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);
void                                   lcd_drawpixel(uint32_t x0, uint32_t y0, uint32_t color);
void                                   lcd_clear(uint32_t color);
uint32_t                               lcd_getorientation(void);
void                                   lcd_setorientation(uint32_t o);
uint32_t                               lcd_getwidth(void);
uint32_t                               lcd_getheight(void);
void                                   lcd_init(void);


#endif //_LCD_H_

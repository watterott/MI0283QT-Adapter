#include <stdint.h>
#include <string.h>
#include "cmsis/LPC11xx.h"
#include "settings.h"
#include "lcd.h"
#include "lcd_mi0283qt9.h"
#include "lcd_ug12d228aa.h"
#include "lcd_font8x8.h"


uint_least16_t width, height, fgcolor, bgcolor, orientation;


uint_least16_t lcd_drawtext(uint_least16_t x, uint_least16_t y, char *s, uint_least8_t size, uint_least16_t fgcolor, uint_least16_t bgcolor, uint_least8_t clear_bg)
{
  size = (size==0)?1:size;

  if(x == LCD_CENTER) //x center
  {
    x = (width/2)-((FONT_WIDTH*size*strlen(s))/2);
    if(x >= width){ x = 0; }
  }
  if(y == LCD_CENTER) //y center
  {
    y = (height/2)-((FONT_HEIGHT*size)/2);
    if(y >= height){ y = 0; }
  }

  while(*s != 0)
  {
    x = lcd_drawchar(x, y, *s++, size, fgcolor, bgcolor, clear_bg);
    if(x > width)
    {
      break;
    }
  }

  return x;
}


uint_least16_t lcd_drawchar(uint_least16_t x, uint_least16_t y, char c, uint_least8_t size, uint_least16_t fgcolor, uint_least16_t bgcolor, uint_least8_t clear_bg)
{
  uint_least16_t ret, i, x0, y0, x1, y1, h;
  uint_least8_t data, mask;
  uint8_t *ptr;

  //size = (size==0)?1:size;

  x1 = x+(FONT_WIDTH*size);
  y1 = y+(FONT_HEIGHT*size);

  if((x1-1) >= width)
  {
    return width+1;
  }
  else if((y1-1) >= height)
  {
    return width+1;
  }
  else if(c < FONT_START)
  {
    return x;
  }
  size--;
  ret = x1;

  lcd_enable();

  ptr = (uint8_t*)&fontdata[(c-FONT_START)*(8*FONT_HEIGHT/8)];
  if(size == 0)
  {
    y0 = y;
    for(h=FONT_HEIGHT; h!=0; h--)
    {
      data = *ptr++;
      x0 = x;
      for(mask=(1<<(FONT_WIDTH-1)); mask!=0; mask>>=1)
      {
        if(data & mask)
        {
          lcd_setarea(x0, y0, x0, y0);
          lcd_drawstart();
          lcd_draw(fgcolor);
          lcd_drawstop();
        }
        else if(clear_bg)
        {
          lcd_setarea(x0, y0, x0, y0);
          lcd_drawstart();
          lcd_draw(bgcolor);
          lcd_drawstop();
        }
        x0++;
      }
      y0++;
    }
  }
  else
  {
    y1 = y;
    for(h=FONT_HEIGHT; h!=0; h--)
    {
      data = *ptr++;
      x0 = x;
      x1 = x0+size;
      y0 = y1;
      y1 = y0+size;
      for(mask=(1<<(FONT_WIDTH-1)); mask!=0; mask>>=1)
      {
        if(data & mask)
        {
          lcd_setarea(x0, y0, x1, y1);
          lcd_drawstart();
          for(i=((1+(x1-x0))*(1+(y1-y0))); i!=0; i--)
          {
            lcd_draw(fgcolor);
          }
          lcd_drawstop();
        }
        else if(clear_bg)
        {
          lcd_setarea(x0, y0, x1, y1);
          lcd_drawstart();
          for(i=((1+(x1-x0))*(1+(y1-y0))); i!=0; i--)
          {
            lcd_draw(bgcolor);
          }
          lcd_drawstop();
        }
        x0 = x1+1;
        x1 = x0+size;
      }
      y1++;
    }
  }

  lcd_disable();

  return ret;
}


void lcd_fillellipse(uint_least16_t x0, uint_least16_t y0, uint_least16_t r_x, uint_least16_t r_y, uint_least16_t color)
{
  int_least16_t x, y;
  int_least32_t e, e2, dx, dy, rx, ry;

  if(((x0-r_x) >= width) ||
     ((y0-r_y) >= height))
  {
    return;
  }

  x  = -r_x;
  y  = 0;
  dx = (1+(2*x))*r_y*r_y;
  dy = x*x;
  e  = dx+dy;
  rx = 2*r_x*r_x;
  ry = 2*r_y*r_y;

  while(x <= 0)
  {
    lcd_fillrect(x0 - x, y0 + y, x0 + x, y0 + y, color);
    lcd_fillrect(x0 - x, y0 - y, x0 + x, y0 - y, color);

    e2 = 2*e;
    if(e2 >= dx) { x++; e += dx += ry; }
    if(e2 <= dy) { y++; e += dy += rx; }
  }

  while(y++ < r_y)
  {
    lcd_drawpixel(x0, y0 + y, color);
    lcd_drawpixel(x0, y0 - y, color);
  }

  return;
}


void lcd_drawellipse(uint_least16_t x0, uint_least16_t y0, uint_least16_t r_x, uint_least16_t r_y, uint_least16_t color)
{
  int_least16_t x, y;
  int_least32_t e, e2, dx, dy, rx, ry;

  x  = -r_x;
  y  = 0;
  dx = (1+(2*x))*r_y*r_y;
  dy = x*x;
  e  = dx+dy;
  rx = 2*r_x*r_x;
  ry = 2*r_y*r_y;

  while(x <= 0)
  {
    lcd_drawpixel(x0 - x, y0 + y, color);
    lcd_drawpixel(x0 + x, y0 + y, color);
    lcd_drawpixel(x0 + x, y0 - y, color);
    lcd_drawpixel(x0 - x, y0 - y, color);

    e2 = 2*e;
    if(e2 >= dx) { x++; e += dx += ry; }
    if(e2 <= dy) { y++; e += dy += rx; }
  }

  while(y++ < r_y)
  {
    lcd_drawpixel(x0, y0 + y, color);
    lcd_drawpixel(x0, y0 - y, color);
  }

  return;
}


void lcd_fillcircle(uint_least16_t x0, uint_least16_t y0, uint_least16_t r, uint_least16_t color)
{
  int_least16_t err, x, y;

  err = -r;
  x   = r;
  y   = 0;
  while(x >= y)
  {
    lcd_fillrect(x0 - x, y0 + y, x0 + x, y0 + y, color);
    lcd_fillrect(x0 - x, y0 - y, x0 + x, y0 - y, color);
    lcd_fillrect(x0 - y, y0 + x, x0 + y, y0 + x, color);
    lcd_fillrect(x0 - y, y0 - x, x0 + y, y0 - x, color);

    err += y++;
    err += y;
    if(err >= 0)
    {
      x--;
      err = err - x - x;
    }
  }

  return;
}


void lcd_drawcircle(uint_least16_t x0, uint_least16_t y0, uint_least16_t r, uint_least16_t color)
{
  int_least16_t err, x, y;

  err = -r;
  x   = r;
  y   = 0;

  while(x >= y)
  {
    lcd_drawpixel(x0 - x, y0 - y, color);
    lcd_drawpixel(x0 - y, y0 - x, color);
    lcd_drawpixel(x0 + x, y0 - y, color);
    lcd_drawpixel(x0 + y, y0 - x, color);
    lcd_drawpixel(x0 + x, y0 + y, color);
    lcd_drawpixel(x0 + y, y0 + x, color);
    lcd_drawpixel(x0 - x, y0 + y, color);
    lcd_drawpixel(x0 - y, y0 + x, color);

    err += y++;
    err += y;
    if(err >= 0)
    {
      x--;
      err = err - x - x;
    }
  }

  return;
}


void lcd_fillrndrect(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1, uint_least16_t r, uint_least16_t color)
{
  int_least16_t err, x, y;

  if(((x0+r) >= (x1-r)) || ((y0+r) >= (y1-r)))
  {
    return;
  }

  lcd_fillrect(x0+r, y0,   x1-r, y1,   color);
  lcd_fillrect(x0,   y0+r, x0+r, y1-r, color);
  lcd_fillrect(x1-r, y0+r, x1,   y1-r, color);

  //calculate corner positions
  y1 = y1-r;
  x1 = x1-r;
  x0 = x0+r;
  y0 = y0+r;

  //fill round corners
  err = -r;
  x   = r;
  y   = 0;
  while(x >= y)
  {
    lcd_fillrect(x0 - x, y0 - y, x0, y0 - y, color);
    lcd_fillrect(x0 - y, y0 - x, x0, y0 - x, color);
    lcd_fillrect(x1 + x, y0 - y, x1, y0 - y, color);
    lcd_fillrect(x1 + y, y0 - x, x1, y0 - x, color);
    lcd_fillrect(x1 + x, y1 + y, x1, y1 + y, color);
    lcd_fillrect(x1 + y, y1 + x, x1, y1 + x, color);
    lcd_fillrect(x0 - x, y1 + y, x0, y1 + y, color);
    lcd_fillrect(x0 - y, y1 + x, x0, y1 + x, color);

    err += y++;
    err += y;
    if(err >= 0)
    {
      x--;
      err = err - x - x;
    }
  }

  return;
}


void lcd_drawrndrect(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1, uint_least16_t r, uint_least16_t color)
{
  int_least16_t err, x, y;

  if(((x0+r) >= (x1-r)) || ((y0+r) >= (y1-r)))
  {
    return;
  }

  //draw horizontal + vertical lines
  lcd_fillrect(x0,   y0+r, x0,   y1-r, color);
  lcd_fillrect(x0+r, y1,   x1-r, y1,   color);
  lcd_fillrect(x1,   y0+r, x1,   y1-r, color);
  lcd_fillrect(x0+r, y0,   x1-r, y0,   color);

  //calculate corner positions
  y1 = y1-r;
  x1 = x1-r;
  x0 = x0+r;
  y0 = y0+r;

  //draw round corners
  err = -r;
  x   = r;
  y   = 0;
  while(x >= y)
  {
    lcd_drawpixel(x0 - x, y0 - y, color);
    lcd_drawpixel(x0 - y, y0 - x, color);
    lcd_drawpixel(x1 + x, y0 - y, color);
    lcd_drawpixel(x1 + y, y0 - x, color);
    lcd_drawpixel(x1 + x, y1 + y, color);
    lcd_drawpixel(x1 + y, y1 + x, color);
    lcd_drawpixel(x0 - x, y1 + y, color);
    lcd_drawpixel(x0 - y, y1 + x, color);

    err += y++;
    err += y;
    if(err >= 0)
    {
      x--;
      err = err - x - x;
    }
  }

  return;
}


void lcd_fillrect(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1, uint_least16_t color)
{
  uint_least32_t size;

  if(x0 >= width)  { x0 = width-1;  }
  if(y0 >= height) { y0 = height-1; }
  if(x1 >= width)  { x1 = width-1;  }
  if(y1 >= height) { y1 = height-1; }

  if(x0 > x1) //swap points
  {
    x0 ^= x1;
    x1 ^= x0;
    x0 ^= x1;
  }
  if(y0 > y1) //swap points
  {
    y0 ^= y1;
    y1 ^= y0;
    y0 ^= y1;
  }

  lcd_enable();

  lcd_setarea(x0, y0, x1, y1);

  lcd_drawstart();
  for(size=((1+(x1-x0))*(1+(y1-y0))); size!=0; size--)
  {
    lcd_draw(color);
  }
  lcd_drawstop();

  lcd_disable();

  return;
}


void lcd_drawrect(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1, uint_least16_t color)
{
  lcd_fillrect(x0, y0, x0, y1, color);
  lcd_fillrect(x0, y1, x1, y1, color);
  lcd_fillrect(x1, y0, x1, y1, color);
  lcd_fillrect(x0, y0, x1, y0, color);

  return;
}


void lcd_drawline(uint_least16_t x0, uint_least16_t y0, uint_least16_t x1, uint_least16_t y1, uint_least16_t color)
{
  int_least16_t dx, dy, dx2, dy2, err, stepx, stepy;

  if((x0 == x1) || //horizontal line
     (y0 == y1))   //vertical line
  {
    lcd_fillrect(x0, y0, x1, y1, color);
  }
  else
  {
    //check parameter
    if(x0 >= width)  { x0 = width-1;  }
    if(y0 >= height) { y0 = height-1; }
    if(x1 >= width)  { x1 = width-1;  }
    if(y1 >= height) { y1 = height-1; }
    //calculate direction
    dx = x1-x0;
    dy = y1-y0;
    if(dx < 0) { dx = -dx; stepx = -1; } else { stepx = +1; }
    if(dy < 0) { dy = -dy; stepy = -1; } else { stepy = +1; }
    dx2 = dx*2;
    dy2 = dy*2;
    //draw line
    lcd_enable();
    lcd_setarea(x0, y0, x0, y0);
    lcd_drawstart();
    lcd_draw(color);
    lcd_drawstop();
    if(dx > dy)
    {
      err = dy2 - dx;
      while(x0 != x1)
      {
        if(err >= 0)
        {
          y0  += stepy;
          err -= dx2;
        }
        x0  += stepx;
        err += dy2;
        lcd_setarea(x0, y0, x0, y0);
        lcd_drawstart();
        lcd_draw(color);
        lcd_drawstop();
      }
    }
    else
    {
      err = dx2 - dy;
      while(y0 != y1)
      {
        if(err >= 0)
        {
          x0  += stepx;
          err -= dy2;
        }
        y0  += stepy;
        err += dx2;
        lcd_setarea(x0, y0, x0, y0);
        lcd_drawstart();
        lcd_draw(color);
        lcd_drawstop();
      }
    }
    lcd_disable();
  }

  return;
}


__attribute__((always_inline)) __INLINE void lcd_drawpixel(uint_least16_t x0, uint_least16_t y0, uint_least16_t color)
{
  if((x0 >= width) ||
     (y0 >= height))
  {
    return;
  }

  lcd_enable();

  lcd_setarea(x0, y0, x0, y0);

  lcd_drawstart();
  lcd_draw(color);
  lcd_drawstop();

  lcd_disable();

  return;
}


void lcd_clear(uint_least16_t color)
{
  uint_least16_t size;

  lcd_enable();

  lcd_setarea(0, 0, width-1, height-1);

  lcd_drawstart();
  for(size=(LCD_WIDTH*LCD_HEIGHT/8UL); size!=0; size--)
  {
    lcd_draw(color); //1
    lcd_draw(color); //2
    lcd_draw(color); //3
    lcd_draw(color); //4
    lcd_draw(color); //5
    lcd_draw(color); //6
    lcd_draw(color); //7
    lcd_draw(color); //8
  }
  lcd_drawstop();

  lcd_disable();

  return;
}


uint_least16_t lcd_getorientation(void)
{
  return orientation;
}


void lcd_setorientation(uint_least16_t o)
{
  uint32_t wh;

  switch(o)
  {
    default:
    case 0:
    //case 36:
    //case 360:
      orientation = 0;
      break;
    case 9:
    case 90:
      orientation = 90;
      break;
    case 18:
    case 180:
      orientation = 180;
      break;
    case 27:
    case 270:
    case 14: //270&0xFF
      orientation = 270;
      break;
  }

  wh = lcd_setbias(orientation);
  height = (wh>> 0)&0xFFFF;
  width  = (wh>>16)&0xFFFF;

  return;
}


uint_least16_t lcd_getwidth(void)
{
  return width;
}


uint_least16_t lcd_getheight(void)
{
  return height;
}


void lcd_init(void)
{
  width       = 0;
  height      = 0;
  orientation = 0;

  //reset display
  lcd_reset();

  //init vars
  lcd_setorientation(0);

  return;
}

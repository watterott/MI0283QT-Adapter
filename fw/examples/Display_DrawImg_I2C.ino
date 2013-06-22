/*
  Example sketch for I2C communication (400kHz).
*/

#include <Wire.h>
#include "cmd.h"

#define RGB(r,g,b) (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //16bit: 5 red | 6 green | 5 blue

#define I2C_ADDR 0xA0


void wait_for_input(void)
{
  Serial.println("Wait for input!");
  while(Serial.available() == 0);
  while(Serial.available() != 0){ Serial.read(); };
}


void write_pos(uint16_t i)
{
  Wire.write(i>>8); //for screens > 255 pixel, otherwise remove
  Wire.write(i);
}


void setup()
{
  uint16_t i=0, x=0, y=0, w=0, h=0;

  //init Serial lib
  Serial.begin(9600);
  while(!Serial); //wait for serial port to connect - needed for Leonardo only
  wait_for_input();
  Serial.println("go...");

  //init Wire lib
  Wire.begin();

  //backlight
  Serial.println("backlight...");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_LED);
  Wire.write(50); //0...100
  Wire.endTransmission();

  //clear screen
  Serial.println("clear...");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_CLEARBG);
  Wire.endTransmission();

  //draw image
  Serial.println("image...");
  x=5; y=5; w=40; h=40;
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_DRAWIMAGE); //x0, y0, w, h, color mode, pixel...
  write_pos(x);
  write_pos(y);
  write_pos(w);
  write_pos(h);
  Wire.write(COLOR_RGB323);
  Wire.endTransmission();
  for(i=w*h; i!=0; i--)
  {
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0x55);
    Wire.endTransmission();
  }

  //draw image (use RLE with magic number 0xAA)
  Serial.println("image (RLE)...");
  x=50; y=50; w=20; h=20;
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_DRAWIMAGE); //x0, y0, w, h, color mode, pixel...
  write_pos(x);
  write_pos(y);
  write_pos(w);
  write_pos(h);
  Wire.write(COLOR_RGB323|COLOR_RLE);
  Wire.endTransmission();
  for(i=w*h; i!=0; i--)
  {
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0xAA);
    Wire.write(1);
    Wire.write(0x55);
    Wire.endTransmission();
  }

  Serial.println("...end");
}


void loop()
{
}

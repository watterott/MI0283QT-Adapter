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
  uint16_t x=0, y=0, w=0, h=0;

  //init Serial lib
  Serial.begin(9600);
  while(!Serial); //wait for serial port to connect - needed for Leonardo only
  wait_for_input();
  Serial.println("go...");

  //init Wire lib
  Wire.begin();

  //get firmware version
  Serial.print("version: ");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_VERSION);
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, 4); //request 4 bytes
  while(Wire.available())
  { 
    char c = Wire.read();
    Serial.print(c);
  }
  Serial.println("");

  //get features
  Serial.print("features: ");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_FEATURES);
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, 1); //request 1 bytes
  while(Wire.available())
  { 
    char c = Wire.read();
    if(c & FEATURE_LCD){ Serial.print("LCD "); }
    if(c & FEATURE_TP) { Serial.print("TP ");  }
    if(c & FEATURE_ENC){ Serial.print("ENC "); }
    if(c & FEATURE_NAV){ Serial.print("NAV "); }
  }
  Serial.println("");

  //get width and height
  Serial.print("width/height: ");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_WIDTH);
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, 2); //request 2 bytes
  if(Wire.available())
  { 
    x  = Wire.read()<<8;
    x |= Wire.read()<<0;
  }
  Serial.print(x, DEC);
  Serial.print("/");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_HEIGHT);
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, 2); //request 2 bytes
  if(Wire.available())
  { 
    y  = Wire.read()<<8;
    y |= Wire.read()<<0;
  }
  Serial.print(y, DEC);
  Serial.println("");

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

  //draw rect
  Serial.println("rect...");
  x=30; y=30; w=20; h=40;
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_DRAWRECTFG); //x0, y0, w, h
  write_pos(x);
  write_pos(y);
  write_pos(w);
  write_pos(h);
  Wire.endTransmission();

  //draw text
  Serial.println("text...");
  x=10; y=10;
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_DRAWTEXTFG); //x0, y0, size, clear, length, text
  write_pos(x);
  write_pos(y);
  Wire.write(1); //size + clear
  Wire.write(strlen("123")); //length
  Wire.write('1');
  Wire.write('2');
  Wire.write('3');
  Wire.endTransmission();

  Serial.println("...end");
}


void loop()
{
}

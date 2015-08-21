/*
  Temperature Demo (I2C communication).

  Note: To compile the program you have to add the file cmd.h in the Arduino IDE:
        Sketch -> Add file...

        and the OneWire Library from  Paul Stoffregen:
        http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip
        http://www.pjrc.com/teensy/td_libs_OneWire.html
*/

#include <Wire.h>
#include <OneWire.h>
#include "cmd.h"

#define RGB(r,g,b) (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //16bit: 5 red | 6 green | 5 blue

#define I2C_ADDR 0x20

OneWire ds(8);  //DS18B20 on pin 8 (with 4k7 to VCC necessary)
uint8_t ds_addr[8];

uint8_t postion_16bit=0, features=0;


void write_pos(uint16_t i)
{
  if(postion_16bit != 0)
  {
    Wire.write(i>>8);
  }
  Wire.write(i);
}


void write_text(uint16_t x, uint16_t y, uint8_t size, char *text)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_DRAWSTRINGFG); //x0, y0, size, clear, text
  write_pos(x);
  write_pos(y);
  Wire.write(size); //size + clear
  Wire.write(text);
  Wire.write(0x00); //end of string
  Wire.endTransmission();
}


void setup()
{
  uint16_t x=0, y=0, w=0, h=0;

  //init Serial lib
  Serial.begin(9600);
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
    features = Wire.read();
    if(features & FEATURE_LCD){ Serial.print("LCD "); }
    if(features & FEATURE_TP) { Serial.print("TP ");  }
    if(features & FEATURE_ENC){ Serial.print("ENC "); }
    if(features & FEATURE_NAV){ Serial.print("NAV "); }
    if(features & FEATURE_LDR){ Serial.print("LDR "); }
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
  if((x > 255) || (y > 255))
  {
    postion_16bit = 1;
  }

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

  //init OneWire lib
  while(ds.search(ds_addr) == 0)
  {
    Serial.println("No DS18B20!");
    write_text(10, 10, 1, "No DS18B20!");
    ds.reset_search();
    delay(1000);
  }
}


void loop()
{
  uint8_t i, data[12];
  float celsius, fahrenheit;
  char text[8];

  //DS18B20 code: http://www.pjrc.com/teensy/td_libs_OneWire.html

  ds.reset();
  ds.select(ds_addr);
  ds.write(0x44, 1); //start conversion
  delay(1000); //wait 1 second
  ds.reset();
  ds.select(ds_addr);
  ds.write(0xBE); //read
  for(i=0; i < 9; i++)
  {
    data[i] = ds.read();
  }

  //Convert the data to actual temperature
  //because the result is a 16 bit signed integer, it should
  //be stored to an "int16_t" type, which is always 16 bits
  //even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  uint8_t cfg = (data[4] & 0x60);
  //at lower res, the low bits are undefined, so let's zero them
       if (cfg == 0x00) raw = raw & ~7;  //9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3; //10 bit res, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1; //11 bit res, 375 ms
  //default is 12 bit resolution, 750 ms conversion time

  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;

  Serial.print("T = ");
  Serial.print(celsius);
  Serial.print(" C, ");
  Serial.print(fahrenheit);
  Serial.println(" F");

  dtostrf(celsius, 2, 1, text);
  write_text(20, 15, 1, "Temperature");
  write_text(18, 40, 3, text);
  write_text(32, 80, 1, "Celsius");
}

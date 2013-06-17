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


void setup()
{
  uint16_t x, y, w, h;

  //init Serial lib
  Serial.begin(9600);
  while(!Serial); //wait for serial port to connect - needed for Leonardo only
  wait_for_input();
  Serial.println("go...");

  //init Wire lib
  Wire.begin();

  //get display firmware version
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

  //backlight
  Serial.println("backlight...");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_LED);
  Wire.write(50); //0...100
  Wire.endTransmission();

  //start test program
  Serial.println("test...");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_TEST);
  Wire.endTransmission();
  wait_for_input();
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x00); //stop test
  Wire.endTransmission();
  Wire.flush();

  delay(500); //wait 500ms (for exit test program)

  //clear screen
  Serial.println("clear...");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_CLEARBG);
  Wire.endTransmission();

  wait_for_input();
  Serial.println("Move Rotary Encoder!");
}


void loop()
{
  int8_t p, s;

  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_ENC_POS);
  Wire.endTransmission();

  if(Wire.requestFrom(I2C_ADDR, 2) == 2)
  { 
    p = Wire.read();
    s = Wire.read();
    
    Serial.print("ENC: ");
    Serial.print(p); //step: +/-127
    Serial.print(", ");
    Serial.print(s); //switch: 1=press, 3=long press
    Serial.println("");
  }

  delay(1000);
}

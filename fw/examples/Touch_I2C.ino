/*
  Example sketch for I2C communication (400kHz).

  Note: To compile the program you have to add the file cmd.h in the Arduino IDE:
        Sketch -> Add file...
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

  //backlight
  Serial.println("backlight...");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_LED);
  Wire.write(50); //0...100
  Wire.endTransmission();

  //touchscreen calibration
  Serial.println("calibration...");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_TP_CALIBRATE);
  Wire.endTransmission();
  wait_for_input();
  Wire.flush();

  delay(500); //wait 500ms (for exit calibration)

  //start test program
  Serial.println("test...");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_TEST);
  Wire.endTransmission();
  wait_for_input();
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x00); //stop test
  Wire.endTransmission();
  delay(1);
  Wire.flush();

  delay(500); //wait 500ms (for exit test program)

  //clear screen
  Serial.println("clear...");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_LCD_CLEARBG);
  Wire.endTransmission();

  wait_for_input();

  //enable touchpanel
  Serial.println("touch on...");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_CTRL);
  Wire.write(CMD_CTRL_FEATURES);
  Wire.write(FEATURE_TP);
  Wire.endTransmission();

  Serial.println("Press Touchpanel!");
}


void loop()
{
  uint16_t x, y, z;
  static uint16_t last_x=0, last_y=0;

  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_TP_POS);
  Wire.endTransmission();

  if(Wire.requestFrom(I2C_ADDR, 6) == 6)
  { 
    x  = Wire.read()<<8;
    x |= Wire.read()<<0;
    y  = Wire.read()<<8;
    y |= Wire.read()<<0;
    z  = Wire.read()<<8;
    z |= Wire.read()<<0;
    
    if(z)
    {
      Serial.print("TP: ");
      Serial.print(x);
      Serial.print(", ");
      Serial.print(y);
      Serial.print(", ");
      Serial.print(z);
      Serial.println("");

      if((x != last_x) || (y != last_y))
      {
        last_x = x;
        last_y = y;
        Wire.beginTransmission(I2C_ADDR);
        Wire.write(CMD_LCD_FILLCIRCLEFG); //x0, y0, radius
        z = 4; //radius
        Wire.write(x>>8);
        Wire.write(x);
        Wire.write(y>>8);
        Wire.write(y);
        Wire.write(z>>8);
        Wire.write(z);
        Wire.endTransmission();
      }
    }
  }
  
  delay(20);
}

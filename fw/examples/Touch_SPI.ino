/*
  Example sketch for SPI communication (max 4MHz).

  Note: To compile the program you have to add the file cmd.h in the Arduino IDE:
        Sketch -> Add file...
*/

#include <SPI.h>
#include <Wire.h>
#include "cmd.h"

#define RGB(r,g,b) (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //16bit: 5 red | 6 green | 5 blue

#define I2C_ADDR 0xA0

int rst_pin  = 8;  //connect to RST from display
int cs_pin   = 7;  //connect to CS from display
int mosi_pin = 11; //Uno 11
int miso_pin = 12; //Uno 12
int sck_pin  = 13; //Uno 13


void wait_for_input(void)
{
  Serial.println("Wait for input!");
  while(Serial.available() == 0);
  while(Serial.available() != 0){ Serial.read(); };
}


void setup()
{
  //init pins
  pinMode(rst_pin, OUTPUT);
  digitalWrite(rst_pin, HIGH);
  pinMode(cs_pin, OUTPUT);
  digitalWrite(cs_pin, HIGH);

  //init Serial lib
  Serial.begin(9600);
  while(!Serial); //wait for serial port to connect - needed for Leonardo only
  wait_for_input();
  Serial.println("go...");

  //reset display to SPI mode
  // via hardware reset
  pinMode(mosi_pin, OUTPUT);
  digitalWrite(rst_pin, LOW);
  delay(10);
  digitalWrite(rst_pin, HIGH);
  digitalWrite(cs_pin, LOW);
  digitalWrite(mosi_pin, LOW); //cs+mosi low -> SPI
  delay(800);
  digitalWrite(cs_pin, HIGH);
  // via default interface (I2C)
  Wire.begin();
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_CTRL);
  Wire.write(CMD_CTRL_INTERFACE);
  Wire.write(INTERFACE_SPI); //INTERFACE_UART | INTERFACE_I2C | INTERFACE_SPI
  Wire.endTransmission();
  
  //init SPI lib
  SPI.setClockDivider(SPI_CLOCK_DIV8); //SPI_CLOCK_DIV4 SPI_CLOCK_DIV8 SPI_CLOCK_DIV16
  SPI.setDataMode(SPI_MODE3); //SPI_MODE3
  SPI.begin(); //sets SS/CS high

  //get firmware version
  Serial.print("version: ");
  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_VERSION);
  delay(1);
  Serial.print((char)SPI.transfer(0xFF));
  Serial.print((char)SPI.transfer(0xFF));
  Serial.print((char)SPI.transfer(0xFF));
  Serial.print((char)SPI.transfer(0xFF));
  Serial.println("");
  digitalWrite(cs_pin, HIGH);

  //backlight
  Serial.println("backlight...");
  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_LCD_LED);
  SPI.transfer(50); //0...100
  digitalWrite(cs_pin, HIGH);

  //touchscreen calibration
  Serial.println("calibration...");
  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_TP_CALIBRATE);
  wait_for_input();
  SPI.transfer(0xFF); //read response
  digitalWrite(cs_pin, HIGH);
  //SPI.flush();

  delay(500); //wait 500ms (for exit calibration)

  //start test program
  Serial.println("test...");
  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_TEST);
  digitalWrite(cs_pin, HIGH);
  wait_for_input();
  digitalWrite(cs_pin, LOW);
  SPI.transfer(0x00); //stop test
  delay(1);
  SPI.transfer(0xFF); //read response
  digitalWrite(cs_pin, HIGH);
  //SPI.flush();

  delay(500); //wait 500ms (for exit test program)

  //clear screen
  Serial.println("clear...");
  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_LCD_CLEARBG);
  digitalWrite(cs_pin, HIGH);

  wait_for_input();

  //enable touchpanel
  Serial.println("touch on...");
  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_CTRL);
  SPI.transfer(CMD_CTRL_FEATURES);
  SPI.transfer(FEATURE_TP);
  digitalWrite(cs_pin, HIGH);

  Serial.println("Press Touchpanel!");
}


void loop()
{
  uint16_t x, y, z;
  static uint16_t last_x=0, last_y=0;
  
  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_TP_POS);
  delay(1);
  x  = SPI.transfer(0xFF)<<8;
  x |= SPI.transfer(0xFF)<<0;
  y  = SPI.transfer(0xFF)<<8;
  y |= SPI.transfer(0xFF)<<0;
  z  = SPI.transfer(0xFF)<<8;
  z |= SPI.transfer(0xFF)<<0;
  digitalWrite(cs_pin, HIGH);
  
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
      digitalWrite(cs_pin, LOW);
      SPI.transfer(CMD_LCD_FILLCIRCLEFG); //x0, y0, radius
      z = 4; //radius
      SPI.transfer(x>>8);
      SPI.transfer(x);
      SPI.transfer(y>>8);
      SPI.transfer(y);
      SPI.transfer(z>>8);
      SPI.transfer(z);
      digitalWrite(cs_pin, HIGH);
    }
  }

  delay(20);
}

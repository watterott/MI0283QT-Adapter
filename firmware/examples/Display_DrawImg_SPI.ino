/*
  Example sketch for SPI communication (max 4MHz).

  Note: To compile the program you have to add the file cmd.h in the Arduino IDE:
        Sketch -> Add file...
        Check function write_pos() if the screen is smaller than 255 pixel.
*/

#include <SPI.h>
#include <Wire.h>
#include "cmd.h"

#define RGB(r,g,b) (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //16bit: 5 red | 6 green | 5 blue

#define I2C_ADDR 0x20

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


void write_pos(uint16_t i)
{
  SPI.transfer(i>>8); //only for screens > 255 pixel, otherwise remove
  SPI.transfer(i);
}


void setup()
{
  uint16_t i=0, x=0, y=0, w=0, h=0;

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

  //backlight
  Serial.println("backlight...");
  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_LCD_LED);
  SPI.transfer(50); //0...100
  digitalWrite(cs_pin, HIGH);

  //clear screen
  Serial.println("clear...");
  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_LCD_CLEARBG);
  digitalWrite(cs_pin, HIGH);

  //draw image
  Serial.println("image...");
  x=5; y=5; w=40; h=40;
  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_LCD_DRAWIMAGE); //x0, y0, w, h, color mode, pixel...
  write_pos(x);
  write_pos(y);
  write_pos(w);
  write_pos(h);
  SPI.transfer(COLOR_RGB323);
  for(i=w*h; i!=0; i--)
  {
    SPI.transfer(0x55);
  }
  digitalWrite(cs_pin, HIGH);

  //draw image (use RLE with magic number 0xAA)
  Serial.println("image (RLE)...");
  x=50; y=50; w=20; h=20;
  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_LCD_DRAWIMAGE); //x0, y0, w, h, color mode, pixel...
  write_pos(x);
  write_pos(y);
  write_pos(w);
  write_pos(h);
  SPI.transfer(COLOR_RGB323|COLOR_RLE);
  for(i=w*h; i!=0; i--)
  {
    SPI.transfer(0xAA);
    SPI.transfer(1);
    SPI.transfer(0x55);
  }
  digitalWrite(cs_pin, HIGH);

  Serial.println("...end");
}


void loop()
{
}

/*
  Example sketch for SPI communication (max 4MHz).
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
  uint16_t x, y, w, h;

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

  //get display version
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

  //enable rotary encoder
  Serial.println("encoder on...");
  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_CTRL);
  SPI.transfer(CMD_CTRL_FEATURES);
  SPI.transfer(FEATURE_ENC);
  digitalWrite(cs_pin, HIGH);

  Serial.println("Move Rotary Encoder!");
}


void loop()
{
  int8_t p, s;

  digitalWrite(cs_pin, LOW);
  SPI.transfer(CMD_ENC_POS);
  delay(1);
  p = SPI.transfer(0xFF);
  s = SPI.transfer(0xFF);
  digitalWrite(cs_pin, HIGH);

  Serial.print("ENC: ");
  Serial.print(p); //step: +/-127
  Serial.print(", ");
  Serial.print(s); //switch: 1=press, 3=long press
  Serial.println("");

  delay(1000);
}

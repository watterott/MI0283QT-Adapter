/*
  Example sketch for UART communication (default 9600 baud).
*/

#include <SoftwareSerial.h>
#include <Wire.h>
#include "cmd.h"

#define RGB(r,g,b) (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //16bit: 5 red | 6 green | 5 blue

#define I2C_ADDR 0xA0

/*
  Note:
  Not all pins on the Mega and Mega 2560 support change interrupts, 
  so only the following can be used for RX: 
  10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69
 
  Not all pins on the Leonardo support change interrupts, 
  so only the following can be used for RX: 
  8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
*/

int rst_pin = 8; //connect to RST from display
int cs_pin  = 7; //connect to CS from display
int rx_pin  = 6; //connect to Tx from display
int tx_pin  = 5; //connect to Rx from display

SoftwareSerial mySerial(rx_pin, tx_pin); //Rx, Tx


void wait_for_input(void)
{
  Serial.println("Wait for input!");
  while(Serial.available() == 0);
  while(Serial.available() != 0){ Serial.read(); };
}


void write_pos(uint16_t i)
{
  mySerial.write(i>>8); //for screens > 255 pixel, otherwise remove
  mySerial.write(i);
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

  //reset display to UART mode
  // via hardware reset
  pinMode(tx_pin, OUTPUT);
  digitalWrite(rst_pin, LOW);
  delay(10);
  digitalWrite(rst_pin, HIGH);
  digitalWrite(cs_pin, LOW);
  digitalWrite(tx_pin, LOW); //cs+tx low -> UART
  delay(800);
  digitalWrite(cs_pin, HIGH);
  // via default interface (I2C)
  Wire.begin();
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_CTRL);
  Wire.write(CMD_CTRL_INTERFACE);
  Wire.write(INTERFACE_UART); //INTERFACE_UART | INTERFACE_I2C | INTERFACE_SPI
  Wire.endTransmission();

  //init SoftwareSerial lib
  pinMode(tx_pin, OUTPUT);
  mySerial.begin(9600);
  digitalWrite(cs_pin, LOW);

  //get firmware version
  Serial.print("version: ");
  mySerial.write(CMD_VERSION);
  while(mySerial.available() < 4); //wait for data
  while(mySerial.available())
  { 
    char c = mySerial.read();
    Serial.print(c);
  }
  Serial.println("");

  //get features
  Serial.print("features: ");
  mySerial.write(CMD_FEATURES);
  while(mySerial.available() < 1); //wait for data
  while(mySerial.available())
  { 
    char c = mySerial.read();
    if(c & FEATURE_LCD){ Serial.print("LCD "); }
    if(c & FEATURE_TP) { Serial.print("TP ");  }
    if(c & FEATURE_ENC){ Serial.print("ENC "); }
    if(c & FEATURE_NAV){ Serial.print("NAV "); }
  }
  Serial.println("");

  //get width and height
  Serial.print("width/height: ");
  mySerial.write(CMD_LCD_WIDTH);
  while(mySerial.available() < 2); //wait for data
  x  = mySerial.read()<<8;
  x |= mySerial.read()<<0;
  Serial.print(x, DEC);
  Serial.print("/");
  mySerial.write(CMD_LCD_HEIGHT);
  while(mySerial.available() < 2); //wait for data
  y  = mySerial.read()<<8;
  y |= mySerial.read()<<0;
  Serial.print(y, DEC);
  Serial.println("");

  //backlight
  Serial.println("backlight...");
  mySerial.write(CMD_LCD_LED);
  mySerial.write(50); //0...100

  //clear screen
  Serial.println("clear...");
  mySerial.write(CMD_LCD_CLEARBG);

  //draw rect
  Serial.println("rect...");
  x=30; y=30; w=20; h=40;
  mySerial.write(CMD_LCD_DRAWRECTFG); //x0, y0, w, h
  write_pos(x);
  write_pos(y);
  write_pos(w);
  write_pos(h);

  //draw text
  Serial.println("text...");
  x=10; y=10;
  mySerial.write(CMD_LCD_DRAWTEXTFG); //x0, y0, size, clear, length, text
  write_pos(x);
  write_pos(y);
  mySerial.write(1); //size + clear
  mySerial.write(strlen("123")); //length
  mySerial.write('1');
  mySerial.write('2');
  mySerial.write('3');

  Serial.println("...end");
}


void loop()
{
}

/*
  Example sketch for UART communication (9600 baud).

  Note: To compile the program you have to add the file cmd.h in the Arduino IDE:
        Sketch -> Add file...
        Check function write_pos() if the screen is smaller than 255 pixel.
*/

#include <SoftwareSerial.h>
#include <Wire.h>
#include "cmd.h"

#define RGB(r,g,b) (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //16bit: 5 red | 6 green | 5 blue

#define I2C_ADDR 0x20

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
  mySerial.write(i>>8); //only for screens > 255 pixel, otherwise remove
  mySerial.write(i);
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

  //backlight
  Serial.println("backlight...");
  mySerial.write(CMD_LCD_LED);
  mySerial.write(50); //0...100

  //clear screen
  Serial.println("clear...");
  mySerial.write(CMD_LCD_CLEARBG);

  //draw image
  Serial.println("image...");
  x=5; y=5; w=40; h=40;
  mySerial.write(CMD_LCD_DRAWIMAGE); //x0, y0, w, h, color mode, pixel...
  write_pos(x);
  write_pos(y);
  write_pos(w);
  write_pos(h);
  mySerial.write(COLOR_RGB323);
  for(i=w*h; i!=0; i--)
  {
    mySerial.write(0x55);
  }

  //draw image (use RLE with magic number 0xAA)
  Serial.println("image (RLE)...");
  x=50; y=50; w=20; h=20;
  mySerial.write(CMD_LCD_DRAWIMAGE); //x0, y0, w, h, color mode, pixel...
  write_pos(x);
  write_pos(y);
  write_pos(w);
  write_pos(h);
  mySerial.write(COLOR_RGB323|COLOR_RLE);
  for(i=w*h; i!=0; i--)
  {
    mySerial.write(0xAA);
    mySerial.write(1);
    mySerial.write(0x55);
  }

  Serial.println("...end");
}


void loop()
{
}

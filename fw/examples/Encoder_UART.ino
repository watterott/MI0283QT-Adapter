/*
  Example sketch for UART communication (9600 baud).

  Note: To compile the program you have to add the file cmd.h in the Arduino IDE:
        Sketch -> Add file...
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

  //backlight
  Serial.println("backlight...");
  mySerial.write(CMD_LCD_LED);
  mySerial.write(50); //0...100

  //start test program
  Serial.println("test...");
  mySerial.write(CMD_TEST);
  wait_for_input();
  mySerial.write((unsigned char)0x00); //stop test
  while(mySerial.available() < 1); //wait for response
  mySerial.flush();

  delay(500); //wait 500ms (for exit test program)

  //clear screen
  Serial.println("clear...");
  mySerial.write(CMD_LCD_CLEARBG);

  wait_for_input();

  //enable rotary encoder
  Serial.println("encoder on...");
  mySerial.write(CMD_CTRL);
  mySerial.write(CMD_CTRL_FEATURES);
  mySerial.write(FEATURE_ENC);

  Serial.println("Move Rotary Encoder!");
}


void loop()
{
  int8_t p, s;

  mySerial.write(CMD_ENC_POS);
  while(mySerial.available() < 2); //wait for data
  p = mySerial.read();
  s = mySerial.read();

  Serial.print("ENC: ");
  Serial.print(p); //position: +/-127
  Serial.print(", ");
  Serial.print(s); //switch: 1=press, 3=long press
  Serial.println("");

  delay(1000);
}

# Connecting MI0283QT-2/MI0283QT-9A Adapter to Arduino
```
Signal  Display  Uno  Mega  Leonardo
----------------------------------------------
GND       1-2    GND   GND   GND   (ISP Pin 6)
5V        3-4     5V    5V    5V   (ISP Pin 2)
LED         5      9     9     9
RST         6      8     8     8
CS          7      7     7     7
SDI/MOSI    8     11    51    16   (ISP Pin 4)
SCL/SCK    10     13    52    15   (ISP Pin 3)
ADS-CS     11      6     6     6
```


# Connecting MI0283QT-9 Adapter v2 to Arduino

## SPI/SSI
```
Signal  Display  Uno  Mega  Leonardo
----------------------------------------------
GND       1-2    GND   GND   GND   (ISP Pin 6)
5V        3-4     5V    5V    5V   (ISP Pin 2)
RST         6      8     8     8
CS          7      7     7     7
SDI/MOSI    8     11    51    16   (ISP Pin 4)
SDI/MISO    9     12    50    14   (ISP Pin 1)
SCL/SCK    10     13    52    15   (ISP Pin 3)
```

## I2C
```
Signal  Display  Uno  Mega  Leonardo
------------------------------------
GND       1-2    GND   GND   GND 
5V        3-4     5V    5V    5V
SDA        14    SDA   SDA   SDA
SCL        13    SCL   SCL   SCL
```

## Hardware UART
```
Signal  Display  Uno  Mega  Leonardo
------------------------------------
GND       1-2    GND   GND   GND
5V        3-4     5V    5V    5V
CS          7      7     7     7
RX         12   1/TX  1/TX  1/TX
TX         11   0/RX  0/RX  0/RX
```

## Software UART (mSD-Shield)
```
Signal  Display  Uno  Mega  Leonardo
------------------------------------
GND       1-2    GND   GND   GND
5V        3-4     5V    5V    5V
CS          7      7     7     7
RX         12      5     5     5
TX         11      6     6     6
```

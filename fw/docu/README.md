# Display Firmware
**Documentation for Firmware v0.04 and greater.**


## Firmware Update
See [Firmware Update Guide](https://github.com/watterott/MI0283QT-Adapter/blob/master/fw/update_guide/README.md).


## Interfaces
The display can be controlled through 3 interfaces: I2C, SPI/SSI and UART. The default interface is I2C.
To activate SPI or UART set CS+MOSI or CS+RX to low after a reset or on power-up.
The current interface can be saved as default with the software command ```CMD_CTRL_SAVE``` and changed with ```CMD_CTRL_INTERFACE```.

**Default settings:**

* Interface: I2C
* Byte order: big endian
* I2C: address 0xA0, max. 400 kHz
* SPI: Mode 3, max. 4 MHz @ 48 MHz system clock
* UART: 9600 baud (8N1)
* System clock: 48 MHz

**I2C activation:**

    RST low, wait 10 ms, RST high
    CS low, MOSI + RX high
    wait 20 ms
    CS high

**SPI activation:**

    RST low, wait 10 ms, RST high
    CS + MOSI low (RX high)
    wait 20 ms
    CS high

**UART activation:**

    RST low, wait 10 ms, RST high
    CS + RX low (MOSI high)
    wait 20 ms
    CS high


## Examples

* [Arduino examples](https://github.com/watterott/MI0283QT-Adapter/tree/master/fw/examples)


## Commands

Parameters have a 8 bit width, expect *color values* (RGB565) and *position values* (x,y,z,w,h,r) on screens bigger than 255 pixel in one direction. These are 16 bit width.

Commands that end with *FG* or *BG* need no color value.

Example: ```CMD_LCD_CLEAR``` has as parameter a 2 byte color value and ```CMD_LCD_CLEARFG``` fills the screen with the foreground color and ```CMD_LCD_CLEARBG``` with the background color.


### General Commands

    CMD_NOP1
    CMD_NOP2
No operation.

    CMD_VERSION
Get firmware version. Returns 4 bytes, for example "0.12".

    CMD_TEST
Start the test program. To exit send ```0``` and the board responses with ```CMD_TEST```.

    CMD_FEATURES
Get features. Returns 1 byte with ```FEATURE_LCD```, ```FEATURE_TP```, ```FEATURE_ENC```, ```FEATURE_NAV``` and/or ```FEATURE_LDR``` set.

    CMD_CTRL
      CMD_CTRL_SAVE      //Save current settings to flash (sysclock, backlight, interface, baud rate, address, byte order, fg color, bg color, touchpanel calibration).
      CMD_CTRL_INTERFACE //Set interface. Parameter: 1 byte (INTERFACE_UART, INTERFACE_I2C, INTERFACE_SPI)
      CMD_CTRL_BAUDRATE  //Set UART baud rate. Parameter: 4 bytes = 32 bit (9600...1000000)
      CMD_CTRL_ADDRESS   //Set I2C address. Parameter: 1 byte
      CMD_CTRL_BYTEORDER //Set byte order. Parameter: 1 byte (0=big endian , 1=little endian)
      CMD_CTRL_SYSCLOCK  //Set system clock in MHz. Parameter: 1 byte (12,16,24,32,36,48)
      CMD_CTRL_FEATURES, //Enable or disable features. Parameter: 1 byte (FEATURE_TP, FEATURE_ENC, FEATURE_NAV, FEATURE_LDR)
General system settings/options. There is no return value.

    CMD_PIN
Planned function...

    CMD_ADC
Read ADC channel. Parameter: 1 byte (4, 5, 7 or 255 for LDR)


### Display Commands

    CMD_LCD_LED
Set or get display backlight power. Parameter: 1 byte (0-100 or 255 for reading)

    CMD_LCD_RESET
Reset the display controller.

    CMD_LCD_POWER
Set display panel power. Parameter: 1 byte (0=off, 1=on)

    CMD_LCD_RAWCMD
Send raw command to the display controller. Parameter: 1 byte
    
    CMD_LCD_RAWDAT
Send raw data to the display controller. Parameter: 1 byte
    
    CMD_LCD_ORIENTATION
Set display orientation. Parameter: 1 byte (0=0°, 9=90°, 18=180°, 27=270°)

![Orientation dia](https://rawgithub.com/watterott/MI0283QT-Adapter/master/fw/docu/orientation.svg)

    CMD_LCD_WIDTH
Get display width. Returns always 2 bytes = 16 bit.
    
    CMD_LCD_HEIGHT
Get display height. Returns always 2 bytes = 16 bit.
    
    CMD_LCD_INVERT
Invert display. Parameter: 1 byte (0=off, 1=on)
    
    CMD_LCD_FGCOLOR
Set foreground color. Parameter: color (2 byte, RGB565)
    
    CMD_LCD_BGCOLOR
Set background color. Parameter: color (2 byte, RGB565)
    
    CMD_LCD_TERMINAL
Start terminal mode. To exit send *0* and the board responses with ```CMD_LCD_TERMINAL```.

    CMD_LCD_DRAWIMAGE
Draw image. Parameter: x0, y0, w, h, color_mode, color...
    
    CMD_LCD_CLEAR
    CMD_LCD_CLEARFG
    CMD_LCD_CLEARBG
Clear display. Parameter: [color]

    CMD_LCD_DRAWPIXEL
    CMD_LCD_DRAWPIXELFG
    CMD_LCD_DRAWPIXELBG
Draw pixel. Parameter: x0, y0, [color]

    CMD_LCD_DRAWLINE
    CMD_LCD_DRAWLINEFG
    CMD_LCD_DRAWLINEBG
Draw line. Parameter: x0, y0, x1, y1, [color]

    CMD_LCD_DRAWLINES
    CMD_LCD_DRAWLINESFG
    CMD_LCD_DRAWLINESBG
Draw lines. Parameter: [color], n, x0, y0...xn, yn

    CMD_LCD_DRAWRECT
    CMD_LCD_DRAWRECTFG
    CMD_LCD_DRAWRECTBG
Draw rectangle. Parameter: x0, y0, w, h, [color]

    CMD_LCD_FILLRECT
    CMD_LCD_FILLRECTFG
    CMD_LCD_FILLRECTBG
Fill rectangle. Parameter: x0, y0, w, h, [color]

    CMD_LCD_DRAWRNDRECT
    CMD_LCD_DRAWRNDRECTFG
    CMD_LCD_DRAWRNDRECTBG
Draw round rectangle. Parameter: x0, y0, w, h, radius, [color]

    CMD_LCD_FILLRNDRECT
    CMD_LCD_FILLRNDRECTFG
    CMD_LCD_FILLRNDRECTBG
Fill round rectangle. Parameter: x0, y0, w, h, radius, [color]

    CMD_LCD_DRAWCIRCLE
    CMD_LCD_DRAWCIRCLEFG
    CMD_LCD_DRAWCIRCLEBG
Draw circle. Parameter: x0, y0, radius, [color]

    CMD_LCD_FILLCIRCLE
    CMD_LCD_FILLCIRCLEFG
    CMD_LCD_FILLCIRCLEBG
Fill circle. Parameter: x0, y0, radius, [color]

    CMD_LCD_DRAWELLIPSE
    CMD_LCD_DRAWELLIPSEFG
    CMD_LCD_DRAWELLIPSEBG
Draw ellipse. Parameter: x0, y0, radius_x, radius_y, [color]

    CMD_LCD_FILLELLIPSE
    CMD_LCD_FILLELLIPSEFG
    CMD_LCD_FILLELLIPSEBG
Fill ellipse. Parameter: x0, y0, radius_x, radius_y, [color]

    CMD_LCD_DRAWTEXT
    CMD_LCD_DRAWTEXTFG
    CMD_LCD_DRAWTEXTBG
Draw text. Parameter: [fg_color], [bg_color], x0, y0, size_clear (0x7F=size, 0x80=clear background), length, text

    CMD_LCD_DRAWSTRING
    CMD_LCD_DRAWSTRINGFG
    CMD_LCD_DRAWSTRINGBG
Draw string. Parameter: [fg_color], [bg_color], x0, y0, size_clear (0x7F=size, 0x80=clear background), text (end with 0x00)


### Touch-Panel Commands

Note: To activate the touch panel use ```CMD_CTRL + CMD_CTRL_FEATURES``` and set ```FEATURE_TP```.
      The touch panel will be checked in background continuously and the result can be read with the following commands.

    CMD_TP_POS
Get last position and pressure. Returns x, y, z.

    CMD_TP_X
Get last x position. Returns x.

    CMD_TP_Y
Get last y position. Returns y.

    CMD_TP_Z
Get current pressure. Returns z.

    CMD_TP_WAITPRESS
Wait till press and get position after press. Returns x, y.

    CMD_TP_WAITRELEASE
Wait till release and get position after release. Returns x, y.

    CMD_TP_WAITMOVE
Wait till move and get direction after move. Returns 1 byte (0x01=x-, 0x02=x+, 0x04=y-, 0x08=y+).

    CMD_TP_CALIBRATE
Calibrate touch panel. This will not save the calibration data to flash (see ```CMD_CTRL + CMD_CTRL_SAVE```). Returns ```CMD_TP_CALIBRATE``` after the calibration is completed or when *0* is sent to exit.


### Rotary-Encoder Commands

Note: To activate the rotary encoder use ```CMD_CTRL + CMD_CTRL_FEATURES``` and set ```FEATURE_ENC```.
      The encoder state will be checked in background continuously and the result can be read with the following commands.

    CMD_ENC_POS
Get position and switch state. Returns 2 bytes: position (-127...+127), state (0x01=press, 0x02=long press).

    CMD_ENC_SW
Get switch state. Returns 1 byte (0x01=press, 0x02=long press)

    CMD_ENC_WAITPRESS
Wait till press. Returns 2 bytes: position (-127...+127), state (0x01=press, 0x02=long press).

    CMD_ENC_WAITRELEASE
Wait till release. Returns 2 bytes: position (-127...+127), state (0x01=press, 0x02=long press).


### Navigation-Switch / Joystick Commands

Note: To activate the navigation switch use ```CMD_CTRL + CMD_CTRL_FEATURES``` and set ```FEATURE_NAV```.
      The switch will be checked in background continuously and the result can be read with the following commands.

    CMD_NAV_POS
Get position and switch state. Returns 3 bytes: left-right position (-127...+127), down-up position (-127...+127), state (0x01=press, 0x02=long press, 0x10=right, 0x20=left, 0x40=up, 0x80=down).

    CMD_NAV_SW
Get switch state. Returns 1 byte: state (0x01=press, 0x02=long press, 0x10=right, 0x20=left, 0x40=up, 0x80=down)

    CMD_NAV_WAITPRESS
Wait till press. Returns 3 bytes: left-right position (-127...+127), down-up position (-127...+127), state (0x01=press, 0x02=long press, 0x10=right, 0x20=left, 0x40=up, 0x80=down).

    CMD_NAV_WAITRELEASE
Wait till release. Returns 3 bytes: left-right position (-127...+127), down-up position (-127...+127), state (0x01=press, 0x02=long press, 0x10=right, 0x20=left, 0x40=up, 0x80=down).

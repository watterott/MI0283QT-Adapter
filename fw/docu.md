# Display Firmware

## Interfaces
The display can be controlled through 3 interfaces: I2C, SPI/SSI and UART. The default interface is I2C.
To activate SPI or UART set CS+MOSI or CS + RX to low after a reset or on power-up for 20ms.

**SPI activation:**

    RST low
    wait 10 ms
    RST high
    CS low
    MOSI low
    wait 20 ms
    CS high

**UART activation:**

    RST low
    wait 10 ms
    RST high
    CS low
    RX low
    wait 20 ms
    CS high


## Commands

Parameters have a 8 bit width, expect *color values* (RGB565) and *position values* (x,y,z,w,h,r) on screens bigger than 255 pixel in one direction. These are 16 bit width.

Commands that end with *FG* or *BG* need no color value.

Example: *CMD_LCD_CLEAR* has as parameter a 2 byte color values and *CMD_LCD_CLEARFG* fills the screen with the foreground color and *CMD_LCD_CLEARBG* with the background color.

### General

    CMD_NOP1
    CMD_NOP2
No operation.

    CMD_VERSION
Get firmware version. Returns 4 bytes, for example "0.12".

    CMD_TEST
Start the test program. To exit send *0* and the board responses with *CMD_TEST*.

    CMD_FEATURES
Get features. Returns 1 byte with FEATURE_LCD, FEATURE_TP, FEATURE_ENC or FEATURE_NAV set.

    CMD_CTRL
      CMD_CTRL_SAVE      //Save current settings to flash
      CMD_CTRL_INTERFACE //Set Interface (Parameter: 1 byte)
      CMD_CTRL_BAUDRATE  //Set UART baud rate (Parameter: 4 bytes = 32 bit)
      CMD_CTRL_ADDRESS   //Set I2C address (Parameter: 1 byte)
      CMD_CTRL_SYSCLOCK  //Set system clock in MHz (Parameter: 1 byte)
General system settings/options. There is no return value.

    CMD_PIN
Planned function...

    CMD_ADC
Planned function...

### Display

    CMD_LCD_LED
Set display backlight power. Parameter: 1 byte (0-100)

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
Start terminal mode. To exit send *0* and the board responses with *CMD_LCD_TERMINAL*.

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
Draw Line. Parameter: x0, y0, x1, y1, [color]

    CMD_LCD_DRAWLINES
    CMD_LCD_DRAWLINESFG
    CMD_LCD_DRAWLINESBG
Draw Lines. Parameter: [color], n, x0, y0...xn, yn

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
Draw text. Parameter: [fg_color], [bg_color], x0, y0, size_clear (0x7F=size, 0x80=clear), length, text

    CMD_LCD_DRAWSTRING
    CMD_LCD_DRAWSTRINGFG
    CMD_LCD_DRAWSTRINGBG
Draw string. Parameter: [fg_color], [bg_color], x0, y0, size_clear (0x7F=size, 0x80=clear), text (end with 0x00)

### Touch-Panel

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
Calibrate touch panel. This will not save the calibration data to flash (see *CMD_CTRL_SAVE*). Returns *CMD_TP_CALIBRATE* after the calibration is completed or when *0* is sent to exit.

### Rotary-Encoder

    CMD_ENC_POS
Get position and switch state. Returns 2 bytes: position (-127...+127), state (0x01=press, 0x02=long press).

    CMD_ENC_SW
Get switch state. Returns 1 byte (0x01=press, 0x02=long press)

    CMD_ENC_WAITPRESS
Wait till press. Returns 2 bytes: position (-127...+127), state (0x01=press, 0x02=long press).

    CMD_ENC_WAITRELEASE
Wait till release. Returns 2 bytes: position (-127...+127), state (0x01=press, 0x02=long press).

### Navigation-Switch

    CMD_NAV_POS
Get position and switch state. Returns 3 bytes: left-right position (-127...+127), down-up position (-127...+127), state (0x01=press, 0x02=long press, 0x10=right, 0x20=left, 0x40=up, 0x80=down).

    CMD_NAV_SW
Get switch state. Returns 1byte: state (0x01=press, 0x02=long press, 0x10=right, 0x20=left, 0x40=up, 0x80=down)

    CMD_NAV_WAITPRESS
Wait till press. Returns 3 bytes: left-right position (-127...+127), down-up position (-127...+127), state (0x01=press, 0x02=long press, 0x10=right, 0x20=left, 0x40=up, 0x80=down).

    CMD_NAV_WAITRELEASE
Wait till release. Returns 3 bytes: left-right position (-127...+127), down-up position (-127...+127), state (0x01=press, 0x02=long press, 0x10=right, 0x20=left, 0x40=up, 0x80=down).

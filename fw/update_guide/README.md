# How to update the Firmware?


## LPCSP
The firmware can be updated via the FTDI connector using [LPCSP](http://elm-chan.org/works/sp78k/report_e.html)
([alternative download](https://raw.github.com/watterott/MI0283QT-Adapter/master/fw/update_guide/lpcsp.zip)).

1. Hold down the ISP switch on the display board and run the program with the following parameters:

        lpcsp.exe -pX:115200 -c1 FIRMWARE.hex
        X            -> COM port number (1 to 99) of the FTDI adapter
        FIRMWARE.hex -> name of the Hex-File

2. Release the ISP switch after the programming (entering ISP mode) has started.

3. Wait till the programming has finished.


## Flash Magic
The firmware can be updated via the FTDI connector using [Flash Magic](http://www.flashmagictool.com).

1. Step 1 - Communications

        Device:    LPC1114/301
        COM Port:  select your FTDI adapter
        Baud Rate: 115200
        Interface: None (ISP)

2. Step 2 - Erase

        [x] Erase blocks used by Hex File

3. Step 3 - Hex File

        Select Hex file

4. Step 4 - Options

        [ ] Verify after programming
        [ ] Fill unsued Flash

5. Step 5 - Start!

        Hold down the ISP switch on the display board and press the Start button.
        Release the ISP switch after the programming (entering ISP mode) has started.

6. Wait till the programming has finished.


## Other programming tools
* [LPC21ISP](http://sourceforge.net/projects/lpc21isp/)
* [NXPprog](http://sourceforge.net/projects/nxpprog/)

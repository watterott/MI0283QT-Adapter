# How to update the Firmware?

## Firmware - Hex Files
* [GLCD-Shield](https://raw.github.com/watterott/MI0283QT-Adapter/master/fw/update_guide/fw_glcdshield.hex)
* [MI0283QT Adapter v2](https://raw.github.com/watterott/MI0283QT-Adapter/master/fw/update_guide/fw_mi0283qt9.hex)


## LPCSP
The firmware can be updated via the FTDI connector using [LPCSP](http://elm-chan.org/works/sp78k/report_e.html)
([alternative download](https://raw.github.com/watterott/MI0283QT-Adapter/master/fw/update_guide/lpcsp.zip)).

1. Connect a [5V USB-Serial-Converter with FTDI pinout](http://www.watterott.com/en/FTDI-Breakout-Reloaded-V11) to the 6-Pin FTDI connector on the PCB bottom side (blk=GND, grn=DTR).

2. Hold down the ISP switch (next to the ARM controller on the MI0283QT Adapter and next to the reset switch on the GLCD-Shield) and run the programming software with the following parameters:

        lpcsp.exe -pX:115200 -c1 FIRMWARE.hex
        X            -> COM port number (1 to 99) of the FTDI adapter
        FIRMWARE.hex -> name of the Hex-File

3. Release the ISP switch after the programming (entering ISP mode) has started.

4. Wait till the programming has finished.


## Flash Magic
The firmware can be updated via the FTDI connector using [Flash Magic](http://www.flashmagictool.com).

1. Connect a [5V USB-Serial-Converter with FTDI pinout](http://www.watterott.com/en/FTDI-Breakout-Reloaded-V11) to the 6-Pin FTDI connector on the PCB bottom side (blk=GND, grn=DTR).

2. Start Flash Magic

3. Step 1 - Communications

        Device:    LPC1114/301
        COM Port:  select your FTDI adapter
        Baud Rate: 115200
        Interface: None (ISP)

4. Step 2 - Erase

        [x] Erase all

5. Step 3 - Hex File

        Select Hex file

6. Step 4 - Options

        [ ] Verify after programming
        [ ] Fill unsued Flash

7. Step 5 - Start!

        Hold down the ISP switch (next to the ARM controller on the MI0283QT Adapter and next to the reset switch on the GLCD-Shield) and press the Start button.
        Release the ISP switch after the programming (entering ISP mode) has started.

8. Wait till the programming has finished.


## Other programming tools
* [LPC21ISP](http://sourceforge.net/projects/lpc21isp/)
* [NXPprog](http://sourceforge.net/projects/nxpprog/)

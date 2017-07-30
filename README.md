# MI0283QT-Adapter
Adapter Board with 2.8" TFT-Display and Touchscreen/Touchpanel.

![MI0283QT-Adapter](https://github.com/watterott/MI0283QT-Adapter/raw/master/hardware/MI0283QT_v20.jpg)

* Multi-Inno MI0283QT 2.8" Display with Touchpanel (320x240 pixel)
* Backlight dimmable
* 3.3V - 5V tolerant IOs
* Power: 3.3V - 5V
* **MI0283QT-Adapter v1**
  * TI ADS7846/TSC2046 Touch-Controller
  * Interface: SPI/SSI (8-Bit or 9-Bit, up to 32MHz)
* **MI0283QT-Adapter v2**
  * NXP LPC1114 ARM Cortex-M0 with Open-Source Firmware
  * Interface: UART, I2C, SPI/SSI (8-Bit, Mode 3, up to 4MHz)
  * Pin-compatible with MI0283QT-Adapter v1


## Shop
* [MI0283QT-Adapter v1](http://www.watterott.com/en/MI0283QT-2-Adapter) (with ADS7846/TSC2046 Touch-Controller)
* [MI0283QT-Adapter v2](http://www.watterott.com/en/MI0283QT-Adapter-v2) (with ARM Cortex-M0 Microcontroller)


## Hardware
* [Dimension and Pinout](https://raw.github.com/watterott/MI0283QT-Adapter/master/hardware/MI0283QT_size.pdf)
* [Schematics + Layout](https://github.com/watterott/MI0283QT-Adapter/tree/master/hardware)
* [3D Model from MI0283QT-Adapter v2](http://grabcad.com/library/mi0283qt-adapter-2-8-240x320-display-with-touch-panel-1)
* [Connecting to an Arduino](https://github.com/watterott/MI0283QT-Adapter/blob/master/Arduino-Connection.md)
* [Hints about Displays (e.g. ghost images, screen burn-in)](https://github.com/watterott/KnowledgeBase/wiki/Displays#hints-about-displays)


## Software
* **MI0283QT-Adapter v1**
  * [Arduino Library and Examples](https://github.com/watterott/Arduino-Libs)
  * [Raspberry Pi Framebuffer](https://github.com/watterott/RPi-ShieldBridge/blob/master/docs/MI0283QT-Adapter.md#mi0283qt-adapter-v1)
  * [BeagleBone Black Framebuffer](https://github.com/notro/fbtft/wiki/BeagleBone-Black)

* **MI0283QT-Adapter v2**
  * [General Examples](https://github.com/watterott/MI0283QT-Adapter/tree/master/firmware/examples)
  * [Arduino Library and Examples](https://github.com/watterott/Arduino-Libs)
  * [Raspberry Pi Framebuffer](https://github.com/watterott/RPi-ShieldBridge/blob/master/docs/MI0283QT-Adapter.md#mi0283qt-adapter-v2)
  * [BeagleBone Black Framebuffer](https://github.com/notro/fbtft/wiki/BeagleBone-Black)
  * [Firmware Source Code](https://github.com/watterott/MI0283QT-Adapter/tree/master/firmware)
  * [Firmware Update Guide](https://github.com/watterott/MI0283QT-Adapter/blob/master/firmware/update_guide/README.md)
  * [Command Description](https://github.com/watterott/MI0283QT-Adapter/blob/master/firmware/docu/README.md)


## Projects
* [Arduino TouchLib](http://www.mafu-foto.de/elektronik/arduino)
* [Arduino Touch Screen Lib](http://code.google.com/p/arduino-touch-gui)
* [Arduino+chipKIT UTFT Lib](http://www.rinkydinkelectronics.com/library.php?id=51)
* [Arduino PrXoS + MI0283QT-2 Control Api](http://arduinoprx.de)
* [Arduino DSO](http://skyduino.wordpress.com/2011/07/08/arduinoscillo-news/)
* [Misc Arduino Programs](http://www.mon-club-elec.fr/pmwiki_mon_club_elec/pmwiki.php?n=MAIN.ArduinoExpertTFTGraphCouleur240x320)
* [Arduino Paint](http://nicksteen.webs.com/projects)
* [mbed + MI0283QT-2](https://developer.mbed.org/users/clemente/notebook/mi0283qt-library-and-program-demonstration/)
* [Raspberry Pi + MI0283QT-2](http://lallafa.de/blog/2013/03/watterott-display-on-raspberry-pi/)
* [Raspberry Pi + MI0283QT-9A](http://lallafa.de/blog/2013/03/watterott-mi0283qt-9a-display-for-the-rasbperry-pi/)
* [BeagleBone Black + MI0283QT-9A](http://noisezero.blogspot.it/2013/12/watterott-mi0283qt-9a-display-on.html)

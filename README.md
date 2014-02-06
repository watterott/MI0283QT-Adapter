# MI0283QT-Adapter
Adapter Board with 2.8" TFT-Display and Touchscreen/Touchpanel.

Shop:
[MI0283QT-Adapter v1](http://www.watterott.com/en/MI0283QT-2-Adapter), 
[MI0283QT-Adapter v2](http://www.watterott.com/en/MI0283QT-Adapter-v2)

![MI0283QT-Adapter](https://raw.github.com/watterott/MI0283QT-Adapter/master/img/mi0283qt-adapter.jpg)


## Features
* **MI0283QT-Adapter v1**
  * Multi-Inno 2.8" Display with Touchpanel (240x320 pixel)
  * TI ADS7846/TSC2046 Touch-Controller
  * Interface: SPI/SSI (up to 32MHz)
  * Backlight dimmable (PWM)
  * 3.3V - 5V tolerant IOs
  * Power: 3.3V - 5V

* **MI0283QT-Adapter v2**
  * Multi-Inno 2.8" Display with Touchpanel (240x320 pixel)
  * NXP LPC1114 ARM Cortex-M0 with Open-Source Firmware
  * Interface: UART, I2C, SPI/SSI (Mode 3, up to 4MHz)
  * Backlight dimmable via interface
  * Pin-compatible with MI0283QT-Adapter v1
  * 3.3V - 5V tolerant IOs
  * Power: 3.3V - 5V


## Hardware
* [Dimension and Pinout](https://raw.github.com/watterott/MI0283QT-Adapter/master/pcb/MI0283QT_size.pdf)
* [3D Model from MI0283QT-Adapter v2](http://grabcad.com/library/mi0283qt-adapter-2-8-240x320-display-with-touch-panel-1)
* [Connecting Adapter to an Arduino](https://github.com/watterott/MI0283QT-Adapter/blob/master/Arduino-Connection.md)


## Software
* MI0283QT-Adapter v1
  * [Arduino Library with Examples](https://github.com/watterott/Arduino-Libs) ([old version](https://github.com/watterott/mSD-Shield/tree/5054db114faef1bcfd9c1d165ed713a681a0edea/src))
  * [Raspberry Pi Framebuffer](https://github.com/watterott/RPi-ShieldBridge/blob/master/docu/MI0283QT-Adapter.md#mi0283qt-adapter-v1)
  * [BeagleBone Black Framebuffer](https://github.com/notro/fbtft/wiki/BeagleBone-Black)

* MI0283QT-Adapter v2
  * [General Arduino Examples](https://github.com/watterott/MI0283QT-Adapter/tree/master/fw/examples)
  * [Arduino Library with Examples](https://github.com/watterott/Arduino-Libs)
  * [Raspberry Pi Framebuffer](https://github.com/watterott/RPi-ShieldBridge/blob/master/docu/MI0283QT-Adapter.md#mi0283qt-adapter-v2)
  * [BeagleBone Black Framebuffer](https://github.com/notro/fbtft/wiki/BeagleBone-Black)
  * [Firmware Source Code](https://github.com/watterott/MI0283QT-Adapter/tree/master/fw)
  * [Firmware Update Guide](https://github.com/watterott/MI0283QT-Adapter/blob/master/fw/update_guide/README.md)
  * [Command Description](https://github.com/watterott/MI0283QT-Adapter/blob/master/fw/docu/README.md)


## Projects
* [Projects using the MI0283QT-Adapter](https://github.com/watterott/MI0283QT-Adapter/blob/master/Projects.md)

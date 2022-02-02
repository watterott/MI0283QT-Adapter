Display Firmware
================
Firmware for MI0283QT-Adapter v2 and GLCD-Shield (with LPC1114 ARM Cortex-M0).
Visit https://github.com/watterott/MI0283QT-Adapter for updates.

Supported IDEs: CrossWorks for ARM and LPCXpresso/MCUxpresso


Documentation
-------------
  See docu.md


Third party software
--------------------
  CMSIS
  http://www.arm.com/products/processors/cortex-m/cortex-microcontroller-software-interface-standard.php


License
-------
  See license.txt


History
-------
  Jan 26 2022  v0.08  Added CMD_LCD_ENABLE + CMD_LCD_DISABLE.

  Jun 30 2014  v0.07  Bugfix in set_pwm.

  Jun 08 2014  v0.06  Bugfix in CMD_LCD_ORIENTATION+CMD_LCD_DRAWTEXT.
                      Bugfix in CMD_LCD_DRAWIMAGE.
                      Bugfix in touch calibration routine.
                      Added CMD_SYNC.

  Dec 09 2013  v0.05  Compiling with LPCXpresso.
                      Added interrupt/status output.

  Sep 18 2013  v0.04  Added ADC read function.

  Aug 18 2013  v0.03  Added byte order setting.

  Jun 22 2013  v0.02  TP/ENC/NAV have to be enabled.
                      Added light sensor support.

  Jun 17 2013  v0.01  First release.

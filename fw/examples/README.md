# Arduino Examples

* Download the respective .ino file and the file ```cmd.h```.
* Copy everything to the same directory.
* Open the .ino file with the Arduino IDE.
* Open menu item **Sketch** -> **Add file** and choose ```cmd.h```.


# Raspberry Pi Examples

## I2C

* Activate I2C:

    ```
    $ sudo modprobe i2c_bcm2708 baudrate=100000
    $ sudo modprobe i2c-dev
    ```

* Install i2c-tools:

    ```
    $ sudo apt-get update
    $ sudo apt-get install i2c-tools
    ```

* Test I2C bus:

    ```
    $ sudo i2cdetect -y 1
    ```
    *The Raspberry Pi hardware revision 1 boards connect I2C bus 0 (GPIO 0 + 1) and revision 2 boards connect I2C bus 1 (GPIO 2 + 3) to the GPIO connector.*

* Start the test program on the display:

    ```
    $ sudo i2cset -y 1 0x20 0x02
    ```

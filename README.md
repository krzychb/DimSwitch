# DimSwitch

This is an Arduino library to control electronic dimmable ballasts for fluorescent light tubes.

It works with ESP8266, Arduino UNO, Micro, MEGA, DUE and other compatible Arduino boards.

For description of API and application examples, please refer to [documentation](extras/) section. You will find there also description and graphs of state machine sequence, wave-forms of control signals as well as sample user interface in [openHAB](http://www.openhab.org/).


## Tested with

* [Philips HF-REGULATOR Touch and Dim HF-R T 236 220-240](http://www.lighting.philips.com/pwc_li/in_en/assets/docs/products/DALI%20Dimmable%20Ballast.pdf)
* [OSRAM QUICKTRONIC - INTELLIGENT QTi DALI 2x28/54 DIM](https://www.osram.com/media/resource/hires/335428/data-sheet-for-product-families-osram-qti-dali...dim.pdf)


## Functions

* Switch the lamp on / off
* Dim to specific light intensity
* Read back on/off status and intensity
* Calibrate


## Required

* ESP8266 or other Arduino compatible board
* Relay connected to one of outputs
* Light sensor connected to one of analog inputs
* Touch and dim lamp connected to the relay


## Application Example

![DimSwitch Application Example](extras/pictures/application-example.png)


## Contribute

Feel free to contribute to the project in any way you like! 

If you find any issues with code or descriptions please report them using *Issues* tab above. 


## Author

krzychb


## Donations

Invite me to freshly squeezed orange juice.


## License

[GNU LESSER GENERAL PUBLIC LICENSE - Version 2.1, February 1999](LICENSE)

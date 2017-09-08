Getting Started with OSD-Contiki using osd-merkur-256 merkurboard
=============================================================
This guide's aim is to help you start using Contiki for merkurboards
The platform supports different AtMega-RF boards:

* Based on MCU AtMega256RFR2
* Based on MCU AtMega128RFA1 

Boards all looks the same. AtMega256RFR2 boards are labeled with "deRFmega256".
The platform and CPU code, are common for both boards, can be found under 
`$(CONTIKI)/platfrom/osd-merkur-256`,
`$(CONTIKI)/platfrom/osd-merkur-128`
The port was developed and tested with Merkurboard.
This guide assumes that you have basic understanding of how to use the 
command line and perform basic admin tasks on UNIX family OSs. You should
also have understanding about the Contiki OS design as well have C 
programming skills.

OSD-Contiki Features
--------------------
* Contiki Features
* Arduino compatible
* use Arduino Libs in your Project (see ../examples/osd/arduino-* )
* store rf parameter at eeprom
* OTA-Features (Update over the Air)

Board Features
----------------
* wire Antenna.
* Robust radio performance. Up to 300 meter line-of-sight.
* Unique EUI64 address via Bootloader
* 32kHz RTC xtal
* Standard. 6-pin TTL-USB header for FTDI cable for UART.
* PCB formfactor for cheap project breadboard
* Power/current:
  * RX ~10mA (Full RPC AtMegaXXRFR2). 
  * Sleep ~45uA @ 16MHz XTAL
  * Sleep ~15uA @  8MHz using internal oscillator
* Preprogammed arduino bootloader with update over the Air support. 
* CE certified by test institute.

UART 
----
The board has one UART via the 6-pin TTL-USB adapter, The recommended
baudrate is 38400 bps. This speed gives the lowest error with respect 
of the used clock frequency used internally. A possible hi-speed is 
250000 bps wich gives 0% Error with 16MHz clock. 

Port Features
--------------
The platform has the following key features:
* Standard, E64 address from built-in chip.
* First hooks for RPC (Reduced Power Consumption) for AtMegaXXXRFR2. 

Toolchain needs
---------------
The Atmel toolcahin is available in most operating system. 
Note. We use Debian 9 as reference Linux


###### For Linux

sudo apt-get install avr-libc binutils-avr gcc-avr gdb-avr simulavr avrdude make git 

sudo apt-get install python-pip

pip install intelhex

sudo apt-get install picocom libcoap-1-0-bin

###### For Windows

Goes here // todo


get the source code:
--------------------

git clone https://github.com/osdomotics/osd-contiki.git
cd osd-contiki
git submodule update --init --recursive

Contiki build TARGET
--------------------

For AtMega128RFR2 boards:

    make TARGET=osd-merkur-256

For AtMega128RFA1 boards:

    make TARGET=osd-merkur-128

Flashing
--------
Programming using avrdude using serial bootloader. (TTL-USB cable)

Flashing commnad line example 256k MCU:

   make TARGET=osd-merkur-256 flash

Flashing commnad line example 128k MCU:

   make TARGET=osd-merkur-128 flash

Tested applications and examples
---------------------------------

see osd-contiki/example/osd/..

Platform tutorial applications
-----------------------------

todo ...

Regressions tests
-----------------
Travis compile regression test for the platform: 
[regression-tests/20-compile-osd-ports](../../regression-tests/20-compile-osd-ports).

This port adds newer version of `avr-gcc` compiler (4.9.2), to support 
testing of newer Atmel MCU as Atmega256RFR2.


Board approvals
---------------
Summary: 
* R&TTE 73/23/EEC, 89/336/EEC and 99/5/EC
* Safety: EN 60950-1:2006 + A12: 2011
* RF: ETSI EN 300 328 V1.7.1 (2006-10)
* EMC: ETSI EN 301 489-1 V1.9.2 (2011-09), ETSI EN 301 489-17 V2.2.1 (2012-09)
* EMF: EN 62479:2010
* Human exposure to electromagnetic fields: EN 62479:2010 

Commercial availability
------------------------
Through vendor and though resellers. Note board is will only available 
were CE approval is covered. This may be further restricted by WEEE.
Contact vendor. For research legislation is more relaxed in most 
countries.

References
----------
AtMega64/128/256/RFR2 chip documentation available via Atmel.
Schematics and boards description. Available via Radio-Senors
Smart Reduced Power Consumption Techniques. AT02594 available via Atmel.

ToDo
-----
API for radio power saving settings introduced Atmels app note AT02594. 
Also function for the desensitizing RPC resister.

Vendor info
-----------
http://wiki.osdomotics.com
http://iot-shop.at/


Maintainer
----------
Harald Pichler <harald@the-develop.net>

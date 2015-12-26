#!/bin/bash
sudo avrdude -pm256rfa1 -c stk500 -P/dev/ttyUSB0 -b57600 -e -U flash:w:border-router.RaspBee.hex:a -U eeprom:w:border-router.RaspBee.eep:a

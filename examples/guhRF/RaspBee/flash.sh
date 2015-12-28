#!/bin/bash
sudo avrdude -pm256rfr2 -c stk500 -P/dev/ttyAMA0 -b57600 -e -U flash:w:border-router.RaspBee.hex:a -U eeprom:w:border-router.RaspBee.eep:a

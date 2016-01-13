#!/bin/bash
# For the new bootloader (using a jump-table) you want to use
# BOOTLOADER_GET_MAC=0x0001ff80 (which is the current default) - Bonsai Merkur board
# BOOTLOADER_GET_MAC=0x0003ef80 guhRF board
make clean TARGET=RaspBee
make TARGET=RaspBee BOOTLOADER_GET_MAC=0x0003ef80
avr-size -C --mcu=MCU=atmega256rfr2 border-router.RaspBee
avr-objcopy -j .text -j .data -O ihex border-router.RaspBee border-router.RaspBee.hex
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex border-router.RaspBee border-router.RaspBee.eep

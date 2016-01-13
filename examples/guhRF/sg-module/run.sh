#!/bin/bash
# For the new bootloader (using a jump-table) you want to use
# BOOTLOADER_GET_MAC=0x0003ff80 (which is the current default)
echo -n "Enter MAC ADDRESS>"
read mac
make clean TARGET=guhRF
make TARGET=guhRF BOOTLOADER_GET_MAC=0x0003ff80 EUI64_ADDRESS="$mac"

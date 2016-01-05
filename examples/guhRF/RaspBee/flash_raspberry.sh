#!/bin/bash

set -e
#set -x

BASH_GREEN="\e[1;32m"
BASH_RED="\e[1;31m"
BASH_NORMAL="\e[0m"

function printGreen(){
 echo -e "${BASH_GREEN}$1${BASH_NORMAL}"
}

if [ "$UID" -ne 0 ]; then
 echo -e "${BASH_RED}Please start the script as root${BASH_NORMAL}"
 exit
fi

printGreen "Stop tunslib6..."

sudo systemctl stop tunslib6

if [ ! -d /sys/class/gpio/gpio18 ]; then
 printGreen "Export GPIO18"
 sudo echo "18" > /sys/class/gpio/export
fi

if [ ! -d /sys/class/gpio/gpio17 ]; then
 printGreen "Export GPIO17"
 sudo echo "17" > /sys/class/gpio/export
fi

printGreen "Set GPIO17 out"
sudo echo "out" > /sys/class/gpio/gpio17/direction
printGreen "Set GPIO17 low"
sudo echo "0" > /sys/class/gpio/gpio17/value

printGreen "Set GPIO18 out"
sudo echo "out" > /sys/class/gpio/gpio18/direction
printGreen "Set GPIO18 low"
sudo echo "0" > /sys/class/gpio/gpio18/value
sleep 1
printGreen "Set GPIO17 high"
sudo echo "1" > /sys/class/gpio/gpio17/value

printGreen "Flash firmware"
sudo avrdude -pm256rfr2 -c stk500 -P/dev/ttyAMA0 -b57600 -D -U flash:w:border-router.RaspBee.hex:a -U eeprom:w:border-router.RaspBee.eep:a || true

printGreen "Set GPIO18 high"
sudo echo "1" > /sys/class/gpio/gpio18/value

printGreen "Start tunslib6"
sudo systemctl restart tunslib6

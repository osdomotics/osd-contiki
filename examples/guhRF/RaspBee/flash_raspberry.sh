#!/bin/bash

echo "killing tunslip"
sudo killall tunslip6

if which gpio > /dev/null; then
	echo "wiringPi exist" 
else
	echo "installing wiringPi"
	git clone git://git.drogon.net/wiringPi
	cd wiringPi
	./build
fi

sudo gpio mode 0 out
sudo gpio write 0 0
sleep 1
sudo gpio write 0 1

sudo avrdude -pm256rfr2 -c stk500 -P/dev/ttyAMA0 -b57600 -D -U flash:w:border-router.RaspBee.hex:a -U eeprom:w:border-router.RaspBee.eep:a

echo "Now you can run tunslip6.sh again"

Arduino compatibility example
=============================

This example shows that it is now possible to re-use arduino sketches in
Contiki. This example documents the necessary magic. Arduino specifies
two routines, `setup` and `loop`. Before `setup` is called, the
framework initializes hardware. In original Arduino, all this is done in
a `main` function (in C). For contiki we define a process that does the
same.

See the documentation file in apps/contiki-compat/README.md

sensor in air: 

power on sensor
sensor.begin
I2C Soil Moisture Sensor Address: 20
Sensor Firmware version: 25
start measure
check if sensor ready
sensor is ready, read measurements:
Temp: 27.20		Soil: 179.00		Light: 527.00
turn power off


sensor in water: 

start again in next round
power on sensor
sensor.begin
I2C Soil Moisture Sensor Address: 20
Sensor Firmware version: 25
start measure
check if sensor ready
sensor is ready, read measurements:
Temp: 27.60		Soil: 738.00		Light: 283.00
turn power off


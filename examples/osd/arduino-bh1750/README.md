BH1750 Digital Light Sensor
===========================

Simple demo to read out sensors from the BH1750 sensor.

Demo uses the BH1750FVI Arduino library.
This library interfaces with the BH1750FVI ambient light sensor.

Build
-----
make clean TARGET=osd-merkur-256 flash

Bebug Serial Line
-----------------

screen /dev/ttyUSB0 38400



Arduino compatibility example
=============================

This example shows that it is now possible to re-use arduino sketches in
Contiki. This example documents the necessary magic. Arduino specifies
two routines, `setup` and `loop`. Before `setup` is called, the
framework initializes hardware. In original Arduino, all this is done in
a `main` function (in C). For contiki we define a process that does the
same.

See the documentation file in apps/contiki-compat/README.md


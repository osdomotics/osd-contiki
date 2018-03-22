Arduino compatibility example
=============================

make clean TARGET=osd-merkur-256 flash

This example shows that it is now possible to re-use arduino sketches in
Contiki. This example documents the necessary magic. Arduino specifies
two routines, `setup` and `loop`. Before `setup` is called, the
framework initializes hardware. In original Arduino, all this is done in
a `main` function (in C). For contiki we define a process that does the
same.

See the documentation file in apps/contiki-compat/README.md

how to connect the epaper module to the merkurboard: 

  * BUSY -> D14
  * RST  -> D19
  * DC   -> D20
  * CS   -> D10 (SS/SPI)
  * CLK  -> D12 (SCK/SPI)
  * DIN  -> D11 (MOSI/SPI)
  * GND  -> GND
  * 3.3V -> 3.3V


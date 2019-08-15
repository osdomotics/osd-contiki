Arduino compatibility example
=============================

First go to apps/tinydtls and issue the command
./reconf.sh
You must have autotools (automake etc.) installed.

Then in this directory:
make clean TARGET=osd-merkur-256 WITH_TINYDTLS=1 WITH_COAPSERVER=1 WITH_DTLS_COAP=1 flash

Boottext:

*******Booting Contiki-2.6-5232-gecf0091a4*******
Panid:aaaa
EUI-64 MAC: 0-21-2e-ff-ff-0-ca-3c
CSMA ContikiMAC, channel 25 , check rate 8 Hz tx power 0
RPL Enabled
Routing Enabled
Started DTLS retransmit process
COAP-DTLS listening on port 5684
Online

Addresses [4 max]
fe80::221:2eff:ff00:ca3c

Neighbors [20 max]

Routes [4 max]
  <none>
---------
Never-used stack > 9300 bytes

-----------------------------------------
$ git clone https://github.com/obgm/libcoap.git --recursive
$ cd libcoap
$ ./autogen.sh
$ ./configure --with-tinydtls --disable-shared
$ make
$ cd examples
$ ./coap-client -m get coaps://[2a00:1158:2:7102:221:2eff:ff00:ca3c]/.well-known/core -k 'secretPSK' -u 'Client_identity'

-----------------------------------------

This example shows that it is now possible to re-use arduino sketches in
Contiki. This example documents the necessary magic. Arduino specifies
two routines, `setup` and `loop`. Before `setup` is called, the
framework initializes hardware. In original Arduino, all this is done in
a `main` function (in C). For contiki we define a process that does the
same.

See the documentation file in apps/contiki-compat/README.md

Arduino shell example
=====================
This example start a shell on merkurboard serial0 line.
You can change the network parameters and store it in eeprom.

make clean TARGET=osd-merkur-256 flash

start a terminal programm
-------------------------

picocom -c -b 38400 --omap crcrlf /dev/ttyUSB0

? [ENTER]

Available commands:
?: shows this help
ccathresholds <threshold: change cca thresholds -91 to -61 dBm (default -77)
exit: exit shell
help: shows this help
kill <command>: stop a specific command
killall: stop all running commands
macconf <conf>: change mac layer 0 -> do nothing; 1 -> Radio allways on
null: discard input
panid <0xabcd>: change panid (default 0xabcd)
quit: exit shell
reboot: reboot the system
rfchannel <channel>: change radio channel (11 - 26)
saverfparam <> save radio parameters txpower, channel, panid to eeprom settingsmanager
txpower <power>: change transmission power 0 (3dbm, default) to 15 (-17.2dbm)

example merkurboard set rf-parameter:
-------------------------------------

panid 0xabcd
rfchannel 11
saverfparam

example old merkurboard (atmega128rfa1) set rf-parameter:
---------------------------------------------------------

macconf 1
*) switch power off/on
panid 0xabcd
rfchannel 11
saverfparam
macconf 0
*) switch power off/on

------------------------------------------------------------------------------
quit picocom, you need to press Ctrl-a , then Ctrl-q
------------------------------------------------------------------------------
Read eeprom to disk:
avrdude -p m256rfr2  -c stk500v2  -P /dev/ttyUSB0 -b 57600 -U eeprom:r:eeprom_img.hex:i
Write eeprom to Merkurboard:
avrdude -p m256rfr2  -c stk500v2  -P /dev/ttyUSB0 -b 57600 -U eeprom:w:eeprom_img.hex:i

todo: 

atmega128rfa1 reboot dont work correct @ fixme

if rfsleep mode, it is not possible to set panid @ fixme 
(workaround: set macconf 1, power off, power on, panid 0xabcd, saverfparam, set macconf 0, power off, power on)

------------------------------------------------------------------------------

This example shows that it is now possible to re-use arduino sketches in
Contiki. This example documents the necessary magic. Arduino specifies
two routines, `setup` and `loop`. Before `setup` is called, the
framework initializes hardware. In original Arduino, all this is done in
a `main` function (in C). For contiki we define a process that does the
same.

See the documentation file in apps/contiki-compat/README.md

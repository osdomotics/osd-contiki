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
null: discard input
panid <0xabcd>: change panid (default 0xabcd)
ps: list all running processes
quit: exit shell
reboot: reboot the system
rfchannel <channel>: change radio channel (11 - 26)
saverfparam <> save radio parameters txpower, channel, panid to eeprom settingsmanager
txpower <power>: change transmission power 0 (3dbm, default) to 15 (-17.2dbm)

------------------------------------------------------------------------------
quit picocom, you need to press Ctrl-a , then Ctrl-q



This example shows that it is now possible to re-use arduino sketches in
Contiki. This example documents the necessary magic. Arduino specifies
two routines, `setup` and `loop`. Before `setup` is called, the
framework initializes hardware. In original Arduino, all this is done in
a `main` function (in C). For contiki we define a process that does the
same.

See the documentation file in apps/contiki-compat/README.md

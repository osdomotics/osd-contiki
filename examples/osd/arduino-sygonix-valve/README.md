Sygonix Heat Valve Example
==========================

Here we use a sygonix heat valve as a general purpose adjustable valve.

However here it is used as a full open/close switch for automatic plant 
watering.

The hardware documentation is currently missing, but i will check in the 
code now. Instead of the LCD a ~1" OLED display is used.

You can get one here: 

	https://www.conrad.at/de/sygonix-ht100-heizkoerperthermostat-elektronisch-8-bis-28-c-1377979.html

CoAP Ressources
---------------

- ``s/pulses`` get the pulses accumulated from the start

- ``a/command`` put ``o`` to open, ``c`` to close


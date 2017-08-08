Access to Gill Windsonic Anemometer
===================================

We use the second serial port to receive data from the Gill Windsonic
ultrasonic anemometer:

    http://gillinstruments.com/products/anemometer/windsonic.htm

The Windsonic needs to be set up properly to send data periodically
(every two seconds in my setup) in the Polar Format with fixed CSV
sizes at 9600 baud.

I used a MAX3323EEPE to translate the RS232 from Windsonic to Merkurboard
3.3V levels.

The complete setup is driven by a 12V solar charged acid battery. The
/power_supply ressource measures the voltage level on the input.
The voltage is measured by a simple 1M/100k voltage divider directly
connected to the merkurboard.

A small DC/DC converter is used to power the merkurboard.

Next step maybe put the merkurboard and MAX3323 in the Windsonic's case
to have the first open source ipv6 addressable windsonic on earth ;-)



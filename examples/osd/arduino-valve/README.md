Arduino valve
=============

This is an example how to open/close a Gardena Water-Valve (the ones
that use a +9V block battery for power).
Note that we used the L293D Dual H-Bridge circuit which needs +5V for
power supply (while the Merkur-Board uses 3.3V) but it can use 3.3V
inputs directly from the Merkur-Board without a level shifter.  So it's
probably a good idea to use some other H-Bridge circuit that can also
use 3.3V. An advantage of the L293D might be that it can be used for two
valves simultaneously (with the second, currently unused H-Bridge).

We provide the schematics of our breadboard-design in
valve-breadboard.png -- as noted above it's a good idea to use a
different H-Bridge circuit.

Also note that one of the pins we use in the software for signals to the
H-Bridge (D4) is also used for the LED on the Merkurboard. The LED is
*on* when the H-Bridge is *off*. This was nice for testing the
breadboard circuit but should be changed for a real deployment to save
power of the battery. It is also probably a good idea to turn off the
power-supply of the H-Bridge with an additional MOSFET depending on the
power consumption of the H-Bridge you're going to use.

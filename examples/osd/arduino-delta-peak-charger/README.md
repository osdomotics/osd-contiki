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

This example also holds a minus-delta-peak charger!

there is one pin that switches charging on and off and two analog inputs,
one for the source volage (solar panel) and the other for the battery
(2 AA NiMh Cells).

V_solar o-o---------o------------     ----------------o-----------------o Vbat
          |         |            v   / 2N4403         |             |
          |         |            -----                |             |
          |         |              |                  |             |
          |         _              _                  _             |
          |        | |            | |                | |            |
          |        | | 10k        | | 120Ohm         | | 10k        |
          |         -              -                  -             |
         ---        | vsol_pin     |                  | vbat_pin   ---
    10uF ---     ---o---           |               ---o---         --- 10uF
          |      |     |           o t_pin         |     |          |
          |      _     _                           _     _          |
          |     | |   | |                         | |   | |         |
          | 10k | |   | | 10k                 10k | |   | | 10k     |
          |      -     -                           -     -          |
          |      |     |                           |     |          |
GND o-----o------o-----o---------------------------o-----o----------o---o GND

t_pin: toggles the charging switch
vbat_pin: anaolg in with 1.6V ref measures battery voltage
vsol_pin: analog in with 1.6V ref measures source (solar) voltage

voltages are divided by three (10k resistors) see V_SCALE in code if you change it

see sketch.pde for current definitions and further details

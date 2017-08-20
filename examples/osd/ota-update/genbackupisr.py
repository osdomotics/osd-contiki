#!/usr/bin/python

""" Generate backup ISR table from given .hex file
    Output is written to out.hex
"""

import sys
import intelhex

ihex = intelhex.IntelHex (sys.argv [1])
bin  = ihex.tobinstr () [:0x200]

hex2 = intelhex.IntelHex ()
hex2.puts (0x1ed00, bin)
hex2.tofile ("out.hex", format="hex")

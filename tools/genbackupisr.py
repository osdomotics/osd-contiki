#!/usr/bin/python

""" Generate backup ISR table from given .hex file
    Output is written to out.hex
"""

import sys
import intelhex

ihex = intelhex.IntelHex (sys.argv [1])
bin  = ihex.tobinstr () [:0x200]

ihex.puts (0x1ed00, bin)
ihex.tofile (sys.stdout, format="hex")

#!/usr/bin/python

from __future__ import division
from math import log

mx = 256 // 8

print "unsigned const char led_setting [] ="

for k in range (mx) :
    if k :
        print ", ",
    else :
        print "{ ",
    print "(unsigned char)", int (round (log (k + 1) / log (mx) * 255 + .5)) - 1

print "};"



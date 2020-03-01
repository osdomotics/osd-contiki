import serial
import time

dumpfile = "serial.dump"

c = serial.Serial ("/dev/ttyUSB1", baudrate = 38400, timeout = 5)

while 1 : 
    l = c.readline () 
    l = l.strip ()
    print ("%.2f %s" % (time.time(), l))
    f = open (dumpfile, "a")
    f.write ("%.2f %s\n" % (time.time(),l))
    f.close ()


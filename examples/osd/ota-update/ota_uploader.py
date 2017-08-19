#!/usr/bin/python

import sys
import intelhex
from   subprocess import Popen

class Upload_Info (object) :

    def __init__ (self, ip, filename) :
        self.ip       = ip
        self.filename = filename
    # end def __init__

    def read_hex (self) :
        self.ihex = intelhex.IntelHex (self.filename)
    # end def read_hex

    def get_coap_int (self, path) :
        """ Get coap variable of type integer
        """
        url = 'coap://[%s]/%s' % (self.ip, path)
        cmd = ('coap-client', '-m', 'get', '-A', 'text/plain', url)
        p = Popen (cmd, stdout = PIPE, stderr = PIPE)
        out, err = p.communicate ()
        if err :
            raise IOError (err)
        return int (out)
    # end def get_coap_int

    def output (self) :
        sys.stdout.write (self.ihex.tobinstr ())
    # end def output

# end class Upload_Info

ui = Upload_Info (sys.argv [1], sys.argv [2])
ui.read_hex ()
ui.output ()

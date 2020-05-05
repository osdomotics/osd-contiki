#!/usr/bin/python

from __future__ import print_function
import sys
import intelhex
from   subprocess import check_call, check_output, STDOUT
import re
import argparse
import sys

EXE_REX = re.compile (r"^EXE=(.*)$",re.MULTILINE)

import logging
logging.basicConfig (level = logging.INFO)
log = logging.getLogger ("ota-uploader")

class Upload_Info (object) :

    def __init__ (self, ip) :
        self.ip = ip
        if not self.has_ota () :
            log.error ("No OTA on this Merkurboard")
            sys.exit (1)

        self.ap = self.get_coap_int ("/ota/active_part")
        self.ok = self.get_coap_int ("/ota/part_ok?part=%d" % self.ap)
        self.bd = self.get_coap_int ("/ota/boot_default")
        self.bn = self.get_coap_int ("/ota/boot_next")
    # end def __init__

    def _cc (self, path, output = False, m = "get", **kws) :
        cmd = ["coap-client", "-m", m]

        opts = [("-%s" % k, str (v)) for k, v in kws.items ()]
        for x in opts :
            cmd.extend (x)

        cmd.append ("coap://[%s]%s" % (self.ip, path))
        log.debug (cmd)

        if output is False :
            res = check_output (cmd, stderr = STDOUT)
            res = res.strip ()

            log.debug ("res: %r" % res)
            if res.startswith ("4.") :
                raise StandardError ("error communicating: %r" % ((path, m, kws), ))
            return res
        else :
            check_call (cmd)
    # end def _cc

    def read_hex (self, filename) :
        self.ihex = intelhex.IntelHex (filename)
        log.info ("read_hex: 0x%x 0x%x" % (self.ihex.minaddr (), self.ihex.maxaddr ()))
    # end def read_hex

    def write_bin (self, filename) :
        open (filename, "wb").write (self.ihex.tobinstr ())
    # end def write_bin

    def has_ota (self) :
        res = self._cc ("/ota/active_part")
        return res and res in "01"
    # end def has_ota

    def get_coap_int (self, path) :
        """ Get coap variable of type integer
        """
        res = self._cc (path, A = "text/plain")
        if "." in res :
            raise ValueError ("%r not an integer" % res)
        return int (res)
    # end def get_coap_int

    def query (self) :
        print ("active_part  = %s" % self.ap)
        print ("boot_default = %s" % self.bd)
        print ("boot_next    = %s" % self.bn)
        print ("part 0 ok    = %s" % self.get_coap_int ("/ota/part_ok?part=0"))
        print ("part 1 ok    = %s" % self.get_coap_int ("/ota/part_ok?part=1"))
    # end def query

    def reboot (self) :
        self._cc ("/ota/reboot", m = "put", e = "OK", t = "text/plain")
    # end def reboot

    def set_ok (self) :
        self._cc ("/ota/part_ok?part=%d" % self.ap, m = "put", e = "1", t = "text/plain")
        self._cc ("/ota/boot_next", m = "put", e = self.ap, t = "text/plain")
        self._cc ("/ota/boot_default", m = "put", e = self.ap, t = "text/plain")
    # end def set_ok

    def make_and_upload (self) :
        if self.ok != 1 :
            log.error ("Active Partition is not set to OK, cant continue")
            sys.exit (1)

        if not (self.ap == self.bd == self.bn) :
            log.error ("active_part  = %d" % self.ap)
            log.error ("boot_default = %d" % self.bd)
            log.error ("boot_next    = %d" % self.bn)
            log.error ("cant continue !")
            sys.exit (1)

        if self.ap == 0 :
            self.tp = 1
        else :
            self.tp = 0
        log.info ("active partition = %d, compiling for partition: %d" % (self.ap, self.tp))

        exe = re.findall (EXE_REX, open ("Makefile", "r").read ()) [0]
        cmd = \
            ( "make -j TARGET=osd-merkur-256 BOOTLOADER_PARTITION=%d %s.osd-merkur-256.%d.hex"
            % (self.tp, exe, self.tp)
            )
        cmd = cmd.split ()
        log.debug (cmd)
        check_call (cmd)

        hex_file = "%s.osd-merkur-256.%d.hex" % (exe, self.tp)
        bin_file = "%s.osd-merkur-256.%d.bin" % (exe, self.tp)

        self.read_hex  (hex_file)
        self.write_bin (bin_file)

        res = self._cc \
            ( "/ota/update"
            , m = "put"
            , t = "application/octet-stream"
            , b = "64"
            , f = bin_file
            , output = True
            )

        if self.get_coap_int ("/ota/boot_next") != self.tp :
            log.error ("not successful ...")
            sys.exit (1)
    # end def make_and_upload

    def output (self) :
        sys.stdout.write (self.ihex.tobinstr ())
    # end def output

# end class Upload_Info

if __name__ == "__main__" :
    p = argparse.ArgumentParser (description = 'OTA Update for merkurboards')
    p.add_argument \
        ( "ip_address"
        , metavar = 'IP-Address'
        , type    = str
        , help    = 'IPv6 Address of merkurboard'
        )

    p.add_argument \
        ( '--reboot'
        , dest = 'reboot'
        , action = 'store_true'
        , help   = 'reboot the board'
        )

    p.add_argument \
        ( "--set-ok"
        , dest = "set_ok"
        , action = "store_true"
        , help   = "set active partition 'ok'"
        )

    p.add_argument \
        ( "--query"
        , dest   = "query"
        , action = "store_true"
        , help   = "query current settings state of merkurboard"
        )

    args = p.parse_args ()

    ui = Upload_Info (args.ip_address)

    if args.query :
        ui.query ()
        sys.exit (0)

    if args.set_ok :
        ui.set_ok ()
        sys.exit (0)

    if args.reboot :
        ui.reboot ()
        sys.exit (0)

    ui.make_and_upload ()

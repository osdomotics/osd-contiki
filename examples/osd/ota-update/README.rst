==========
OTA Update
==========

OTA stands for "Over the Air". OTA update is used to flash a new
firmware to a device over the air. This documents (some of) the
requirements for OTA and how to use the current prototype
implementation.

For the impatient
=================

First see the Security note below.

Then, if you've decided you want to try this: This is experimental code,
use at your own risk.

OK, how to run this: 
There is now a new make variable that determines which image (the one
for the upper or the one for the lower half of flash memory) is to be
built. Set ::

    BOOTLOADER_PARTITION=0

for building the first partition (the default) or set it to 1 for
building the second one.

There are now two ELF files, ``ota.osd-merkur-256.0`` and
``ota.osd-merkur-256.1``. From each of these two .hex files can be
generated (we're describing only the files for the first image, the
filenames of the second image are determined by replacing '0' with '1'
in the following):

- ota.osd-merkur-256.0.hex is the file used for OTA update
- ota.osd-merkur-256.0-combined.hex is the file used for uploading via
  the bootloader. This file has an additional section that contains a
  copy of the irq-vector table (see below for further details). This is
  only necessary if you're using the latest bootloader.

To upload an image via OTA:

- Create the ``.hex`` file with::

   make TARGET=osd-merkur-256 BOOTLOADER_PARTITION=1 ota.osd-merkur-256.1.hex

- Generate the ``.bin`` file with::

  ./ota_uploader.py x ota.osd-merkur-256.1.hex > ota.osd-merkur-256.1.bin

  Note that the ``ota_uploader.py`` tool is intended to turn into a
  full-fledged upgrade tool. This is work in progress. The first
  parameter will be the destination IP-Address in the future and is
  currently ignored.

- Upload this image to the ``/update`` resource, either via the firefox
  copper plugin or via the commandline::

      coap-client -t application/octet-stream -f ota.osd-merkur-256.1.bin
      -m put -b 64 'coap://[2001:db8:c001:f00d:221:2eff:ff00:5dd4]/update'

  Be sure to specify the type ``application/octet-stream`` if you are
  uploading via copper. Also make sure you replace the IP address above
  with the IP address of your board to be upgraded.

CoAP-Resources, Image Management
++++++++++++++++++++++++++++++++

The bootloader now keeps a directory of the images. In this directory we
keep the following information which can be retrieved or set via CoAP
resources:

- partition-ok: A flag per partition that indicates if this partition
  has ever been successfully booted. This flag can only be set for a
  partition that is currently running. Via CoAP this is set via the
  ``/part_ok`` URI. In addition to the URI, a query-parameter indicating
  the partition has to be specified, e.g., ``/part_ok?part=1``.
  Note that when a partition is flashed via the bootloader (and not via
  OTA) this flag is *not* set. So if later a new partition is loaded via
  OTA and the old partition never was marked OK, the old partition has
  to be rebooted before it can be made the default partition again.
  The part_ok resource can currently not be queried via a GET request.
- boot-default: The default boot partition, can be changed if the new
  default partition has the partition-ok flag set. The CoAP resource is
  ``/boot_default`` and it can be queried via a GET request and set via
  a PUT request, the parameter to the PUT request is the partition
  number.
- boot-next: A temporary flag that can be set on a partition to boot it
  just the next time. If booting fails the system will automatically
  boot the boot-default partition next time. This is automatically set
  after uploading an image. The CoAP resource is ``boot_next``. It can
  also be changed via the resource similar to ``/boot_default`` but
  without any constraints.

Additional CoAP resources exist to query the bootloader for parameters
that are not kept in the directory:

- ``/part_count``: Currently always 2, in the future the bootloader may
  support more than two partitions
- ``/part_size``: The size of one partition, all partitions are equal in
  size.
- ``/part_start``: This resource needs an additional query-parameter
  indicating the partition number, e.g., ``/part_start?part=1`` and
  returns the partition start address in flash.
- ``/active_part``: The partition that is currently booted.

How to use in your own code
===========================

- add app "ota-update" to the Makefile
- add #include "ota-update.h"
- add OTA_ACTIVATE_RESOURCES() to your code to activate the resources

Security
========

This is experimental code. There is currently no security, everybody (!)
can update your node. So use this only in experimental setups for now
until client-side DTLS authentication is available.

Position Independent Code
=========================

The new contiki-osd-merkur-256 target should have enough memory for two
independent application images. An application image should include the
code for over-the-air update to ensure it can be upgraded in the field.

Upgrading an image means writing a new image into the other half of the
flash memory (the part which does not run the current image). Since an
image has internal addresses and is usually linked to fixed addresses we
have two options:

- Find a way to generate position independent code for the Atmel
  microcontrollers so that an image can be used in the lower- or upper
  half of flash memory
- Generate two images, one linked for the lower, one linked for the
  upper half of flash memory.

It seems the GCC Atmel compiler cannot generate position independent
code. So we have to modify the first option to make it work: We can use
some magic during loading of an image to link it to the correct half of
flash-memory during loading. This can either mean full relocation of the
image (the same job that is normally done before runtime by the linker)
or offline-generation of a jump-table for all objects (functions) that
are accessed and the bootloader then only relocates the addresses in the
jump-table.

The first implementation will use two images (one for upper-, one for
lower half). We may decide to add on-the-fly relocation or we may always
ship two images (e.g. in a .zip file) and create a flash tool that
determines the correct image from the system to be updated.

Memory Layout
=============

The following table might have changed when you read this. See the
``stk500boot_atmega256rfr2`` bootloader on github, in particular the
file ``flash_layout.h`` for details.

  +--------------------------------------+
  | 3E000-3FFFF Bootloader               |
  +--------------------------------------+
  | 3DE00-3DFFF Flash image directory    |
  +--------------------------------------+
  | 3D600-3DDFF IRQVec copy upper image  |
  +--------------------------------------+
  | 1EF00-3D5FF                          |
  | Upper Image                          |
  |                                      |
  |                                      |
  +--------------------------------------+
  | 1E700-1EEFF IRQVec copy lower image  |
  +--------------------------------------+
  | 00000-1E6FF                          |
  | Lower Image                          |
  |                                      |
  |                                      |
  +--------------------------------------+
  | 00000-001FF IRQVec running image     |
  +--------------------------------------+

We have two identical images. Each image contains the IRQ vectors (and
some code after the vector table) in the lower two pages. A copy of
these pages (currently 8 pages as of this writing) is kept after the
image. The reason is that the IRQ vectors are fixed at address 00000 in
this processor architecture. In addition the compiler creates jumptables
(so-called trampoline code) to reach functions everywhere in memory via
a near call. So for running an image we need to copy the irq-vectors to
the fixed location (and therefore we keep a backup to be able to restore
the original image at that location).

We use the irq vectors in the bootloader to determine the
currently-running image: The first vector at position 0 is a jump to the
start of our program. From the address of this jump we can find out
which image is currently running.

Note that in the table above an image as generated by the compiler
consists of the IRQ vectors in the first pages plus the rest of the code
for that image.

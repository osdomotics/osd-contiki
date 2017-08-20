/*
 * Gardena 9V Magnet-Valve
 * We have a CoAP Resource for the Valve, it can be in state 1 (on) and
 * 0 (off).
 * Transition on-off outputs a negative pulse
 * Transition off-on outputs a positive pulse
 */

extern "C" {
#include <stdio.h>
#include "contiki.h"
#include "contiki-net.h"
#include "er-coap.h"
#include "bootloader_if.h"
#include "resources.h"
char resname[] = "update";
}


void setup (void)
{
    rest_init_engine ();
    rest_activate_resource (&res_upload_image, resname);
    rest_activate_resource (&res_part_count,     (char *)"part_count");
    rest_activate_resource (&res_part_size,      (char *)"part_size");
    rest_activate_resource (&res_boot_default,   (char *)"boot_default");
    rest_activate_resource (&res_boot_next,      (char *)"boot_next");
    rest_activate_resource (&res_callers_part,   (char *)"callers_part");
}

void loop (void)
{
    printf ("part-count: %ld\n",      bootloader_get_part_count   ());
    printf ("part-size: %ld\n",       bootloader_get_part_size    ());
    printf ("part-start 0: 0x%lx\n",  bootloader_get_part_start   (0));
    printf ("part-start 1: 0x%lx\n",  bootloader_get_part_start   (1));
    printf ("boot-default: %ld\n",    bootloader_get_boot_default ());
    printf ("boot-next: %ld\n",       bootloader_get_boot_next    ());
    printf ("callers-part: %ld\n",    bootloader_get_callers_part ());
}

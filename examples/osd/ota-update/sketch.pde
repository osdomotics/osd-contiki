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
    rest_activate_resource (&res_part_start,     (char *)"part_start");
    rest_activate_resource (&res_part_ok,        (char *)"part_ok");
}

void loop (void)
{
    printf ("Hello\n");
}

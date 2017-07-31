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
extern resource_t res_upload_image;
char resname[] = "update";
}


void setup (void)
{
    rest_init_engine ();
    rest_activate_resource (&res_upload_image, resname);
}

void loop (void)
{
    //printf ("Huhu\n");
}

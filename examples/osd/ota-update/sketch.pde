/*
 * Simple example with ota-update only
 */

extern "C" {
#include <stdio.h>
#include "contiki.h"
#include "contiki-net.h"
#include "bootloader_if.h"
#include "er-coap.h"
#include "ota-update.h"
}

void setup (void)
{
    rest_init_engine ();
    OTA_ACTIVATE_RESOURCES();
}

void loop (void)
{
    printf ("Hello %lu\n",bootloader_get_active_part());
    printf ("just sitting round and waiting for ota update\n");
}

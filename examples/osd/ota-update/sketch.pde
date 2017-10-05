/*
 * Simple example with ota-update only
 */

extern "C" {
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
    printf ("just sitting round and waiting for ota update\n");
}

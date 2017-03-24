/**
 * \defgroup Magnetic Valve
 *
 * Resource definition for Magnetic valve
 *
 * @{
 */

/**
 * \file
 *         Resource definitions for the Magnetic valve
 *      
 * \author
 *         Ralf Schlatterbeck <rsc@tux.runtux.com>
 */

#ifndef valve_h
#define valve_h
#include "contiki.h"
#include "contiki-net.h"
#include "er-coap.h"

#define ENABLE_PIN  2
#define BRIDGE1_PIN 3
#define BRIDGE2_PIN 4

extern uint8_t  valve;
extern resource_t res_valve;

#endif // valve_h
/** @} */

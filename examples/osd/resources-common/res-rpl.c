/**
 * \file
 *      Resources to get smallest_rssi, neighbortable, routes
 * \author
 *      Marcus Priesch <marcus@priesch.co.at>
 *
 * \brief get min rssi since last call (min rssi gets reset after the call)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "jsonparse.h"
#include "er-coap.h"
#include "generic_resource.h"
#include "Arduino.h"

//#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
//#include "net/rpl/rpl.h"
#include "net/link-stats.h"

extern int8_t rf230_smallest_rssi;

size_t
smallest_rssi_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  size_t res;
  res = snprintf (buf, bufsize, "%d", rf230_smallest_rssi);
  rf230_smallest_rssi = 0;
  return res;
}

GENERIC_RESOURCE \
    ( smallest_rssi
    , SMALLEST_RSSI
    , smallest_rssi
    , 0
    , 0
    , smallest_rssi_to_string
    );


size_t
nbt_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  size_t res = 0;

  static uip_ds6_nbr_t *nbr;
  linkaddr_t * linkaddr;
  const struct link_stats * linkstats;

  for (nbr = nbr_table_head(ds6_neighbors); nbr != NULL; nbr = nbr_table_next(ds6_neighbors, nbr)) {
    char * state = "X";
    switch (nbr->state) {
      case NBR_INCOMPLETE: state = "I";break;
      case NBR_REACHABLE: state = "R";break;
      case NBR_STALE: state = "S";break;
      case NBR_DELAY: state = "D";break;
      case NBR_PROBE: state = "P";break;
      }
    linkaddr = nbr_table_get_lladdr(ds6_neighbors, nbr);
    linkstats = link_stats_from_lladdr(linkaddr);
    res += snprintf \
      ( buf + res
      , bufsize - res
      , "%02x%sE%dR%dF%d "
      , (((&nbr->ipaddr)->u8[sizeof(uip_ipaddr_t)-2]) << 8) + ((&nbr->ipaddr)->u8[sizeof(uip_ipaddr_t)-1])
      , state
      , linkstats->etx
      , linkstats->rssi
      , linkstats->freshness
      );
  }
  return res;
}

GENERIC_RESOURCE \
    ( nbt
    , NBT
    , nbt
    , 0
    , 0
    , nbt_to_string
    );


size_t
routes_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  size_t res = 0;

  static uip_ds6_route_t *r;

  for (r = uip_ds6_route_head(); r != NULL; r = uip_ds6_route_next(r)) {
    res += snprintf \
      ( buf + res
      , bufsize - res
      , "%02x/%uV%02x/%lu "
      , (((&r->ipaddr)->u8[sizeof(uip_ipaddr_t)-2]) << 8) + ((&r->ipaddr)->u8[sizeof(uip_ipaddr_t)-1])
      , r->length
      , (uip_ds6_route_nexthop(r)->u8[sizeof(uip_ipaddr_t)-2] << 8) + uip_ds6_route_nexthop(r)->u8[sizeof(uip_ipaddr_t)-1]
      , (unsigned long)r->state.lifetime
      );
  }
  return res;
}

GENERIC_RESOURCE \
    ( routes
    , ROUTES
    , routes
    , 0
    , 0
    , routes_to_string
    );




/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=4
 */

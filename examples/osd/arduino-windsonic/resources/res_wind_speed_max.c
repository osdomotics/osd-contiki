/*
** Copyright (C) 2017 Marcus Priesch, All rights reserved
** In Prandnern 31, A--2122 Riedenthal, Austria. office@priesch.co.at
** ****************************************************************************
**
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
** ****************************************************************************
**
**++
** Name
**    wind_speed_max
**
** Purpose
**    provide wind_speed_max
**
**
** Revision Dates
**     8-Aug-2017 (MPR) Creation
**    ««revision-date»»···
**--
*/

#include "contiki.h"

#include <string.h>
#include "rest-engine.h"
#include "Arduino.h"

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_wind_speed_max,
         "title=\"Windspeed max last minute\";rt=\"m/s\"",
         res_get_handler,
         NULL,
         NULL,
         NULL);

extern int ws_max_speed_hi, ws_max_speed_lo;
extern char ws_unit    [8];

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

  unsigned int accept = -1;
  REST.get_header_accept(request, &accept);

  if(accept == -1 || accept == REST.type.TEXT_PLAIN) {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%02d", ws_max_speed_hi, ws_max_speed_lo);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));

  } else if(accept == REST.type.APPLICATION_JSON) {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'max':%d.%02d,'unit':'%s'}", ws_max_speed_hi, ws_max_speed_lo, ws_unit);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));

  } else {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}




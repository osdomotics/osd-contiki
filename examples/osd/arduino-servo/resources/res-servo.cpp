/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Door resource
 * \author
 *      Harald Pichler <harald@the-develop.net>
 */
#include "Arduino.h"
extern "C" {
#include "contiki.h"
#include <string.h>
#include "rest-engine.h"
}
#include "Servo.h"
extern Servo servo;

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/* A simple getter example. Returns the reading from the sensor with a simple etag */
RESOURCE(res_servo,
         "title=\"SERVO: , POST/PUT mode=on|off\";rt=\"Control\"",
         res_get_handler,
         res_post_put_handler,
         res_post_put_handler,
         NULL);

//extern  uint8_t servo_pin;
//extern  uint8_t servo_status;

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  unsigned int accept = -1;
  REST.get_header_accept(request, &accept);

//todo  if(accept == -1 || accept == REST.type.TEXT_PLAIN) {
//todo    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
//todo    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", servo_status);
//todo
//todo    REST.set_response_payload(response, buffer, strlen((char *)buffer));
//todo  } else if(accept == REST.type.APPLICATION_JSON) {
//todo    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
//todo    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'servo':%d}", servo_status);
//todo
//todo    REST.set_response_payload(response, buffer, strlen((char *)buffer));
//todo  } else {
//todo    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
//todo    const char *msg = "Supporting content-types text/plain and application/json";
//todo    REST.set_response_payload(response, msg, strlen(msg));
//todo  }
}

static void
res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *value = NULL;
  int success = 1;

//  if(success && (len = REST.get_post_variable(request, "mode", &mode))) {
//    if(strncmp(mode, "on", len) == 0) {
//      digitalWrite(servo_pin, LOW);
//      servo_status=1;
//    } else if(strncmp(mode, "off", len) == 0) {
//      digitalWrite(servo_pin, HIGH);
//      servo_status=0;
//    } else {
//      success = 0;
//    }
//  } else {
//    success = 0;
//  } if(!success) {
//    REST.set_response_status(response, REST.status.BAD_REQUEST);
//  }
  if(success && (len = REST.get_post_variable(request, "value", &value))) {
          int val = atoi(value);
          //int val = map(newval, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
          servo.write(val);
  }

  if(success && (len = REST.get_post_variable(request, "strike", &value))) {
          int val = atoi(value);
          //int val = map(newval, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
          servo.write(val);
          delay(20);
          servo.write(90);
  }

}

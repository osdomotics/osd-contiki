
#include "contiki.h"
#include <string.h>
#include "contiki.h"
#include "rest-engine.h"
#include "dev/sg-ready.h"

static void res_get_sg_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_sg_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_sg_ready,          
	"title=\"SG-Ready\";rt=\"sg_mode\"", 
	res_get_sg_handler, 
	res_post_sg_handler, 
	NULL, 
	NULL);

static void
res_get_sg_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  uint16_t state = sg_get_state();

  unsigned int accept = -1;
  REST.get_header_accept(request, &accept);

  if(accept == -1 || accept == REST.type.TEXT_PLAIN) {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u", state);

    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  } else if(accept == REST.type.APPLICATION_XML) {
    REST.set_header_content_type(response, REST.type.APPLICATION_XML);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<State =\"%u\">", state);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  } else if(accept == REST.type.APPLICATION_JSON) {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'sg-ready':{'mode':%u}", state);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  } else {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain, application/xml, and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  } 
}

static void
res_post_sg_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int success = 1;
  int length = 0;
  const char *value = NULL;
  size_t len = 0;

  int sg_state = sg_get_state();

  if (success &&  (len=REST.get_post_variable(request, "sg_mode", &value))) {
       sg_set_state((unsigned char) atoi(value));
  } else {
       success = 0;
  }

  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}



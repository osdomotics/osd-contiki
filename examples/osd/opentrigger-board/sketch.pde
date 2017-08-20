/*
 * Sample arduino sketch using contiki features.
 * We turn the LED off 
 * We allow read the moisture sensor
 * Unfortunately sleeping for long times in loop() isn't currently
 * possible, something turns off the CPU (including PWM outputs) if a
 * Proto-Thread is taking too long. We need to find out how to sleep in
 * a Contiki-compatible way.
 * Note that for a normal arduino sketch you won't have to include any
 * of the contiki-specific files here, the sketch should just work.
 */

extern "C" {
#include "opentrigger.h"
#include "rest-engine.h"
#include "er-coap-engine.h"
#include "generic_resource.h"
#include "net/netstack.h"
#include "dev/button-sensor.h"
#include "ChainableLED.h"
#include "sketch.h"
}

extern resource_t 
    res_led,
    res_bled,
    res_rgb, 
    res_red, 
    res_green, 
    res_blue, 
    res_battery, 
    res_cputemp,
    res_event,
    res_separate,
    res_server;

uint8_t led_pin=4;
uint8_t led_status;
uint8_t bled_pin=7;
uint8_t bled_status;

// coap_server_post
#define REMOTE_PORT UIP_HTONS(COAP_DEFAULT_PORT)
// should be the same :-)
#define UIP_NTOHS(x) UIP_HTONS(x)
#define SERVER_NODE(ip) uip_ip6addr(ip,0xaaaa,0,0,0,0,0,0,0x01)

uip_ipaddr_t server_ipaddr, tmp_addr;

#define NUM_LEDS  1

// Merkurboard grove i2c D8, D9
ChainableLED leds(8, 9, NUM_LEDS);

uint8_t color_rgb [3] = {0, 0, 0};


static uint8_t 
name_to_offset (const char * name)
{
  uint8_t offset = 0;
  if (0 == strcmp (name, "green")) {
    offset = 1;
  } else if (0 == strcmp (name, "blue")) {
    offset = 2;
  }
  return offset;
}

extern "C" size_t
color_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
  return snprintf (buf, bsize, "%d", color_rgb [name_to_offset (name)]);
}

extern "C" int 
color_from_string
    (const char *name, const char *uri, const char *query, const char *s)
{
    color_rgb [name_to_offset (name)] = atoi (s);
    leds.setColorRGB(0,color_rgb [0], color_rgb [1], color_rgb [2]);
    return 0;
}
 
extern "C" int 
color_rgb_from_string (const char *r, const char *g, const char *b)
{
  //  printf("rgb r=%s; g=%s; b=%s;\n",r,g,b);
    color_rgb [0] = atoi (r);
    color_rgb [1] = atoi (g);
    color_rgb [2] = atoi (b);
    leds.setColorRGB(0,color_rgb [0], color_rgb [1], color_rgb [2]);
    return 0;
}

static size_t
ip_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
    #define IP(x) UIP_NTOHS(server_ipaddr.u16[x])
    return snprintf
        ( buf, bsize, "%x:%x:%x:%x:%x:%x:%x:%x"
        , IP(0), IP(1), IP(2), IP(3), IP(4), IP(5), IP(6), IP(7)
        );
}

int ip_from_string
    (const char *name, const char *uri, const char *query, const char *s)
{
    /* Returns 1 if successful, only copy valid address */
    if (uiplib_ip6addrconv (s, &tmp_addr)) {
        uip_ip6addr_copy (&server_ipaddr, &tmp_addr);
        return 0;
    }
    return -1;
}

#pragma GCC diagnostic ignored "-Wwrite-strings"
GENERIC_RESOURCE
  ( server_ip
  , ip
  , ipv6_address
  , 1
  , ip_from_string
  , ip_to_string
  );
#pragma GCC diagnostic pop
  
void setup (void)
{
    // switch off the led
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, HIGH);
    led_status=0;
    // switch off the bled
    pinMode(bled_pin, OUTPUT);
    digitalWrite(bled_pin, LOW);
    bled_status=0;
    // init chainable led
    leds.init();
    leds.setColorRGB(0,color_rgb [0], color_rgb [1], color_rgb [2]);
    // sensors
    SENSORS_ACTIVATE(button_sensor);
    // init coap resourcen
    rest_init_engine ();
    SERVER_NODE (&server_ipaddr);
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_led,     "s/led");
    rest_activate_resource (&res_bled,    "s/bled");
    rest_activate_resource (&res_battery, "s/battery");
    rest_activate_resource (&res_cputemp, "s/cputemp");
    rest_activate_resource(&res_event,    "s/button");
    rest_activate_resource (&res_server_ip,"button/ip");
    rest_activate_resource (&res_red,     "led/R");
    rest_activate_resource (&res_green,   "led/G");
    rest_activate_resource (&res_blue,    "led/B");         
    rest_activate_resource (&res_rgb,     "led/RGB");         
    #pragma GCC diagnostic pop

//    mcu_sleep_set(64);
//    NETSTACK_MAC.off(1);
}

int coap_server_post(void)
{
    char buf [25];
    static coap_packet_t        request [1]; /* Array: treat as pointer */
    coap_transaction_t          *transaction;
    static int32_t              levent_counter; 

    char server_resource [20] = "button";
    int buttonstate           = button_sensor.value(0);

    sprintf (buf, "state=%d&event=%lu",buttonstate,levent_counter++);
//  printf("post\n");
//  printf ("%s\n", buf);
    coap_init_message (request, COAP_TYPE_NON, COAP_PUT, 0);
    coap_set_header_uri_path (request, server_resource);
    coap_set_header_content_format (request, REST.type.TEXT_PLAIN);
    coap_set_payload (request, buf, strlen (buf));
    request->mid = coap_get_mid ();
    transaction = coap_new_transaction
        (request->mid, &server_ipaddr, REMOTE_PORT);
    transaction->packet_len = coap_serialize_message
        (request, transaction->packet);
    coap_send_transaction (transaction);
    return 0;
}

void button (void)
{
      /* Call the event_handler for this application-specific event. */
      res_event.trigger();

      /* Also call the separate response example handler. */
      res_separate.resume();
      
      /* Call the Coap Server */
      coap_server_post();
}

void loop (void)
{

// test chainable led
/*
   static byte power=0;
   for (byte i=0; i<NUM_LEDS; i++)
   {
     if (i%2 == 0)
       leds.setColorRGB(i, power, 0, 0);
     else
       leds.setColorRGB(i, 0, 255-power, 0);
   }
   power+= 10;
*/
}

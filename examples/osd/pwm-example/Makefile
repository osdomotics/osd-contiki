EXE=er-example-server

all: $(EXE)

CONTIKI=../../..

# Contiki IPv6 configuration
CONTIKI_WITH_IPV6 = 1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PROJECT_SOURCEFILES += resource_led_pwm.c

CFLAGS += -DUIP_CONF_IPV6_RPL=1

# linker optimizations
SMALL=1

# REST Engine shall use Erbium CoAP implementation
APPS += er-coap
APPS += rest-engine
APPS += json json-resource

include $(CONTIKI)/Makefile.include

$(CONTIKI)/tools/tunslip6:      $(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router: $(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja:   $(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 aaaa::1/64

connect-minimal:
	sudo ip address add fdfd::1/64 dev tap0

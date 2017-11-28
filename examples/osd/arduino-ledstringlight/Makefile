# Set this to the name of your sketch (without extension .pde)
SKETCH=sketch
EXE=arduino-example

all: $(EXE)

CONTIKI=../../..

# Contiki IPv6 configuration
CONTIKI_WITH_IPV6 = 1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PROJECT_SOURCEFILES += resource_led_pwm.c ${SKETCH}.cpp

# variable for Makefile.include
ifneq ($(TARGET), minimal-net)
CFLAGS += -DUIP_CONF_IPV6_RPL=1
else
# minimal-net does not support RPL under Linux and is mostly used to test CoAP only
${info INFO: compiling without RPL}
CFLAGS += -DUIP_CONF_IPV6_RPL=0
CFLAGS += -DHARD_CODED_ADDRESS=\"fdfd::10\"
${info INFO: compiling with large buffers}
CFLAGS += -DUIP_CONF_BUFFER_SIZE=2048
CFLAGS += -DREST_MAX_CHUNK_SIZE=1024
CFLAGS += -DCOAP_MAX_HEADER_SIZE=640
endif

# linker optimizations
SMALL=1

# REST Engine shall use Erbium CoAP implementation
APPS += er-coap
APPS += rest-engine
APPS += time json arduino json-resource

include $(CONTIKI)/Makefile.include
include $(CONTIKI)/apps/arduino/Makefile.include

$(CONTIKI)/tools/tunslip6:      $(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router: $(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja:   $(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 aaaa::1/64

connect-minimal:
	sudo ip address add fdfd::1/64 dev tap0

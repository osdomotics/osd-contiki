EXE=wallclock

all: $(EXE)

CONTIKI=../../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# automatically build RESTful resources
REST_RESOURCES_DIR = ./resources
REST_RESOURCES_DIR_COMMON = ../resources-common
REST_RESOURCES_FILES= $(notdir \
    $(shell find $(REST_RESOURCES_DIR_COMMON) -name '*.c') \
    )

PROJECTDIRS += $(REST_RESOURCES_DIR) $(REST_RESOURCES_DIR_COMMON)
PROJECT_SOURCEFILES += $(REST_RESOURCES_FILES)

# linker optimizations
SMALL=1

# REST Engine shall use Erbium CoAP implementation
APPS += er-coap
APPS += rest-engine
APPS += json json-resource time

CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include

$(CONTIKI)/tools/tunslip6: $(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router: $(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja: $(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 -p 60001 aaaa::1/64

connect-minimal:
	sudo ip address add fdfd::1/64 dev tap0

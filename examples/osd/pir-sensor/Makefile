EXE=er-example-server

all: $(EXE)

CONTIKI=../../..
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# linker optimizations
SMALL=1

APPS += er-coap rest-engine json

include $(CONTIKI)/Makefile.include

# border router rules
$(CONTIKI)/tools/tunslip6:	$(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 aaaa::1/64

connect-minimal:
	sudo ip address add fdfd::1/64 dev tap0

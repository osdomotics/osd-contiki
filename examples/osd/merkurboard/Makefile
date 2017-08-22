EXE=er-example-server er-example-client

all: $(EXE)

CONTIKI=../../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# automatically build RESTful resources including common resources
REST_RESOURCES_DIR = ./resources
REST_RESOURCES_DIR_COMMON = ../resources-common
REST_RESOURCES_FILES = $(notdir \
    $(shell find $(REST_RESOURCES_DIR) -name '*.c') \
    $(shell find $(REST_RESOURCES_DIR_COMMON) -name '*.c') \
    )

PROJECTDIRS += $(REST_RESOURCES_DIR) $(REST_RESOURCES_DIR_COMMON)
PROJECT_SOURCEFILES += $(REST_RESOURCES_FILES)

# linker optimizations
SMALL=1

# REST Engine shall use Erbium CoAP implementation
APPS += er-coap
APPS += rest-engine

# optional rules to get assembly
#CUSTOM_RULE_C_TO_OBJECTDIR_O = 1
#CUSTOM_RULE_S_TO_OBJECTDIR_O = 1

CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include

# minimal-net target is currently broken in Contiki
ifeq ($(TARGET), minimal-net)
CFLAGS += -DHARD_CODED_ADDRESS=\"fdfd::10\"
${info INFO: er-example compiling with large buffers}
CFLAGS += -DUIP_CONF_BUFFER_SIZE=1300
CFLAGS += -DREST_MAX_CHUNK_SIZE=1024
CFLAGS += -DCOAP_MAX_HEADER_SIZE=176
CONTIKI_WITH_RPL=0
endif

# optional rules to get assembly
#$(OBJECTDIR)/%.o: asmdir/%.S
#	$(CC) $(CFLAGS) -MMD -c $< -o $@
#	@$(FINALIZE_DEPENDENCY)
#
#asmdir/%.S: %.c
#	$(CC) $(CFLAGS) -MMD -S $< -o $@

# border router rules
$(CONTIKI)/tools/tunslip6:	$(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 -p 60001 aaaa::1/64

connect-router-native:	$(CONTIKI)/examples/ipv6/native-border-router/border-router.native
	sudo $(CONTIKI)/exmples/ipv6/native-border-router/border-router.native -a 127.0.0.1 -p 60001 aaaa::1/64

connect-minimal:
	sudo ip address add fdfd::1/64 dev tap0 

avr-size-server: er-example-server.$(TARGET)$(CUSTOM_SUFFIX).sz

avr-size-client: er-example-server.$(TARGET)$(CUSTOM_SUFFIX).sz

flash-server: er-example-server.$(TARGET)$(CUSTOM_SUFFIX).u \
    er-example-server.$(TARGET)$(CUSTOM_SUFFIX).eu

flash-client: er-example-client.$(TARGET)$(CUSTOM_SUFFIX).u \
    er-example-client.$(TARGET)$(CUSTOM_SUFFIX).eu

.PHONY: flash-client avr-size-client flash-server avr-size-server
.PRECIOUS: er-example-server.$(TARGET)$(CUSTOM_SUFFIX).hex \
    er-example-server.$(TARGET)$(CUSTOM_SUFFIX).eep \
    er-example-client.$(TARGET)$(CUSTOM_SUFFIX).hex \
    er-example-client.$(TARGET)$(CUSTOM_SUFFIX).eep


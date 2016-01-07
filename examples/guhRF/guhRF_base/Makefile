ifeq ($(TARGET), guhRF)
PLATFORM_FILES= avr-size-server er-example-server.guhRF.hex \
    er-example-server.guhRF.eep                             \

endif


all: er-example-server $(PLATFORM_FILES)

# use target "er-plugtest-server" explicitly when requried 

CONTIKI=../../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# automatically build RESTful resources including common resources
ifndef TARGET
    REST_EXCLUDE=
else
ifeq ($(TARGET), native)
    REST_EXCLUDE=
else
    REST_EXCLUDE= ! -name 'res-plugtest*'
endif
endif
REST_RESOURCES_DIR = ./resources
REST_RESOURCES_DIR_COMMON = ../resources-common
REST_RESOURCES_FILES = $(notdir \
    $(shell find $(REST_RESOURCES_DIR) -name '*.c' $(REST_EXCLUDE)) \
    $(shell find $(REST_RESOURCES_DIR_COMMON) -name '*.c') \
    )

PROJECTDIRS += $(REST_RESOURCES_DIR) $(REST_RESOURCES_DIR_COMMON)
PROJECT_SOURCEFILES += $(REST_RESOURCES_FILES)

# linker optimizations
SMALL=1

# REST Engine shall use Erbium CoAP implementation
APPS += er-coap
APPS += rest-engine
#APPS += er-coap-13
#APPS += erbium


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


#-----------------------------------------------------------------------
#			guh is good for you
#------------------------------------------------------------------------
avr-size-server: er-example-server.guhRF
	avr-size -C --mcu=MCU=atmega256rfr2 er-example-server.guhRF

er-example-server.guhRF.hex: er-example-server.guhRF
	avr-objcopy -j .text -j .data -O ihex er-example-server.guhRF \
	    er-example-server.guhRF.hex

er-example-server.guhRF.eep: er-example-server.guhRF
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	    --change-section-lma .eeprom=0 -O ihex                      \
	    er-example-server.guhRF er-example-server.guhRF.eep

flash-server: er-example-server.guhRF.hex er-example-server.guhRF.eep
	avrdude -pm256rfr2 -cstk500 -b57600 -P/dev/ttyUSB0 -D -U \
	    flash:w:er-example-server.guhRF.hex:a -U \
	    eeprom:w:er-example-server.guhRF.eep:a

flash-server-jtag: er-example-server.guhRF.hex er-example-server.guhRF.eep
	avrdude -pm256rfr2 -catmelice -P usb -U \
	    flash:w:er-example-server.guhRF.hex:a -U \
	    eeprom:w:er-example-server.guhRF.eep:a

avr-size-client: er-example-client.guhRF
	avr-size -C --mcu=MCU=atmega256rfr2 er-example-client.guhRF 

er-example-client.guhRF.hex: er-example-client.guhRF
	avr-objcopy -j .text -j .data -O ihex er-example-client.guhRF \
	    er-example-client.guhRF.hex

er-example-client.guhRF.eep: er-example-client.guhRF
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	    --change-section-lma .eeprom=0 -O ihex                      \
	    er-example-client.guhRF er-example-client.guhRF.eep

flash-client: er-example-client.guhRF.hex -b57600 er-example-client.guhRF.eep
	avrdude -pm256rfr2 -c arduino -P/dev/ttyUSB0 -e -U \
	    flash:w:er-example-client.guhRF.hex:a -U \
	    eeprom:w:er-example-client.guhRF.eep:a


.PHONY: flash-client avr-size-client flash-server avr-size-server

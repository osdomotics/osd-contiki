# Set this to the name of your sketch (without extension .pde)
SKETCH=sketch
EXE=ota


all: $(EXE)

CONTIKI=../../..

# Contiki IPv6 configuration
CONTIKI_WITH_IPV6 = 1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PROJECT_SOURCEFILES += res_upload_image.c res_bootloader.c ${SKETCH}.cpp

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
APPS += arduino json-resource json #time

include $(CONTIKI)/Makefile.include
include $(CONTIKI)/apps/arduino/Makefile.include

avr-size: $(EXE).$(TARGET).sz

flash: $(EXE).$(TARGET).u # $(EXE).$(TARGET).eu

.PHONY: flash avr-size
.PRECIOUS: $(EXE).$(TARGET).hex $(EXE).$(TARGET).eep

AVRDUDE_PORT=/dev/ttyUSB1

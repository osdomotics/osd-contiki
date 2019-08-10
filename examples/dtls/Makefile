CONTIKI=../..
CONTIKI_WITH_IPV4 = 0
CONTIKI_WITH_IPV6 = 1
CONTIKI_WITH_RPL = 0
CONTIKI_WITH_WOLFSSL = 1
CFLAGS+=-DWOLFSSL_USER_SETTINGS

ifeq ($(MAKECMDGOALS),)
$(error Please specify whether dtls-client or dtls-server should be built)
endif

CONTIKI_PROJECT=$(MAKECMDGOALS)
all: $(CONTIKI_PROJECT)


APPS=wolfssl time

PROJECT_SOURCEFILES += CA.c server.c client.c

include $(CONTIKI)/Makefile.include

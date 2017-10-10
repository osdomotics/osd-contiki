EXE=sensniff
all: $(EXE)

CONTIKI = ../../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PROJECT_SOURCEFILES += sensniff-rdc.c netstack.c
PROJECTDIRS += pool $(TARGET)

ifeq ($(TARGET),)
  -include Makefile.target
  ifeq ($(TARGET),)
    TARGET=srf06-cc26xx
    $(info TARGET not defined, using target $(TARGET))
  endif
endif

CONTIKI_WITH_IPV6 = 1

include $(CONTIKI)/Makefile.include

PYTHON ?= python
SENSNIFF = $(CONTIKI)/tools/sensniff/sensniff.py

ifeq ($(BAUDRATE),)
  BAUDRATE = 460800
endif

SENSNIFF_FLAGS += -b $(BAUDRATE)

ifneq ($(PORT),)
  SENSNIFF_FLAGS += -d $(PORT)
endif

sniff:
ifeq ($(wildcard $(SENSNIFF)), )
	$(error Could not find the sensniff script. Did you run 'git submodule update --init' ?")
else
	$(PYTHON) $(SENSNIFF) $(SENSNIFF_FLAGS)
endif

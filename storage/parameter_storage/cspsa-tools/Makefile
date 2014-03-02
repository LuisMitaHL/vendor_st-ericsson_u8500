#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2010 ST-Ericsson SA                                   *
# *                                                                      *
# *  This software is released either under the terms of the BSD-style   *
# *  license accompanying CSPSA or a proprietary license obtained from   *
# *  ST-Ericsson SA.                                                     *
# *                                                                      *
# *  Author: Martin Lundholm <martin.xa.lundholm@stericsson.com>         *
# *                                                                      *
# ************************************************************************/

CSPSADIR = ../cspsa
ifeq ($(O),)
	BUILDDIR :=
	CSPSALIBDIR := $(abspath .)
else
	BUILDDIR := $(O)/
	CSPSALIBDIR := $(abspath $(BUILDDIR))
endif

ifndef CC
	CC = gcc
endif
ifndef LD
	LD = gcc
endif

ifeq ($(V),1)
	#Verbose usage when V=1
	COMPILE = $(CC)
	LINK = $(CC)
	CP = cp
	SED = sed
else
	COMPILE = @echo "cc $<" && $(CC)
	LINK = @echo "ld $^" && $(CC)
	CP = @cp
	SED = @sed
endif

CFLAGS := $(CFLAGS) -g -Wall -Wextra -Werror -D LINUX
# -D LINUX needed for qstore utils

DEFINES = -D_XOPEN_SOURCE=500 -D_BSD_SOURCE=1
INCLUDES = -Iose_simulate/ -I$(CSPSADIR) -Iqtools_src/

LDFLAGS = -g

DEPCP = $(CP)
DEPSED = $(SED)
DEPFLAGS = -Wp,-MD,$*.d

ifndef DISABLE_NWM_TARGET_INIT_AGENT
CSPSA2NWM_TRGT = cspsa2nwm
endif

ifndef DISABLE_QSTORE_TARGET_INIT_AGENT
CSPSA2MAP_TRGT = cspsa2map
endif

TARGETS = $(CSPSA2NWM_TRGT) $(CSPSA2MAP_TRGT) gdflist map2q q2map nwm2gdf \
		gdf2pff pff2cspsa cspsalist map2cspsa
TARGETS := $(TARGETS:%=$(BUILDDIR)%)

TARGETS_HOST = $(CSPSA2NWM_TRGT) nwm2gdf gdflist gdf2pff pff2cspsa cspsalist
TARGETS_HOST := $(TARGETS_HOST:%=$(BUILDDIR)%)

TARGETS_TARGET = $(CSPSA2NWM_TRGT) $(CSPSA2MAP_TRGT) map2q q2map map2cspsa
TARGETS_TARGET := $(TARGETS_TARGET:%=$(BUILDDIR)%)

OBJS = nwm2gdf.o nwmcommon.o gdflist.o qtools_src/map2q.o qtools_src/q2map.o \
		qtools_src/getarg.o nwm2gdf.o gdf2pff.o cspsa2nwm.o \
		pff2cspsa.o cspsalist.o cspsa2map.o map2cspsa.o
OBJS := $(OBJS:%=$(BUILDDIR)%)

.PHONY: all target host $(TARGETS:%=b-%)
all: $(TARGETS)

host: $(TARGETS_HOST)

target: $(TARGETS_TARGET)

b-gdflist: $(BUILDDIR)gdflist
$(BUILDDIR)gdflist: $(BUILDDIR)gdflist.o
	$(LINK) $(LDFLAGS) -o $@ $^

b-map2q: $(BUILDDIR)map2q
$(BUILDDIR)map2q: $(BUILDDIR)qtools_src/map2q.o $(BUILDDIR)qtools_src/getarg.o
	$(LINK) $(LDFLAGS) -o $@ $^

b-q2map: $(BUILDDIR)q2map
$(BUILDDIR)q2map: $(BUILDDIR)qtools_src/q2map.o
	$(LINK) $(LDFLAGS) -o $@ $^

b-nwm2gdf: $(BUILDDIR)nwm2gdf
$(BUILDDIR)nwm2gdf: $(BUILDDIR)nwm2gdf.o $(BUILDDIR)nwmcommon.o
	$(LINK) $(LDFLAGS) -o $@ $^

b-gdf2pff: $(BUILDDIR)gdf2pff
$(BUILDDIR)gdf2pff: $(BUILDDIR)gdf2pff.o
	$(LINK) $(LDFLAGS) -o $@ $^

# - The following targets require folder 'cspsa' on same file hierachy level
#   as current Makefile
b-cspsa2nwm: $(BUILDDIR)cspsa2nwm
$(BUILDDIR)cspsa2nwm: $(BUILDDIR)cspsa2nwm.o
	if test -d $(CSPSADIR); then $(MAKE) O=$(CSPSALIBDIR) --directory=$(CSPSADIR) -f Makefile libs; fi;
	$(LINK) $(LDFLAGS) -L$(CSPSALIBDIR) -o $@ $^ -lcspsa

b-pff2cspsa: $(BUILDDIR)pff2cspsa
$(BUILDDIR)pff2cspsa: $(BUILDDIR)pff2cspsa.o
	if test -d $(CSPSADIR); then $(MAKE) O=$(CSPSALIBDIR) --directory=$(CSPSADIR) -f Makefile libs; fi;
	$(LINK) $(LDFLAGS) -L$(CSPSALIBDIR) -o $@ $^ -lcspsa-core -lcspsa-ll

b-cspsalist: $(BUILDDIR)cspsalist
$(BUILDDIR)cspsalist: $(BUILDDIR)cspsalist.o
	if test -d $(CSPSADIR); then $(MAKE) O=$(CSPSALIBDIR) --directory=$(CSPSADIR) -f Makefile libs; fi;
	$(LINK) $(LDFLAGS) -L$(CSPSALIBDIR) -o $@ $^ -lcspsa-core -lcspsa-ll

b-cspsa2map: $(BUILDDIR)cspsa2map
$(BUILDDIR)cspsa2map: $(BUILDDIR)cspsa2map.o
	if test -d $(CSPSADIR); then $(MAKE) O=$(CSPSALIBDIR) --directory=$(CSPSADIR) -f Makefile libs; fi;
	$(LINK) $(LDFLAGS) -L$(CSPSALIBDIR) -o $@ $^ -lcspsa

b-map2cspsa: $(BUILDDIR)map2cspsa
$(BUILDDIR)map2cspsa: $(BUILDDIR)map2cspsa.o $(BUILDDIR)qtools_src/getarg.o
	if test -d $(CSPSADIR); then $(MAKE) O=$(CSPSALIBDIR) --directory=$(CSPSADIR) -f Makefile libs; fi;
	$(LINK) $(LDFLAGS) -L$(CSPSALIBDIR) -o $@ $^ -lcspsa

$(BUILDDIR)%.o: %.c
	[ ! -d $(dir $@) ] && mkdir -p $(dir $@) || true
	$(COMPILE) $(CFLAGS) $(DEPFLAGS) $(DEFINES) $(INCLUDES) -o $@ -c $<
	$(DEPCP) $*.d $*.P
	$(DEPSED) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P

.PHONY: distclean clean
distclean clean:
	-rm -f $(TARGETS) $(OBJS) $(OBJS:.o=.d) $(OBJS:.o=.P)
	$(MAKE) -C $(CSPSADIR) O=$(CSPSALIBDIR) clean

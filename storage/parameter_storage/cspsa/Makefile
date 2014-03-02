#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2010 ST-Ericsson                                      *
# *                                                                      *
# *  Author: Joakim AXELSSON <jokaim.axelsson AT stericsson.com>         *
# *                                                                      *
# ************************************************************************/

#Make sure a compiler and an archive tool is selected
ifndef CC
	CC = gcc
endif
ifndef AR
	AR = ar
endif

# Tools used. Use 'make V=1 target' to print true tool cmdline
ifeq ($(V),1)
	#Verbose usage when V=1
	COMPILE = $(CC)
	LINK = $(CC)
	ARCHIVE = $(AR)
	LN = ln
	CP = cp
	RM = rm
	SED = sed
else
	#None verbose output. Hide most of the output
	COMPILE = @echo "cc $<" && $(CC)
	LINK = @echo "ld $@" && $(CC)
	ARCHIVE = @echo "ar $@" && $(AR)
	LN = ln
	CP = @cp
	RM = rm
	SED = @sed
endif

ifeq ($(O),)
	BUILDDIR :=
else
	BUILDDIR := $(O)/
endif
$(info BUILDDIR=$(BUILDDIR))

#Common flags
INC =
#-fno-short-enums is needed by Loaders
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -fno-short-enums -DCFG_CSPSA_LBP=1
DEFINES =
LDFLAGS =
ARFLAGS = -cqs

#Dependency
DEPCP = $(CP)
DEPSED = $(SED)
DEPFLAGS = -Wp,-MD,$(BUILDDIR)$*.d

prefix ?= /usr/local/
sbindir ?= ${prefix}/sbin
sysconfdir ?= ${prefix}/etc
libdir ?= ${prefix}/lib

#If BUILD isn't selected, we assume debug
ifeq ($(BUILD),)
BUILD=release
endif

#DEBUG compile
ifeq ($(BUILD),debug)
CFLAGS += -g
DEFINES += -DDEBUG
LDFLAGS += -g
endif

#RELEASE compile
ifeq ($(BUILD),release)
CFLAGS += -O3 -ffunction-sections -fdata-sections -g
DEFINES += -UDEBUG -DNDEBUG
LDFLAGS += -Wl,--gc-sections -g
#-ffunction-sections -fdata-sections will section the functions and data during compilation
#-Wl,--gc-sections will remove the sections not used during linking
endif

#Added needed flags for gcov. Use it with "make GCOV=1 all"
GCOV_LDFLAGS = -lgcov -fprofile-arcs
GCOV_CFLAGS = -fprofile-arcs -ftest-coverage
ifeq ($(GCOV),1)
	CFLAGS += $(GCOV_CFLAGS)
	LDFLAGS += $(GCOV_LDFLAGS)
endif


#This is the first target, it will be execute if no target is given to 'make'.
.PHONY: default
default: libs exe


# CSPSA socket client implementation.
# -----
SRC_CLIENT = cspsa_api_linux_socket_common.c cspsa_api_linux_socket_client.c
OBJ_CLIENT = $(SRC_CLIENT:%.c=$(BUILDDIR)%.o)
$(OBJ_CLIENT): SHARED_LIB_FLAGS=-fPIC

# Libraries of functions.
# ===========================================

# Core CSPSA functions, as static archive lib. Doesn't depend on OS.
# -----
LIBCORE = $(BUILDDIR)libcspsa-core.a
SRC_CORE = cspsa_core.c crc32.c cspsa_ll.c
OBJ_CORE = $(SRC_CORE:%.c=$(BUILDDIR)%.o)
$(LIBCORE): $(OBJ_CORE)
	$(ARCHIVE) $(ARFLAGS) $@ $^

# Static client library for Linux (UNIX) enviroment.
# -----
LIBCLIENTSTA = $(BUILDDIR)libcspsa.a
$(LIBCLIENTSTA): $(OBJ_CLIENT)
	$(ARCHIVE) $(ARFLAGS) $@ $^

# Shared client library for Linux (UNIX) enviroment.
# -----
LIBCLIENTDYN = $(BUILDDIR)libcspsa.so
LIBCLIENTDYN_VMAJ = 1
LIBCLIENTDYN_VMIN = 0
$(LIBCLIENTDYN): $(OBJ_CLIENT)
	$(COMPILE)  $(DEPFLAGS) $(INC) $(CFLAGS) $(DEFINES) -shared -Wl,-soname,$(notdir $@).$(LIBCLIENTDYN_VMAJ) -o $@.$(LIBCLIENTDYN_VMAJ).$(LIBCLIENTDYN_VMIN) $(OBJ_CLIENT)
	ln -sf $(notdir $@).$(LIBCLIENTDYN_VMAJ).$(LIBCLIENTDYN_VMIN) $@.$(LIBCLIENTDYN_VMAJ)
	ln -sf $(notdir $@).$(LIBCLIENTDYN_VMAJ).$(LIBCLIENTDYN_VMIN) $@

# Static library of low level functions.
LIBLL = $(BUILDDIR)libcspsa-ll.a
SRC_LL = cspsa_ll_ram.c cspsa_ll_file.c cspsa_ll_dev.c cspsa_ll_mmc.c
OBJ_LL = $(SRC_LL:%.c=$(BUILDDIR)%.o)
$(LIBLL): $(OBJ_LL)
	$(ARCHIVE) $(ARFLAGS) $@ $^

# OSFree target.
# -----
#Will create an archive of object files, which loaders and ITP can use.
LIBOSFREE = $(BUILDDIR)libcspsa-osfree.a
SRC_OSFREE = cspsa_api_osfree.c cspsa_core.c crc32.c cspsa_ll.c
OBJ_OSFREE = $(SRC_OSFREE:%.c=$(BUILDDIR)%.o)
libcspsa-osfree: $(LIBOSFREE)
$(LIBOSFREE): $(OBJ_OSFREE)
	$(ARCHIVE) $(ARFLAGS) $@ $^

#Helper target to build all lib targets
ALL_LIBS = $(LIBCORE) $(LIBCLIENTSTA) $(LIBCLIENTDYN) $(LIBOSFREE) $(LIBLL)
.PHONY: libs
libs: $(ALL_LIBS)

# Final executable targets
# ==========================

# Main Linux (UNIX) CSPSA userspace server
# -----
SRC_SERVER = cspsa_api_linux_socket_server.c cspsa_server.c config_file_reader.c
OBJ_SERVER = $(SRC_SERVER:%.c=$(BUILDDIR)%.o)

# The server actually needs to use parts of the client libcspsa.a as well
$(BUILDDIR)cspsa-server: $(OBJ_SERVER) $(OBJ_LL) $(OBJ_CORE) $(OBJ_CLIENT)
	$(LINK) $(LDFLAGS) -o $@ $^


# Clients using libcspsa.a in Linux userspace
# ----

SRC_TEST_CMD = cspsa_test_client_cmd.c
OBJ_TEST_CMD = $(SRC_TEST_CMD:%.c=$(BUILDDIR)%.o)
$(BUILDDIR)cspsa-cmd: $(OBJ_TEST_CMD) $(OBJ_CLIENT)
	$(LINK) $(LDFLAGS) -o $@ $^

#Build all executable targets
ALL_EXES = $(BUILDDIR)cspsa-cmd $(BUILDDIR)cspsa-server
.PHONY: exe
exe: $(ALL_EXES)

# Test programs
# ----

SRC_TEST_READER = cspsa_test_client_reader.c
OBJ_TEST_READER = $(SRC_TEST_READER:%.c=$(BUILDDIR)%.o)
$(BUILDDIR)cspsa-test-reader: $(OBJ_TEST_READER) $(OBJ_CLIENT)
	$(LINK) $(LDFLAGS) -o $@ $^

SRC_TEST_WRITER = cspsa_test_client_writer.c
OBJ_TEST_WRITER = $(SRC_TEST_WRITER:%.c=$(BUILDDIR)%.o)
$(BUILDDIR)cspsa-test-writer: $(OBJ_TEST_WRITER) $(OBJ_CLIENT)
	$(LINK) $(LDFLAGS) -o $@ $^


#TODO: Add the targets test_api and test_core. But it is compilicated as they
# use the same source files, only compiled with difference -DDEFINEs.
# They should be fixed to either use different source files, or take command
# line options on start.

#Build regression test for CSPSA core using RAM as storage media.
TEST_CORE_RAM_DEFINES = -DCFG_CSPSA_TEST_TARGET=CFG_CSPSA_TEST_TARGET_LINUX_US -DCFG_CSPSA_TEST_VARIANT=CFG_CSPSA_TEST_VARIANT_CORE
TEST_CORE_RAM_DEFINES += -DCFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE=512 -DCFG_CSPSA_CORE_CACHE_INITIAL_NBR_OF_IMAGE_ARRAYS=1
TEST_CORE_RAM_DEFINES += -DCFG_CSPSA_TEST_MEMORY_TYPE=T_CSPSA_MEMORY_TYPE_RAM -DCFG_CSPSA_TEST_READ_ERRORS
$(BUILDDIR)cspsa-test-core-ram:
	$(COMPILE) $(INC) $(CFLAGS) -c $(DEFINES) $(TEST_CORE_RAM_DEFINES) $(SRC_CORE) $(SRC_LL) cspsa_api_linux_us_core_test.c cspsa_test_ram.c cspsa_test.c
	$(LINK) $(LDFLAGS) -o $@ $^ $(OBJ_CORE) $(OBJ_LL) cspsa_api_linux_us_core_test.o cspsa_test_ram.o cspsa_test.o

#Build regression test for CSPSA core using MMC as storage media.
TEST_CORE_MMC_DEFINES = -DCFG_CSPSA_TEST_TARGET=CFG_CSPSA_TEST_TARGET_LINUX_US -DCFG_CSPSA_TEST_VARIANT=CFG_CSPSA_TEST_VARIANT_CORE
TEST_CORE_MMC_DEFINES += -DCFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE=512 -DCFG_CSPSA_CORE_CACHE_INITIAL_NBR_OF_IMAGE_ARRAYS=1
TEST_CORE_MMC_DEFINES += -DCFG_CSPSA_TEST_MEMORY_TYPE=T_CSPSA_MEMORY_TYPE_DEV_MMC -DCFG_CSPSA_TEST_READ_ERRORS
$(BUILDDIR)cspsa-test-core-mmc:
	$(COMPILE) $(INC) $(CFLAGS) -c $(DEFINES) $(TEST_CORE_MMC_DEFINES) $(SRC_CORE) $(SRC_LL) cspsa_api_linux_us_core_test.c cspsa_test_ram.c cspsa_test.c
	$(LINK) $(LDFLAGS) -o $@ $(OBJ_CORE) $(OBJ_LL) cspsa_api_linux_us_core_test.o cspsa_test_ram.o cspsa_test.o

#Build regression test for CSPSA API using RAM as storage media.
TEST_API_RAM_LIBS := -lpthread
TEST_API_RAM_DEFINES = -DCFG_CSPSA_TEST_TARGET=CFG_CSPSA_TEST_TARGET_LINUX_US -DCFG_CSPSA_TEST_VARIANT=CFG_CSPSA_TEST_VARIANT_API
TEST_API_RAM_DEFINES += -DCFG_CSPSA_TEST_MEMORY_TYPE=T_CSPSA_MEMORY_TYPE_RAM
$(BUILDDIR)cspsa-test-api-ram:
	$(COMPILE) $(INC) $(CFLAGS) -c $(DEFINES) $(TEST_API_RAM_DEFINES) $(SRC_CORE) $(SRC_LL) cspsa_api_linux_socket_server.c config_file_reader.c cspsa_test.c
	$(LINK) $(LDFLAGS) $(TEST_API_RAM_LIBS) -o $@ $(OBJ_CORE) $(OBJ_CLIENT) $(OBJ_LL) cspsa_api_linux_socket_server.o config_file_reader.o cspsa_test.o

#Build regression test for CSPSA API using MMC as storage media.
TEST_API_MMC_LIBS := -lpthread
TEST_API_MMC_DEFINES = -DCFG_CSPSA_TEST_TARGET=CFG_CSPSA_TEST_TARGET_LINUX_US -DCFG_CSPSA_TEST_VARIANT=CFG_CSPSA_TEST_VARIANT_API
TEST_API_MMC_DEFINES += -DCFG_CSPSA_TEST_MEMORY_TYPE=T_CSPSA_MEMORY_TYPE_DEV_MMC
$(BUILDDIR)cspsa-test-api-mmc:
	$(COMPILE) $(INC) $(CFLAGS) -c $(DEFINES) $(TEST_API_MMC_DEFINES) $(SRC_CORE) $(SRC_LL) cspsa_api_linux_socket_server.c config_file_reader.c cspsa_test.c
	$(LINK) $(LDFLAGS) $(TEST_API_MMC_LIBS) -o $@ $(OBJ_CORE) $(OBJ_CLIENT) $(OBJ_LL) cspsa_api_linux_socket_server.o config_file_reader.o cspsa_test.o

#Build all test targets
ALL_TESTS = $(BUILDDIR)cspsa-test-reader $(BUILDDIR)cspsa-test-writer $(BUILDDIR)cspsa-test-core-ram $(BUILDDIR)cspsa-test-api-ram $(BUILDDIR)cspsa-test-core-mmc $(BUILDDIR)cspsa-test-api-mmc
.PHONY: test
test: $(ALL_TESTS)


#Target for compiling individual files
#-----------------------------------------

#We include all dependecy .deps/file.P -files (those that exist,
#we are using "-include" not "include").
-include $(BUILDDIR)*.P

#Normal compile target. It will compile a file from file.c to file.o
#It will also create the dependency file n .deps/file.d
#(using -Wp,-MD,$depout).d. It will be used on next compile to figure if
#file.c needs to be compiled (if any .h files it depends on has changed)
#It will copy the file.d into a .P file, and add all depenency as targets.
#This way, if a .h files disapears, the makefile won't complain
#that a dependency is missing.
$(BUILDDIR)%.o: %.c Makefile $(BUILDDIR)
	$(COMPILE) $(DEPFLAGS) $(INC) $(CFLAGS) $(DEFINES) $(SHARED_LIB_FLAGS) -c -o $@ $<
	$(DEPCP) $(BUILDDIR)$*.d $(BUILDDIR)$*.P
	$(DEPSED) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDDIR)$*.d >> $(BUILDDIR)$*.P

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Install target for MeeGo
# ================
.PHONY: install
install: $(ALL_EXES) $(LIBCLIENTDYN)
	@install -d $(sbindir) $(sysconfdir)/rc.d/init.d $(libdir)
	install $(ALL_EXES) $(sbindir)/
	install $(LIBCLIENTDYN).$(LIBCLIENTDYN_VMAJ).$(LIBCLIENTDYN_VMIN) $(libdir)/
	cp -d $(LIBCLIENTDYN).$(LIBCLIENTDYN_VMAJ) $(libdir)/
	install -m 644 config/cspsa.lbp.conf $(sysconfdir)/cspsa.conf
	install -m 755 cspsa.init $(sysconfdir)/rc.d/init.d/cspsa


# Clean targets
# ==============

.PHONY: clean
clean: clean_gcov
	-$(RM) -f $(BUILDDIR)*.o $(BUILDDIR)*.a $(BUILDDIR)*.d $(BUILDDIR)*.P core $(BUILDDIR)*.so*
	-$(RM) -f $(ALL_LIBS) $(ALL_EXES) $(ALL_TESTS)

clean_gcov:
	-$(RM) -f *.gc*

# All targets
# ==============
.PHONY: all
all: libs test exe

#Simple target to force a rebuild of everything
.PHONY: rebuild
rebuild: clean all


# Documentation
# ==============
.PHONY: doc
doc:
	asciidoc cspsa_ds.txt
	asciidoc cspsa_ts.txt


# Other code checking tools
# =================================
.PHONY: check cppcheck clang pmccabe gcov

check: pmccabe cppcheck clang

#Outputs an analyzis of which function are the biggest/most complex ones
#The first pmccabe will print the header of the columns
pmccabe:
	pmccabe -v -c /dev/null; pmccabe -v -c *.c | sort -nr | head -20

cppcheck:
	cppcheck -a -s -v --unused-functions *.c

#TODO: Fix the path for clang tool
clang:
	~esebras/bin/clang -fsyntax-only $(CFLAGS) *.c
	~esebras/bin/clang --analyze $(CFLAGS) *.c


#Coverty test. Compile the output to collect coverty info
gcov:
	$(MAKE) GCOV=1 all #Run our self, but with GCOV flag enabled



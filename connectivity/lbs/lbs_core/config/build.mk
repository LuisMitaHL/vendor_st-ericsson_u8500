##########################################################
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
##########################################################
# build.mk
#

# build module
ifneq ($(MODULE), )
SRCDIR := $($(MODULE)_DIR)
SRCS := $(addprefix $(SRCDIR)/, $($(MODULE)_SRC))
INCLUDES := $($(MODULE)_INCL)
MODNAME := $($(MODULE)_MOD).o
OBJDIR := $($(MODULE)_OBJ)
OUTDIR := output/$(ARCH)
LBS_FLAGS += $($(MODULE)_FLAGS)
$(warning "Building $(MODULE)")
build: build-module
endif

# build shared library
ifneq ($(LIB), )
LIBNAME=lib$($(LIB)_MOD).so
$(warning, "Building $(LIBNAME)")
SRCDIR := $($(LIB)_DIR)
SRCS := $(addprefix $(SRCDIR)/, $($(LIB)_SRC))
INCLUDES := $($(LIB)_INCL)
OBJDIR := $($(LIB)_OBJ)
OUTDIR := lib/$(ARCH)
LBS_FLAGS += $($(LIB)_FLAGS)
build: build-shared-library
install: install-shared-library
endif

# build static library
ifneq ($(STATIC_LIB), )
LIBNAME=lib$($(STATIC_LIB)_MOD).a
$(warning, "Building $(LIBNAME)")
SRCDIR := $($(STATIC_LIB)_DIR)
SRCS := $(addprefix $(SRCDIR)/, $($(STATIC_LIB)_SRC))
INCLUDES := $($(STATIC_LIB)_INCL)
OBJDIR := $($(STATIC_LIB)_OBJ)
OUTDIR := lib/$(ARCH)
LBS_FLAGS += $($(STATIC_LIB)_FLAGS)
build: build-static-library
install: install-static-library
endif

# build executable
ifneq ($(PROG), )
PROGNAME=$(PROG)
$(warning, "Building $(PROGNAME)")
SRCDIR := $($(MOD)_DIR)
SRCS := $(addprefix $(SRCDIR)/, $($(MOD)_SRC))
INCLUDES := $($(MOD)_INCL)
OBJDIR := $($(MOD)_OBJ)
OUTDIR := $(PROGOUT)/bin/$(ARCH)
LBS_FLAGS += $($(MOD)_FLAGS)
build: build-$(PROG)
install: install-prog
endif


# build rules
OBJS = $(patsubst %.c, %.o, $(SRCS))
build-shared-library: BUILD_CFLAGS := -fPIC
%.o: %.c
	@$(CC) $(CFLAGS) $(BUILD_CFLAGS) $(LBS_FLAGS) $(INCLUDES) $(LBS_INCLUDES) -o $@ -c $<

# build targets
build-module: @$(OBJS)
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	@$(LD) $(LDFLAGS) -o $(MODNAME) -r $(OBJS)
	@$(MV) $(OBJS) $(OBJDIR)
	@test -d $(OUTDIR) || mkdir -p $(OUTDIR)
	@$(MV) *.o $(OUTDIR)

build-shared-library: $(OBJS)
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	@test -d $(OUTDIR) || mkdir -p $(OUTDIR)
	@$(CC) -shared -W1,-soname,$(LIBNAME) -o $(OUTDIR)/$(LIBNAME) $(OBJS) $(LBS_LIB_PATH) $(LDFLAGS)
ifeq ($(LBS_UBUNTU),true)
	@$(STRIP) $(OUTDIR)/$(LIBNAME)
endif
	@$(MV) $(OBJS) $(OBJDIR)

install-shared-library: build-shared-library
	$(INSTALL) -D $(LIBNAME) lib/$(LIBNAME)

build-static-library: $(OBJS)
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	@test -d $(OUTDIR) || mkdir -p $(OUTDIR)
	@$(AR) $(ARFLAGS) $(LIBNAME) $(OUTDIR)/$(LIBNAME) $(OBJS) $(LBS_LIB_PATH) $(LDFLAGS)
	@$(MV) $(OBJS) $(OBJDIR)

install-static-library: build-static-library
	$(INSTALL) -D $(LIBNAME) lib/$(LIBNAME)

build-$(PROG): $(OBJS)
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	@test -d $(OUTDIR) || mkdir -p $(OUTDIR)
ifeq ($(LBS_UBUNTU),true)
	$(CC) $(LDFLAGS) $(LBS_LIB_PATH) -o $(OUTDIR)/$(PROGNAME) $(OBJS) $(LDLIBS)
	@$(STRIP) $(OUTDIR)/$(PROGNAME)
else
ifeq ($(UBUNTU_X86),true)
	$(CC) $(LDFLAGS) $(LBS_LIB_PATH) -o $(OUTDIR)/$(PROGNAME) $(OBJS) $(LDLIBS)
else
	$(LD) $(LDFLAGS) $(LBS_LIB_PATH) -o $(OUTDIR)/$(PROGNAME) $(OBJS)
endif
endif
	@$(MV) $(OBJS) $(OBJDIR)

install-prog: build-$(PROG)
	@$(INSTALL) -D $(PROGNAME) $(OUTDIR)/$(PROG)

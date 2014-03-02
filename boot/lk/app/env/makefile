#
# Little Kernel environment creation tool
#
#

ENVCC=gcc

BUILDDIR := $(BOOTLOADER_OUT)

ENVAPPNAME=$(BUILDDIR)/envcreate

ENVOBJS =\
	$(BUILDDIR)/app/env/envcreate.o \
	$(BUILDDIR)/app/env/crc32.o

ENVINCLUDE_DIRS=\
	-I ./platform/ste/shared/include

CCOPTS= \
	-c \
	$(ENVINCLUDE_DIRS)

ENVLINKOPTS=\
	-o $(ENVAPPNAME)


# Build the tool
build:  $(ENVOBJS)
	@echo Linking $(ENVAPPNAME)
	@$(ENVCC) $(ENVOBJS) $(ENVLINKOPTS)
	@$(ENVAPPNAME) $(BUILD_LK_ENV_IMG_INPUT) $(BUILD_LK_ENV_IMG_OUTPUT)

# Execute tool
mkenv: build $(ENVAPPNAME) $(ENVOBJS)
	@$(ENVAPPNAME) $(BUILD_LK_ENV_IMG_INPUT) $(BUILD_LK_ENV_IMG_OUTPUT)

# Compile rule
$(ENVOBJS): $(BUILDDIR)/%.o: $<
	@echo $(BUILDDIR)/%.o : cc $<
	@mkdir -p `dirname $@`
	@$(ENVCC) $(CCOPTS) -o $@ $<

# Dependencies
$(BUILDDIR)/app/env/envcreate.o : ./app/env/envcreate.c
$(BUILDDIR)/app/env/crc32.o     : ./platform/ste/shared/crc32.c

#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2010 ST-Ericsson                                      *
# *                                                                      *
# *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
# *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
# *                                                                      *
# ************************************************************************/

#Make sure a compiler and an archive tool is selected
ifndef CC
	CC = gcc
endif

# Tools used. Use 'make V=1 target' to print true tool cmdline
ifeq ($(V),1)
	#Verbose usage when V=1
	COMPILE = $(CC)
	LINK = $(CC)
	DEP = $(CC)
else
	#None verbose output. Hide most of the output
	COMPILE = @echo "cc $<" && $(CC)
	LINK = @echo "ld $^" && $(CC)
	DEP = @$(CC)
endif

CFLAGS = -Wall -Werror -Wextra -std=gnu99
DEFINES = -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -D_XOPEN_SOURCE=500

# These are the supposedly required compilation options mentioned at
# http://swadvice.lud.stericsson.com/guideline.aspx?nr=6&ver=latest
# Removed -Wunreachable-code as the errors it generates conflicts with
# the flags normally setup for Android. Removed -Wmissing-format-attribute
# since it conflicted with the use of vsnprintf() and its declaration.
# Removed -Wcast-align since it conflicted with fsa packing functions.
CFLAGS_HIGH = -Wall -Wbad-function-cast \
	-Werror-implicit-function-declaration -Wextra -Wfloat-equal \
	-Wformat-nonliteral -Wformat-security -Wformat=2 -Winit-self \
	-Wmissing-declarations -Wmissing-include-dirs \
	-Wmissing-noreturn -Wmissing-prototypes -Wnested-externs -Wpointer-arith \
	-Wshadow -Wstrict-prototypes -Wswitch-default \
	-Wunsafe-loop-optimizations -Wwrite-strings
CFLAGS_MEDIUM = -Waggregate-return -Wlarger-than-65500 -Wredundant-decls \
	-Wswitch-enum -fno-common
# Removed -Wpacked -pedantic -std=c99 since they give excessive errors
CFLAGS_LOW = -Winline -Wno-missing-field-initializers -Wno-unused-parameter \
	-Wold-style-definition -Wstrict-aliasing=2 -Wundef

CFLAGS += $(CFLAGS_HIGH) $(CFLAGS_MEDIUM) $(CFLAGS_LOW)

DEPSRCSFLAGS = -M -MM
DEPTAGSFLAGS = -M

LDFLAGS =

prefix ?= /usr/local/
sbindir ?= ${prefix}/sbin
sysconfdir ?= ${prefix}/etc

ifeq ($(BUILD),)
BUILD=debug
endif

ifeq ($(BUILD),debug)
CFLAGS += -g
DEFINES += -DDEBUG
LDFLAGS += -g
endif

ifeq ($(BUILD),release)
CFLAGS += -O2 -ffunction-sections -fdata-sections
DEFINES +=
LDFLAGS += -Wl,--gc-sections
#-ffunction-sections -fdata-sections will section the functions and data during compilation
#-Wl,--gc-sections will remove the sections not used during linking
endif

# Exclude check for modem started when building LBP
DEFINES += -DCONFIG_EXCLUDE_WAIT_FOR_MODEM

MSA_TARGET = msa
MSA_SRCS = main.c config.c log.c fsa.c process.c convenience.c posix.c state.c wakelock.c
MSA_OBJS = $(MSA_SRCS:.c=.o)
MSA_DEPS = $(MSA_SRCS:.c=.d)

FUSE_TARGET = msa_fuse
FUSE_SRCS = msa_fuse.c fsa.c posix.c convenience.c log.c testclient.c wakelock.c
FUSE_OBJS = $(FUSE_SRCS:.c=.o)
FUSE_DEPS = $(FUSE_SRCS:.c=.d)

TEST_TARGET = testdir
TEST_SRCS = testdir.c
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_DEPS = $(TEST_SRCS:.c=.d)

.PHONY: all
all: Makefile.dep tags $(MSA_TARGET) $(FUSE_TARGET) $(TEST_TARGET)

.PHONY: release
release:
	make BUILD=release $* distclean all

.PHONY: debug
debug:
	make BUILD=debug $* distclean all

-include Makefile.dep

$(MSA_TARGET): $(MSA_OBJS)
	$(LINK) $(LDFLAGS) -o $@ $^

$(FUSE_TARGET): $(FUSE_OBJS)
	$(LINK) $(LDFLAGS) -lfuse -o $@ $^

$(TEST_TARGET): $(TEST_OBJS)
	$(LINK) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(COMPILE) $(CFLAGS) $(DEFINES) -o $@ -c $<

%.d: %.c
	$(DEP) $(DEPSRCSFLAGS) $(DEFINES) $< | tr -d '\\\n' | sed -e 's/^\(.*\)\.o: \(.*\)$$/\1\.o \1\.d: Makefile \2\n/' > $@
	$(DEP) $(DEPTAGSFLAGS) $(DEFINES) $< | tr -d '\\\n' | sed -e 's/^\(.*\)\.o: \(.*\)$$/tags: \2\n/' >> $@
	@touch -r $< $@

deps: Makefile.dep
Makefile.dep: $(MSA_DEPS) $(FUSE_DEPS) $(TEST_DEPS)
	@echo "computing dependencies..."
	@grep -Hv '^tags:' $^ | cut -d: -f2- | sed -e 's/  */ /g' | sort > $@
	@echo "tags: Makefile `grep -H 'tags:' $^ | cut -d: -f2- | sed -e 's/tags: *//' -e 's/  */ /g' | tr ' ' '\n' | sort | uniq | tr '\n' ' '`" >> $@
	@touch -r Makefile $@

tags: Makefile.dep
	@which ctags > /dev/null && echo "generating tags..." || true
	@which ctags > /dev/null && ctags `cut -d: -f2- Makefile.dep | sed -e 's/Makefile//g'` || true

.PHONY: analyze
analyze:
	@which cppcheck > /dev/null && cppcheck -a -s -v --unused-functions $(MSA_SRCS) || true
	@which clang > /dev/null && clang -fsyntax-only $(DEFINES) $(MSA_SRCS) || true
	@which clang > /dev/null && clang --analyze $(DEFINES) $(MSA_SRCS) || true
	-@rm -f $(MSA_SRCS:.c=.plist)
	@which pmccabe > /dev/null && pmccabe $(MSA_SRCS) | sort -nr | head || true

.PHONY: install
install: $(MSA_TARGET)
	@mkdir -p $(sbindir) $(sysconfdir)/rc.d/init.d
	@install $(MSA_TARGET) $(sbindir)/$(MSA_TARGET)
	@install -m 755 msa.init $(sysconfdir)/rc.d/init.d/msa
	@if [ -e $(FUSE_TARGET) ]; then \
		install $(FUSE_TARGET) $(sbindir)/$(FUSE_TARGET); \
	else \
		echo "Warning: $(FUSE_TARGET) not built."; \
	fi

.PHONY: clean
clean:
	-rm -f $(MSA_TARGET) $(MSA_OBJS) $(MSA_DEPS)
	-rm -f $(FUSE_TARGET) $(FUSE_OBJS) $(FUSE_DEPS)
	-rm -f $(TEST_TARGET) $(TEST_OBJS) $(TEST_DEPS)

.PHONY: distclean
distclean: clean
	-rm -f Makefile.dep tags

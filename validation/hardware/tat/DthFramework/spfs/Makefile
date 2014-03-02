#
#**********************************************************************
#
# Synopsis: SPFS Makefile- Linux Application Build Rules.
#
#**********************************************************************
#

include .config.mk

PACKAGE_NAME=spfs
PACKAGE_EXEC_NAME=9pserver
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).a

SOURCES_LIB= conn.c error.c fcall.c fdconn.c fidpool.c file.c fmt.c np.c poll.c socksrv.c srv.c user.c
SOURCES_LIB:=$(addprefix libspfs/,$(SOURCES_LIB))
EXEC_SOURCES= fs/9pserver.c
HEADERS= $(wildcard include/*.h)
LIB_HEADERS= $(wildcard libspfs/*.h)
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/,$(notdir $(HEADERS)))
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS= $(addprefix $(OUTPUT)/,$(EXEC_SOURCES:.c=.o))
OBJECTS_LIB= $(addprefix $(OUTPUT)/,$(SOURCES_LIB:.c=.o))
BINARIES= $(addprefix $(HATS_BIN)/,$(PACKAGE_EXEC_NAME))
LIBRARIES= $(HATS_SYM)/$(PACKAGE_LIB_NAME)
SYMBOLS_EXEC= $(HATS_SYM)/$(PACKAGE_EXEC_NAME)

SYSNAME:=${shell uname}
SYSNAME!=uname
CFLAGS=-g -I$(HATS_INC)/$(PACKAGE_NAME) -DSYSNAME=$(SYSNAME) -DUSE_EXTERN_FUNCTION=\"libdthsrvhelper.so\"
LDFLAGS+= -ldl
EXEFLAGS+=$(LIBRARIES)

.PHONY: all install clean

all: install

$(HATS_INC)/$(PACKAGE_NAME)/%.h: include/%.h
	mkdir -p $(HATS_INC)/$(PACKAGE_NAME)
	ln -sf $(CURDIR)/$< $@

$(OUTPUT)/fs/%.o: fs/%.c $(HEADERS) $(INCLUDES)
	$(MKDIR) -p $(dir $@)
	$(CC) $< -c -Werror $(CFLAGS) -o $(@)

$(OUTPUT)/libspfs/%.o: libspfs/%.c $(HEADERS) $(LIB_HEADERS) $(INCLUDES)
	$(MKDIR) -p $(dir $@)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(LIBRARIES): $(OBJECTS_LIB)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(AR) rc $@ $^
	$(RANLIB) $@

$(SYMBOLS_EXEC): $(OBJECTS) $(LIBRARIES)
	$(CC) $(OBJECTS) $(LDFLAGS) $(EXEFLAGS) -o $(@)

$(BINARIES): $(SYMBOLS_EXEC)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_BIN)
	$(STRIP) --strip-unneeded $(@)

install: $(INCLUDES) $(LIBRARIES) $(BINARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS_EXEC) $(BINARIES)
	$(RM) -rf $(LIBRARIES) $(HATS_INC)/$(PACKAGE_NAME)

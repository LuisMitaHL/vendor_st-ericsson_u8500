ifdef COPS_PLATFORM_TYPE
export PLATFORM_NAME = $(COPS_PLATFORM_TYPE)
else
export PLATFORM_NAME = 8500
endif

ifndef TA_DIR
$(error TA_DIR must have a defined value that should be set in the makefile \
  that includes this particular makefile. TA_DIR tells where the actual \
  trusted applications are located (ssw-files). \
  For example TA_DIR := ../path/to/ta_dir
endif

TA_BIN := cops_ta.sh
TA_BIN_DIR := $(TA_DIR)

TA_SRC := $(TA_BIN:.sh=.c)
TA_SRC_DIR ?= $(TA_DIR)

TA_SSW := $(wildcard $(TA_BIN_DIR)/*.ssw)

TA_OBJ := $(TA_SRC:.c=.o)
TA_OBJ_DIR ?= $(TA_DIR)

CPPFLAGS += -I./$(TA_DIR)

$(TA_SRC_DIR)/$(TA_SRC): $(TA_SSW)
	@rm -f $@
	@sh $(TA_BIN_DIR)/$(TA_BIN) $@

clean-ta:
	rm -f $(TA_OBJ_DIR)/$(TA_OBJ) $(TA_SRC_DIR)/$(TA_SRC)

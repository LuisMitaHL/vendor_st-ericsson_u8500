#
#***************************************************************
#
# Synopsis: ab8500 register access - Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk
include $(HATS_CONFIG)

ifeq ($(HATSCONF_AB_LIB_8500),y)
VERSION_HW=AB8500
INCLUDE_PATH= . Accessory Charger GPIO MaskInterrupt Regu2 SIM ADC Control Interrupt \
	Miscellanous Regul Audio Gauge Latch PwmOut RTC USB TVOut
else ifeq ($(HATSCONF_AB_LIB_8505),y)
VERSION_HW=AB8505
INCLUDE_PATH= . Accessory Charger GPIO MaskInterrupt Regu2 SIM ADC Control Interrupt \
	Miscellanous Regul Audio Gauge Latch PwmOut RTC USB
else ifeq ($(HATSCONF_AB_LIB_9540),y)
VERSION_HW=AB9540
INCLUDE_PATH= . Accessory Charger GPIO MaskInterrupt Regu2 SIM ADC Control Interrupt \
	Miscellanous Regul Audio Gauge Latch PwmOut RTC USB
else ifeq ($(HATSCONF_AB_LIB_X600),y)
VERSION_HW=ABX600
else
$(error AB library not selected)
endif

ifeq ($(HATSCONF_AB_HW_REVISION_1.0),y)
REVISION=V1
else ifeq ($(HATSCONF_AB_HW_REVISION_2.0),y)
REVISION=V2
else ifeq ($(HATSCONF_AB_HW_REVISION_3.0),y)
REVISION=V3
else
$(error AB revision not selected)
endif

VPATH= $(addprefix $(VERSION_HW)$(REVISION)/, $(INCLUDE_PATH)) $(CURDIR)

PACKAGE_NAME=AB_APE_lowlevel
TARGET_DYN_LIBRARY= lib$(PACKAGE_NAME).so
HEADERS=$(foreach dir,$(VPATH), $(wildcard $(dir)/*.h))
SOURCES=$(foreach dir,$(VPATH), $(wildcard $(dir)/*.c))
OBJECTS= $(addprefix $(OUTPUT)/,$(notdir $(SOURCES:.c=.o)))
SYMBOLS=$(HATS_SYM)/$(TARGET_DYN_LIBRARY)
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
INCLUDES=$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS)))
LIBRARIES=$(HATS_LIB)/$(TARGET_DYN_LIBRARY)

CFLAGS:= -fPIC -I$(HATS_INC)/libab -Wall -Werror

LDFLAGS+= -lab

.PHONY: all install clean

all: install

$(OUTPUT)/%.o : %.c $(HEADERS)
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $@

$(SYMBOLS): $(OBJECTS)
	$(CC)  $(OBJECTS) $(LDFLAGS) $(LIBFLAGS) -o $@

$(LIBRARIES): $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_LIB)
	$(STRIP) --strip-unneeded $@

$(HATS_INC)/$(PACKAGE_NAME)/%.h: %.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf `pwd`/$< $@

install: $(INCLUDES)  $(LIBRARIES)
ifdef VERSION_HW
ifdef REVISION
		$(ECHO) -e "\t ----------------- config $(VERSION_HW)$(REVISION) --------------"
endif
else
	$(ECHO) -e "\t ----- WARNING : NO DEFINE OF AB Version ----"
	$(ECHO) -e "\t ----------- config $(VERSION_HW)$(REVISION) (default) ----------"
endif

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRARIES)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)

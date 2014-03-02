# vim:syntax=make:
# -*- Mode: Makefile -*-

# Parameters passsed to this makefile:
# ====================================
# PARAM_OBJ_DIR: Absolute path of directory that stores intermediate files (.o, etc)

# Ensure that the caller makefile has passed the needed parameters
ifndef PARAM_OBJ_DIR
$(error "Variable PARAM_OBJ_DIR not set!")
endif

ifndef PARAM_REPORTS_DIR
$(error "Variable PARAM_REPORTS_DIR not set!")
endif

ifndef PARAM_XTI_DIR
$(error "Variable PARAM_XTI_DIR not set!")
endif

ifndef PARAM_CC_OPTIONS
$(error "Variable PARAM_CC_OPTIONS not set!")
endif

-include ../makefile.init

# All of the sources file participating in the build are defined here
VPATH = $(subst ./,,$(shell find . -type d))
#C_SRCS = $(subst ./,,$(shell find . -type d -name "*.c"))
#S_SRCS = $(subst ./,,$(shell find . -type d -name "*.s"))

# List of all dependency files (including its absolute path) that would be generated (for both .c and .s files)
C_DEPS = $(addprefix $(PARAM_OBJ_DIR)/,$(subst .c,.d,$(notdir $(shell find . -type f -name "*.c"))))
S_DEPS = $(addprefix $(PARAM_OBJ_DIR)/,$(subst .s,.d,$(notdir $(shell find . -type f -name "*.s"))))

# List of all object files (including its absolute path) that would be generated (for both .c and .s files)
MY_C_OBJS = $(addprefix $(PARAM_OBJ_DIR)/,$(subst .c,.o,$(notdir $(shell find . -type f -name "*.c"))))
MY_S_OBJS = $(addprefix $(PARAM_OBJ_DIR)/,$(subst .s,.o,$(notdir $(shell find . -type f -name "*.s"))))
OBJS = $(MY_C_OBJS) $(MY_S_OBJS)

-include objects.mk



INCLUDE_DIR = $(addprefix -I,$(shell find . -type d))
INCLUDE_DIR += -I$(PARAM_XTI_DIR)
CC_OPTIONS = $(PARAM_CC_OPTIONS) -Wall -Werror -fno-verbose-asm -Mfarcall -corecfg=0xc1703894 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
CMD = stxp70cc -mcore=stxp70v3 -Mextension=fpx -g -fshort-double $(INCLUDE_DIR)

CMD1 = $(CMD) -O1 $(CC_OPTIONS)
CMD2 = $(CMD) -Os $(CC_OPTIONS)

### Makefiles rules start here ###
# Specific rules for .s files
$(PARAM_OBJ_DIR)/%.o: STXP70/%.s .stxp70cf
	@echo $(CMD2)
	$(CMD2)
	@echo

$(PARAM_OBJ_DIR)/%.o: Misc/%.s .stxp70cf
	@echo $(CMD2)
	$(CMD2)
	@echo

$(PARAM_OBJ_DIR)/%.o: InterruptManager/%.s .stxp70cf
	@echo $(CMD2)
	$(CMD2)
	@echo

# Specific rule for all .c files in Misc/ folder as it needs to use $(CMD1) instead of $(CMD2). TBD: Why?
$(PARAM_OBJ_DIR)/%.o: Misc/%.c .stxp70cf
	@echo $(CMD1)
	$(CMD1)
	@echo

# Generic rule for all other .c files
$(PARAM_OBJ_DIR)/%.o: %.c .stxp70cf
	@echo $(CMD2)
	$(CMD2)
	@echo

# Below lines are needed to enable compilation of all dependent .c files to be compiled
# when a .h file is changed (all .c files that include this modified .h file)
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
endif

-include ../makefile.defs


### Makefiles rules start here ###
pre-build:
	-@echo 'Prebuild stage to generate the virtual register list file (to be used in build stage), the partial device wrapper file and perform other sanity checks'
	$(shell mkdir -p $(PARAM_OBJ_DIR))
	$(shell mkdir -p $(PARAM_REPORTS_DIR))
	$(shell mkdir -p $(PARAM_REPORTS_DIR)/DeviceParams)
	$(shell mkdir -p $(PARAM_REPORTS_DIR)/Reports)

	-python ../Scripts/SupportScripts/SharedScripts/PageElementExtraction/PreBuild.py -t $(PWD) -m Pictor -s STXP70,MemoryMap,LLA_Headers -M ProjectLevel -o $(PARAM_REPORTS_DIR) -b 0xF0000000 -a 0 -c 0xF0002A00 -i 0xF0002800  -d 1
	-@echo
	-@echo 'Starting main build..'
	-@echo
	make main-build

main-build: PictorProject.out

PictorProject.out: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: STxP70 C Linker'
	stxp70cc -mcore=stxp70v3 -Os -T"./lla_configuration/Linker.ld" -Mnostartup -map $(PARAM_OBJ_DIR)/PictorProject.map -Mextension=fpx -fshort-double -corecfg=0xc1703894 -o$(PARAM_OBJ_DIR)/PictorProject.out $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo
	make --no-print-directory post-build

post-build:
	-@echo 'Postbuild stage to complete the device wrapper file'
	-python ../Scripts/SupportScripts/SharedScripts/PageElementExtraction/PostBuild.py ProjectLevel $(PWD)/../Scripts/SupportScripts/SharedScripts/PageElementExtraction/DeviceSpecificMethods.py $(PARAM_OBJ_DIR) PictorProject $(PARAM_REPORTS_DIR) primary

.SECONDARY: pre-build main-build post-build

-include ../makefile.targets

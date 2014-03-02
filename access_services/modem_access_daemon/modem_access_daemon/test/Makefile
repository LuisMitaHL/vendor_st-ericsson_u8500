#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
### DIRECTORY DEFINITIONS ###
ACCESS_SERVICES_PATH ?= $(TOP)/vendor/st-ericsson/access_services
MODULE_ROOT := ..
OUT := out
OUT_OBJ := $(OUT)/obj

### TARGET DEFINITIONS ###
TARGET := $(OUT)/mad_test


### FUNCTION DEFINITIONS ###
define transform-d-to-p
@cp $(@:%.o=%.d) $(@:%.o=%.P); \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(@:%.o=%.d) >> $(@:%.o=%.P); \
	rm -f $(@:%.o=%.d)
endef

define transform-c-to-o-no-deps
@mkdir -p $(dir $@)
$(CC) \
	$(foreach incdir, $(INCDIRS), -I $(incdir)) \
	$(CFLAGS) \
	-MD -o $@ $<
endef

define transform-c-to-o
$(transform-c-to-o-no-deps)
$(hide) $(transform-d-to-p)
endef

define transform-o-to-exectuable
@mkdir -p $(dir $@)
$(CC) \
	$(OBJ) \
	-o $@ $(LDFLAGS)
endef

# $(info in add-o-target $(1) : $(2))\

define add-o-target
$(1): $(2)
endef


define declare-o-target
$(eval _obj_tmp := $(basename $(OUT_OBJ)/$(notdir $(1))).o)\
$(eval OBJ += $(_obj_tmp))\
$(eval $(call add-o-target, $(strip $(_obj_tmp)), $(strip $(1))))
endef

##
#
#
#
#

### INCLUDE DIRS ###
INCDIRS		:=	$(MODULE_ROOT)/include \
			$(MODULE_ROOT)/include/internal \
			$(ACCESS_SERVICES_PATH)/common/common_functionality/include \
			/usr/include/dbus-1.0/ \
			/usr/lib/dbus-1.0/include

### COMPILER FLAGS ###
CC		:=	gcc
CFLAGS		:=	-c -Wall -Wextra  -pthread -g -O0 -ftest-coverage -fprofile-arcs -DENABLE_MODULE_TEST -D_GNU_SOURCE
LDFLAGS		:=	-lpthread -ldbus-1 -lgcov -lm

# CFLAGS 		+= -DCN_DEBUG_ENABLED

### SOURCE FILES ###
SRC		:=	$(MODULE_ROOT)/src/dbus_handler.c \
		$(MODULE_ROOT)/src/log_client.c \
		$(MODULE_ROOT)/src/dbus_method_table.c \
		$(MODULE_ROOT)/src/backend/tx_bo.c \
		$(MODULE_ROOT)/src/backend/at_handler.c \
		$(MODULE_ROOT)/src/main.c \
		$(ACCESS_SERVICES_PATH)/common/common_functionality/log/src/util_log.c \
		$(ACCESS_SERVICES_PATH)/common/common_functionality/security/src/util_file.c \
		$(ACCESS_SERVICES_PATH)/common/common_functionality/main_loop/src/main_loop.c

### GCOV FILES ###
COV := $(addprefix $(OUT_OBJ)/, $(SRC_tmp:.c=.gcda) $(SRC_tmp:.c=.gcno))

### OBJECT FILES ###
# Will be filled by declare-o-target below.
OBJ :=


### BUILD TARGETS ###
.PHONY: all
all: $(TARGET)

.PHONY: %.P

%.o:
	$(transform-c-to-o)

$(foreach src, $(SRC), $(call declare-o-target, $(src)))


-include $(OBJ:%.o=%.P)

$(TARGET): $(OBJ)
	$(transform-o-to-exectuable)


cov: clean # Initial Code coverage setup, please improve!
	@for f in $(HDR) $(SRC); do cp -f $$f .; done
	@for f in *.c; do $(CC) -c $(CFLAGS) -g -O0 -ftest-coverage -fprofile-arcs $$f; done
	@$(CC) *.o $(LDFLAGS) -lgcov -o cov_test
	@./cov_test
	@rm -f module_test_main* *stubs* *testcases* # Remove files that shouldn't be measured
	@gcov *.gcda
	@lcov -d . -b . -c -o cov.info
	@genhtml -o coverage_results cov.info
	@rm -f *.c *.h *.o *.gcda *.gcno *.cov cov_test *.info
	@firefox $(MODULE_ROOT)/internal/test/coverage_results/internal/test/index.html &

### EXECUTION TARGETS ###
.PHONY: test
test: all
	@echo
	@echo "internal/test/test.txt created with the execution log"
	@echo
	@./out/mad_test 2>&1 | tee test.txt

# .PHONY: valgrind
# valgrind: all
# 	@valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./test 2>&1 | tee valgrind.txt && ./analyze_valgrind.py
# 	@echo
# 	@echo "internal/test/valgrind.txt' created with the execution log"
# 	@echo

.PHONY:
debug_print:
	@ echo
	@ echo "Enabling all prints (bit field value 15) by specifying log level file in /tmp/telephony_log_levels"
	@cp $(MODULE_ROOT)/test/telephony_log_levels /tmp/telephony_log_levels
	@ echo

clean:
	@rm -rfv $(OUT)

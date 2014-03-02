SUBDIRS := main anchor executor parser predial selector white_box_test
COVDIRS := executor parser

ifeq ($(ATC_ENABLE_FEATURE_ATC_CUSTOMER_EXTENSIONS),true)
include $(TOP)/vendor/st-ericsson/access_services/at/at_core_extension/internal/inc_Makefile
endif

gcovinfo := $(COVDIRS:%= -a %.info)

.PHONY: $(SUBDIRS) all clean test_fb test_mb test

all: $(SUBDIRS)
all: TARGET=all

test_fb: $(SUBDIRS)
test_fb: TARGET=test_fb

test_mb: $(SUBDIRS)
test_mb: TARGET=test_mb

test: $(SUBDIRS)
test: TARGET=test

clean: $(SUBDIRS)
clean: TARGET=clean

$(SUBDIRS):
	$(MAKE) -C $@ $(TARGET)

main: selector parser predial core anchor

anchor: selector

parser: core

core: selector
cov:
	for dir in $(COVDIRS) ; do  \
		lcov -d $$dir -b $$dir -c -o $$dir.info ; \
	done
	lcov $(gcovinfo) -o ../covtest.info
	genhtml -o ../covtestfolder/ ../covtest.info
	firefox ../covtestfolder/index.html

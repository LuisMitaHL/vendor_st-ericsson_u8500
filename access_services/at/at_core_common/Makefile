SUBDIRS := config internal

.PHONY: $(SUBDIRS) all test_mb test_fb test_cn clean cov test

#change all target to test for now to make it easier to work with eclipse.
all: $(SUBDIRS)
all: TARGET=test

clean: $(SUBDIRS) clean_cov
clean: TARGET=clean

test: $(SUBDIRS)
test: TARGET=test

valgrind: test
	valgrind --log-file=valgrind_result.txt --leak-check=full --track-origins=yes internal/white_box_test/white_box_test; less valgrind_result.txt

coverity:
	@sh internal/scripts/coverity.sh at_core_common

$(SUBDIRS):
	$(MAKE) -C $@ $(TARGET)

internal: config

test_mb:
	@echo "The 'test_mb' target is no longer relevant since the introduction of the Call Network module. Use 'make test' instead!"
	@echo

test_fb:
	@echo "The 'test_fb' target is no longer relevant since the introduction of the Call Network module. Use 'make test' instead!"
	@echo

test: internal config

cov: test
	$(MAKE) -C internal $@

debug_print:
	@make -C internal/white_box_test debug_print

clean_cov:
	rm -f covtest.info
	rm -fr covtestfolder
	rm -f valgrind_result.txt
	rm -f internal/*.info
ifeq ($(ATC_ENABLE_FEATURE_ATC_CUSTOMER_EXTENSIONS),true)
	rm -f $(TOP)/vendor/st-ericsson/access_services/at/at_core_extension/internal/*.info
endif



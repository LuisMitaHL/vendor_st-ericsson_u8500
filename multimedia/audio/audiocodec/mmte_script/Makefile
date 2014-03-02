# vim:syntax=make:
# -*- Mode: Makefile -*-

all test:
ifeq ($(findstring href, $(PLATFORM)),href)
	source $(MM_TOOLS_DIR)/lauterbach.sh && test_launcher.pl -n -v --platform $(PLATFORM) --list all_unitary_auto.ate
else
	@echo No test for platform: $(PLATFORM)
endif

cleantest regression:
	@echo "$@ target no yet implemented"


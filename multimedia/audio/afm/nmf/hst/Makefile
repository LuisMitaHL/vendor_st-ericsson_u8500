# vim:syntax=make:
# -*- Mode: Makefile -*-

all clean realclean install uninstall:
	$(MAKE) -f Make.nmf $@
	$(MAKE) -f Make.lib $@
	$(MAKE) -f Make.optim $@

TESTLISTS = testlist.lst
DATABASE  = hst
TEST_TAG  = $(PLATFORM)
include $(MM_MAKEFILES_DIR)/AudioTest.mk

######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : Build call and networking service daemon
## 
## Author          : Mats Bengtsson
##                   Johan Norberg
## 
###############################################################################

TARGETS=test valgrind cloc doxygen cov coverity astyle debug_print clean

all:
	@echo "available targets: $(TARGETS)"
	@echo ""

doxygen:
	@make -C libcn doxygen

test:
	@make -C internal/test test

valgrind:
	@make -C internal/test valgrind

cloc:
	@cloc *

cov:
	@make -C internal/test cov

coverity:
	@sh internal/test/script/coverity.sh call_network_module_test

astyle:
	@for i in `find . -name *.[c,h]`; do astyle --style=k/r --indent=spaces --break-blocks --convert-tabs --add-brackets --unpad-paren --pad-header --pad-oper --indent-col1-comments --align-pointer=name $$i; done

debug_print:
	@make -C internal/test debug_print

clean:
	@make -C libcn clean
	@make -C internal/test clean


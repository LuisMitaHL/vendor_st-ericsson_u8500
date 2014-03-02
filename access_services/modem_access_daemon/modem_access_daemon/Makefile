#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

TARGETS=test valgrind debug_print cloc cov coverity astyle clean


all:
	@echo "available targets: $(TARGETS)"
	@echo ""

test:
	@make -C test test

valgrind:
	@make -C test valgrind

debug_print:
	@make -C debug_print

cloc:
	@cloc *

cov:
	@make -C test cov

coverity:
	@sh test/script/coverity.sh modem_access_daemon_test

astyle:
	@for i in `find . -name *.[c,h]`; do astyle --style=k/r --indent=spaces --break-blocks --convert-tabs --add-brackets --unpad-paren --pad-header --pad-oper --indent-col1-comments --align-pointer=name $$i; done

clean:
	@make -C test clean
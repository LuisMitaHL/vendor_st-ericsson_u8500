.PHONY: all build build-copsd build-cops_api build-cops_cmd install cscope clean distclean

all: build

build-copsd: build-cops_api
	@echo "Building copsd"
	$(MAKE) CSPSALIBDIR=$(CSPSALIBDIR) --directory=copsd --no-print-directory -f lbp_copsd.mak

build-cops_api:
	@echo "Building cops-api"
	$(MAKE) --directory=cops-api --no-print-directory -f lbp_cops_api.mak

build-cops_cmd: build-cops_api
	@echo "Building cops-cmd"
	$(MAKE) --directory=tools/cops_cmd --no-print-directory -f lbp_cops_cmd.mak

build: build-cops_api build-cops_cmd build-copsd

install: build

cscope:
	@echo "Generating cscope files for bass_app."
	@find $(CURDIR) -name "*.[ch]" > $(CURDIR)/cscope.files
	@cscope -b -q -k

clean: clean-cops_api clean-cops_cmd clean-copsd
	@rm -rf cscope.*

clean-copsd:
	$(MAKE) --directory=copsd --no-print-directory -f lbp_copsd.mak clean

clean-cops_api:
	$(MAKE) --directory=cops-api --no-print-directory -f lbp_cops_api.mak clean

clean-cops_cmd:
	$(MAKE) --directory=tools/cops_cmd --no-print-directory -f lbp_cops_cmd.mak clean

distclean: clean

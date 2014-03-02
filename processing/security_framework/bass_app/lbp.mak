.PHONY: all build build-libtee build-libbassapp build-bass_app_testsuite install cscope clean distclean
all: build

build-libtee:
	@echo "Building libtee.so"
	@$(MAKE) --directory=teec --no-print-directory -f lbp_libtee.mak

build-libbassapp:
	@echo "Building libbassapp.so"
	@$(MAKE) --directory=libbassapp --no-print-directory -f lbp_libbassapp.mak

ifeq ($(BASS_APP_BUILD_TESTSUITE), true)
build-bass_app_testsuite: build-libtee build-libbassapp
	@echo "Building bass_app_testsuite"
	$(MAKE) --directory=test --no-print-directory -f lbp_test.mak
else
build-bass_app_testsuite:
	@echo "Not building bass_app_testsuite"
endif


build: build-libtee build-libbassapp build-bass_app_testsuite

install: build

cscope:
	@echo "Generating cscope files for bass_app."
	@find $(CURDIR) -name "*.[ch]" > $(CURDIR)/cscope.files
	@cscope -b -q -k

clean: clean-libtee clean-libbassapp clean-bass_app_testsuite
	@rm -rf cscope.*

clean-libtee:
	@$(MAKE) --directory=teec --no-print-directory -f lbp_libtee.mak clean

clean-libbassapp:
	@$(MAKE) --directory=libbassapp --no-print-directory -f lbp_libbassapp.mak clean

clean-bass_app_testsuite:
	$(MAKE) --directory=test --no-print-directory -f lbp_test.mak clean

distclean: clean

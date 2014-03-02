
#ifndef WIN_ROOT
#    $(error Please set WIN_ROOT variable (ex: setenv WIN_ROOT X:/work_dummy/multimedia))
#endif


UNIX_DIR=$(shell echo $(MMROOT) | cut -d \/ -f1 )/

all install: mmenv.cmd

mmenv.cmd:
	@rm -rf $@
	@echo "set MMROOT=$(MMROOT)" > $@ 
	@echo "set UNIX_DIR=$(UNIX_DIR)" >> $@
	@echo "set TRACE_TOOLSET=C:\System-Trace-110-02" >> $@
	@echo "set PERL5LIB=%MMROOT%/shared/launcher;%MMROOT%/shared/launcher/perllib;%MMROOT%/shared/osttools/ostdec" >> $@

	@echo "set T32SYS=C:\T32" >> $@
	@echo "set T32CONFIG=C:\T32\config.t32," >> $@
	@echo "set PATH=%PATH%;C:\T32;" >> $@
	@echo "@perl -v > NUL" >> $@
	@echo "@if errorlevel==1 echo Perl not found in the path!!?" >> $@

	@echo "$@ generated"
#	cp ${T32SYS}/t32cadi.so.1 .

clean realclean:
	rm -rf mmenv.cmd
	rm -rf ./t32cadi.so.1

test cleantest:
	@echo No Test...

uninstall:
	@echo Nothing...

.PHONY: mmenv.cmd

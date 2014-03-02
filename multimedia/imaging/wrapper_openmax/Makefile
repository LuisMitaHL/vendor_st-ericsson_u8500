# -*- Mode: Makefile -*-
# vim:syntax=make:

CORE=arm
MMSYSTEM=x
MMBOARD=x
NORMAL_BUILD=yes

ifeq ($(NORMAL_BUILD),yes)
    DIRECTORIES= nmf  proxy
    #shall be before Make.dispatch to have it taken into account
    include $(MM_MAKEFILES_DIR)/SharedDispatch.mk
else
    include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

all:
	make NORMAL_BUILD=yes $@ ; make patch ; make NORMAL_BUILD=yes $@
install uninstall realclean clean:
	make NORMAL_BUILD=yes $@
endif

PATCH_DIR=${COMPONENT_TOP_DIR}/nmf/host/patch
CORE=arm

NMFREPO_DIRHEADERS=$(BUILD)
NMFREPO_DIRSOURCES=$(TMPDIR)

makepatch:
	mkdir -p ${PATCH_DIR}/orig
	rm -f ${PATCH_DIR}/hpp.patch.old ${PATCH_DIR}/cpp.patch.old 
	if test -e  ${PATCH_DIR}/hpp.patch; then  mv ${PATCH_DIR}/hpp.patch ${PATCH_DIR}/hpp.patch.old; fi
	if test -e  ${PATCH_DIR}/cpp.patch; then  mv ${PATCH_DIR}/cpp.patch ${PATCH_DIR}/cpp.patch.old; fi
	cp nmf/host/${NMFREPO_DIRSOURCES}/openmax_processor_wrp/src/openmax_processor_wrp/openmax_processor_wrp-nmf.cpp ${PATCH_DIR}/orig/openmax_processor_wrp-nmf.cpp
	cp nmf/host/${NMFREPO_DIRHEADERS}/openmax_processor_wrp.hpp ${PATCH_DIR}/orig/openmax_processor_wrp.hpp
	cd ${PATCH_DIR} && diff -u orig/openmax_processor_wrp-nmf.cpp fixed/openmax_processor_wrp-nmf.cpp > ${PATCH_DIR}/cpp.patch || test $$? -eq 1 
	cd ${PATCH_DIR} && diff -u orig/openmax_processor_wrp.hpp fixed/openmax_processor_wrp.hpp > ${PATCH_DIR}/hpp.patch || test $$? -eq 1

clean_nmf:
	printf "cleaning nmf\n"
	find nmf/host/nmf_repository ! -regex ".*\.svn.*" -type f -delete
	find nmf/host/_nmf_tmpdir ! -regex ".*\.svn.*" -type f -delete

patch:
	cd nmf/host/${NMFREPO_DIRHEADERS}/ && patch -N -p1 < ${PATCH_DIR}/hpp.patch
	cd nmf/host/${NMFREPO_DIRSOURCES}/openmax_processor_wrp/src/openmax_processor_wrp/ && patch -N -p1 < ${PATCH_DIR}/cpp.patch

printdir:
	echo $(BUILD)
	echo $(TMPDIR)

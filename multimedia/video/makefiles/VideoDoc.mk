# -*- Mode: Makefile -*-
# vim:syntax=make:

export MMDSPPROCESSOR

ifndef NO_NMF_BUILD
all : idl-doc
clean: doc-clean
realclean: doc-clean
endif

ifneq ($(MMSYSTEM),android)
DOXYGEN_INPUT =	$(addprefix $(MODULE)/../, $(IDLLIST))	$(addprefix $(MODULE)/../, $(IDTLIST))

$(MODULE)/doc/doc_$(SVA_SUBSYSTEM).pdf: $(MODULE)/doc/main_page $(MMROOT)/video/makefiles/doxyfile $(DOXYGEN_INPUT)
	@mkdir -p $(MODULE)/doc/doxygen/$(MMDSPPROCESSOR)
	# convert $(MODULE)/doc/pictures/nomadik.jpg $(MODULE)/doc/pictures/nomadik.eps
	@cpp -E -o $(MODULE)/doc/doxyfile_$(SVA_SUBSYSTEM) $(MMROOT)/video/makefiles/doxyfile 	\
		-DDOXYGEN_INPUT="$(DOXYGEN_INPUT)"						\
		-DMODULE=$(MODULE) 								\
		-DMODULE_NAME=$(shell echo $(shell pwd) | sed -e "s/.*\/components\///" | sed -e "s/\/.*//")	\
		-DPLATFORM=$(PLATFORM) 								\
		-DMMDSPPROCESSOR=$(MMDSPPROCESSOR) 						\
		-DHW_VERSION=$(shell grep '#define HV_HW_REL' $(MMROOT)/video/components/registers_def/$(SVA_SUBSYSTEM)/inc/t1xhv_hamac_xbus_regs.h | sed -e "s/.*0x//" | sed -e "s/U.*//") \
		-DCOMPONENT_VERSION='$(shell getversion $(MODULE)/. | sed -e "s/branch: //")'
	@doxygen $(MODULE)/doc/doxyfile_$(SVA_SUBSYSTEM) > /dev/null
	#@$(MAKE) -C $(MODULE)/doc/doxygen/$(MMDSPPROCESSOR)/latex pdf > /dev/null
	#@cp $(MODULE)/doc/doxygen/$(MMDSPPROCESSOR)/latex/refman.pdf $(MODULE)/doc/doc_$(SVA_SUBSYSTEM).pdf

idl-doc:
	@if test -e $(MODULE)/doc; then if $(MAKE) -f $(MM_MAKEFILES_DIR)/VideoDoc.mk $(MODULE)/doc/doc_$(SVA_SUBSYSTEM).pdf ;\
	then :; else exit 1 ; fi ; fi;

doc-clean:
	@rm -rf $(MODULE)/doc/doxygen $(MODULE)/doc/doxyfile_* $(MODULE)/doc/doc_*.pdf

else
all : idl-doc
clean: doc-clean
realclean: doc-clean
endif

.PHONY: idl-doc doc-clean

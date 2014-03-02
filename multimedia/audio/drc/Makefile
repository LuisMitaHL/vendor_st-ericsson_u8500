# -*- Mode: Makefile -*-
# vim:syntax=make:
include $(MM_MAKEFILES_DIR)/AudioAlgoAll.mk


#-----------------------------------------------------------
# local Rules
#-----------------------------------------------------------

#---------------
# Doxygen Doc
#---------------

doc:
	@echo "=================================================================="
	@echo "Generating html documentation for DRC library interface ...."
	@cd effect/doc; doxygen doxy.conf ; cd html ; sed -i s/Modules/Commands/ *.html 
	@cd effect/doc/latex; sed -i s/Module/Commands/ *.tex ; sed -i s/Page// *.tex ; make 2>&1 > make.log; mv refman.pdf drc.pdf 
	@echo "=================================================================="
	@echo "Html documentation generated in effect/doc/html/index.html"
	@echo "Pdf  documentation generated in effect/doc/pdf/drc.pdf"


.PHONY: doc


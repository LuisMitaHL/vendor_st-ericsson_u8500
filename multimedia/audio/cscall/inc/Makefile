include ../make.ost_conf

all install: gendico

gendico:
	@perl $(TRACECOMPILERDIR)/tracecompiler.pl --inc=$(OST_COMPONENT_INC) --id=$(OST_MODULE_ID) --name=$(OST_DICO_NAME) --dico=$(OST_COMPONENT_INC)/autogentraces/CSCALL_nmf_host.xml --action="GenGroupDefinition"

clean realclean:
	@\rm -rf autogentraces

.PHONY: gendico


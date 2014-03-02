doc:
ifneq (,$(wildcard src/Doxyfile))
	-@sh src/update_version.sh
	-@doxygen src/Doxyfile
	@cp src/ST-Ericsson.png html
endif
	-@make -C latex > /dev/null
	-@cp latex/refman.pdf speech_proc_itf.pdf
	-@rm -rf latex
	@echo  "\n-- Interface Documentation generated in $(PWD)/html/index.html and $(PWD)/speech_proc_itf.pdf\n"
#	@tar czf doc/speech_proc_itf_v`cat include/speech_proc_interface.h | grep \version | cut -d v -f3`.tar.gz include/speech_proc_interface.h include/speech_proc_omx_interface.h doc/html/ --exclude *tab*

.PHONY: doc

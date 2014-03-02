doc:
ifneq (,$(wildcard src/Doxyfile))
	-@sh src/update_version.sh
	-@doxygen src/Doxyfile
	@cp src/ST-Ericsson.png html
endif
	-@make -C latex > /dev/null
	-@cp latex/refman.pdf speech_proc.pdf
	-@rm -rf latex
	@echo "\n-- Documentation generated in $(PWD)/html/index.html and $(PWD)/speech_proc.pdf\n"

wkpdf:
	-@wkhtmltopdf --outline -R 0 --use-xserver html/index.html html/overview.html html/wrapper.html html/time.html html/config.html html/trace.html html/diff.html html/speech__proc__config_8h.html speech_proc.pdf


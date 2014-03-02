REFMAN_FLAGS ?= --no-uml-filtering --use-repo
REFMAN_KERNEL_FLAGS ?=
KERNELDIR ?= ../../stelp/linux/
BASESYSTEMDIR ?= ../base_system
REFMANDIR ?= $(CURDIR)
REFMAN_OUT_DIR ?= $(PRODUCT_OUT)
TEMP_OUTDIR ?= $(REFMAN_OUT_DIR)/temp_out/
OUTDIR ?= $(REFMAN_OUT_DIR)/eclipseplugin
KERNEL_FLAGS ?=
STD_LINUX_DOC_DIR ?= $(TEMP_OUTDIR)/std-linux-kernel-documentation
LATEXDIR = $(REFMAN_OUT_DIR)/latex
TARGET_PRODUCT ?= ste_u5500

# Dependencies of RefMan.jar
JAVACLASSPATH  = lib/jdom.jar:lib/log4j-1.2.15.jar:lib/xmlunit-1.3.jar:lib/slf4j-api-1.5.10.jar:lib/slf4j-log4j12-1.5.10.jar:.
# Required by the java tool build part
REFMANJAVAOUT  = $(REFMAN_OUT_DIR)/javaout
REFMANJAVASRC  = $(REFMANDIR)/src
REFMANJAVAMANIFEST = meta-inf/Manifest.mf
JAVAFILELIST   = $(REFMANJAVAOUT)/filelist.txt

ifndef UML_EXPORT_XML
	UML_EXPORT_XML = $(TOPLEVEL)/uml-export/ste-uml-export.xml
endif

ifdef ANDROID
	REFMAN_FLAGS += --include_android_doc
endif

.PHONY: default
default: help

.PHONY: help
help:
	@echo "****  Reference Manual Makefile for STELP  ****"
	@echo "This Makefile doesn't support multiple make jobs like make -j 2"
	@echo "Environment variables"
	@echo "* TARGET_PRODUCT = <The name of the platform>"
	@echo "* KERNELDIR = <path to linux kernel sources, mandatory>"
	@echo "* REFMAN_KERNEL_FLAGS = --ignore-errors"
	@echo "* REFMAN_FLAGS = --ignore-errors, --verbose, --use-repo(default), [--no-uml-filtering] (default), --recurse ADDITIONALDIR "
	@echo "* KERNEL_FLAGS: passed directly to linux kernel makefile"
	@echo "* PROJECTROOTDIR = <path to root of project>"
	@echo "* UML_EXPORT_XML = <Absolute path to the Uml_export_xml file>"
	@echo "Available make targets"
	@echo "* all - runs htmldocs"
	@echo "* config - configures the manual tool chain"
	@echo "* htmldocs - creates htmldocs"
	@echo "* install - installs the tool chain"
	@echo "* clean - cleans all build files"
	@echo "* distclean - removes all config files"
	@echo "Build output"
	@echo "* temp_out"
	@echo "Examples of how to build the ref manual for Fairbanks"
	@echo "* make -f refman.mak all"

.PHONY: all
all:
	$(MAKE) -f refman.mak config
	$(MAKE) -f refman.mak build
	$(MAKE) -f refman.mak htmldocs
	$(MAKE) -f refman.mak install

.PHONY: config
config:
	mkdir -p $(STD_LINUX_DOC_DIR)
	mkdir -p $(TEMP_OUTDIR)
	mkdir -p $(OUTDIR)
	mkdir -p $(REFMANJAVAOUT)
	find >$(JAVAFILELIST) $(REFMANJAVASRC) -iname "*.java" -print
	$(REFMANDIR)/tools/extract_version_number.sh $(PROJECTROOTDIR) $(TEMP_OUTDIR) $(REFMAN_FLAGS)
	$(REFMANDIR)/tools/verify_tools_dependency.sh
	cp -r $(REFMANDIR)/lib $(REFMAN_OUT_DIR)/
	cp -r $(REFMANDIR)/resources $(REFMAN_OUT_DIR)/

.PHONY: build
build: build-tool

.PHONY: clean
clean: clean-tool clean-htmldocs

.PHONY: htmldocs
htmldocs: build-htmldocs

.PHONY: distclean
distclean: clean distclean-tool

build-tool: build-tool-classes build-tool-jar

.PHONY: build-tool-classes
build-tool-classes:
		javac -sourcepath $(REFMANJAVASRC) -classpath $(JAVACLASSPATH) -d $(REFMANJAVAOUT) @$(JAVAFILELIST)

.PHONY: build-tool-jar
build-tool-jar: build-tool-classes
		jar c0f $(REFMAN_OUT_DIR)/RefMan.jar -C $(REFMANJAVAOUT) .
		jar u0f $(REFMAN_OUT_DIR)/RefMan.jar -C $(REFMANJAVASRC) log4j.xml
		jar u0mf $(REFMANJAVAMANIFEST) $(REFMAN_OUT_DIR)/RefMan.jar
		chmod 755 $(REFMAN_OUT_DIR)/RefMan.jar

.PHONY: clean-tool
clean-tool:
		rm -rf $(REFMANJAVAOUT)/*
		rm -f $(REFMAN_OUT_DIR)/RefMan.jar


.PHONY: distclean-tool
distclean-tool:
		rm -f $(JAVAFILELIST)
		rm -rf $(REFMANJAVAOUT)

.PHONY: build-htmldocs
build-htmldocs:
	mv $(KERNELDIR)/Documentation/DocBook/stylesheet.xsl $(KERNELDIR)/Documentation/DocBook/stylesheet.xsl.bak
	cp -f $(REFMANDIR)/resources/stylesheet.xsl $(KERNELDIR)/Documentation/DocBook
	$(MAKE) -C $(KERNELDIR) $(REFMAN_KERNEL_FLAGS) $(KERNEL_FLAGS) htmldocs
	@mv $(KERNELDIR)/Documentation/DocBook/stylesheet.xsl.bak $(KERNELDIR)/Documentation/DocBook/stylesheet.xsl
	$(REFMANDIR)/tools/stelp_extract_kernel_toc.sh $(KERNELDIR) $(REFMAN_OUT_DIR) $(TEMP_OUTDIR) $(REFMAN_FLAGS)
	$(REFMANDIR)/tools/stelp_extract_kernel_std_linux_toc.sh $(KERNELDIR) $(PROJECTROOTDIR) $(STD_LINUX_DOC_DIR) $(REFMAN_FLAGS)
	java -jar $(REFMAN_OUT_DIR)/RefMan.jar -p $(TARGET_PRODUCT) -o $(OUTDIR) -k $(KERNELDIR) -s $(TOPLEVEL) -r $(PROJECTROOTDIR) -t $(TEMP_OUTDIR) -u $(UML_EXPORT_XML) $(REFMAN_FLAGS) --recurse $(STD_LINUX_DOC_DIR)
	rm -f $(TEMP_OUTDIR)/versionNumber.txt

.PHONY: install
install:
	mkdir -p $(IMAGE_OUTPUT)
	cp -f $(REFMANDIR)/resources/install.txt $(OUTDIR)/install.txt
	$(REFMANDIR)/tools/zip_output.sh $(OUTDIR)
	mv $(OUTDIR)/Reference_manual.zip $(IMAGE_OUTPUT)/Reference_manual.zip

.PHONY: clean-htmldocs
clean-htmldocs:
	rm -rf $(TEMP_OUTDIR)
	rm -rf $(OUTDIR)
	rm -rf $(LATEXDIR)
	rm -rf $(REFMAN_OUT_DIR)

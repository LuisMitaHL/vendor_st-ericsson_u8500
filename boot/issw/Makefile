OUT_DIR ?= ./
CONFIG_FILE ?= $(OUT_DIR)/.config
-include $(CONFIG_FILE)

config : PREFIX := $(shell bash -c "mktemp -d")
config : BOOTPREFIX := $(PREFIX)/boot/
config : RELBOOTPATH := ../../boot/
config : RELLOADERSPATH := ../../loaders/
config : RELMODEMPATH := ../../modem_images/
config : ENABLE_MODEM_IN_FILELIST := true
config : MEMINIT_PRCMU_CONFIGLIST_OUTSIDE_ISSW := false
config :
	@mkdir -p $(OUT_DIR)
	@rm -f $(CONFIG_FILE)
	@touch $(CONFIG_FILE)
	@echo PREFIX: $(PREFIX)
	@echo "PREFIX := $(PREFIX)" >> $(CONFIG_FILE)
	@echo BOOTPREFIX: $(BOOTPREFIX)
	@echo "BOOTPREFIX := $(BOOTPREFIX)" >> $(CONFIG_FILE)
	@echo RELBOOTPATH: $(RELBOOTPATH)
	@echo "RELBOOTPATH := $(RELBOOTPATH)" >> $(CONFIG_FILE)
	@echo RELLOADERSPATH: $(RELLOADERSPATH)
	@echo "RELLOADERSPATH := $(RELLOADERSPATH)" >> $(CONFIG_FILE)
	@echo RELMODEMPATH: $(RELMODEMPATH)
	@echo "RELMODEMPATH := $(RELMODEMPATH)" >> $(CONFIG_FILE)
	@echo ENABLE_MODEM_IN_FILELIST: $(ENABLE_MODEM_IN_FILELIST)
	@echo "ENABLE_MODEM_IN_FILELIST := $(ENABLE_MODEM_IN_FILELIST)" >> $(CONFIG_FILE)
	@echo MEMINIT_PRCMU_CONFIGLIST_OUTSIDE_ISSW: $(MEMINIT_PRCMU_CONFIGLIST_OUTSIDE_ISSW)
	@echo "MEMINIT_PRCMU_CONFIGLIST_OUTSIDE_ISSW := $(MEMINIT_PRCMU_CONFIGLIST_OUTSIDE_ISSW)" >> $(CONFIG_FILE)
	@echo "CONFIGURED := 1" >> $(CONFIG_FILE)

build :
	@echo dummy >>/dev/null


install :
ifeq ($(CONFIGURED),1)
	@$(if $(wildcard $(BOOTPREFIX)),,mkdir -p $(BOOTPREFIX))
	@cp -r bootimages $(BOOTPREFIX)/
ifneq ($(MEMINIT_PRCMU_CONFIGLIST_OUTSIDE_ISSW),true)
	@cp -r meminits $(BOOTPREFIX)/
endif
ifneq ($(MEMINIT_PRCMU_CONFIGLIST_OUTSIDE_ISSW),true)
	@cp -r power_management $(BOOTPREFIX)/
endif
	@cp -r boot_cert $(BOOTPREFIX)/
	@cp -r signing $(PREFIX)/

	#for new flash solution
	@cp sw_type_map_no_signing.txt $(PREFIX)/
	@cp sw_type_map.txt $(PREFIX)/
	@cp sw_type_map_hz3.txt $(PREFIX)/
	@cp sw_type_map_hz3_basic.txt $(PREFIX)/

else
	@echo Warning: nothing to install
endif

clean :
ifeq ($(CONFIGURED),1)
	@rm -rf $(BOOTPREFIX)/bootimages
ifneq ($(MEMINIT_PRCMU_CONFIGLIST_OUTSIDE_ISSW),true)
	@rm -rf $(BOOTPREFIX)/meminits
	@rm -rf $(BOOTPREFIX)/power_management
endif
	@rm -rf $(BOOTPREFIX)/boot_cert
	@rm -rf $(PREFIX)/signing
	@rm -f $(PREFIX)/sw_type_map.txt
	@rm -f $(PREFIX)/sw_type_map_no_signing.txt
	@rm -f $(PREFIX)/sw_type_map_hz3.txt
	@rm -f $(PREFIX)/sw_type_map_hz3_basic.txt
else
	@echo nothing to clean
endif

distclean : clean
	@rm -f $(CONFIG_FILE)

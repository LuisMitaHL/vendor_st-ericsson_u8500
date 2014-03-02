ifeq ($(CONFIG_DIR),)
    configfile=.config
else
    configfile=$(CONFIG_DIR)/.config
endif
-include $(configfile)

config : PREFIX := $(shell bash -c "mktemp -d")
config : FLASHKITDIR := $(PREFIX)/flashkit
config : COPS_DIR := $(COPS_DIR)
config : COPS_MAKEFILE := $(COPS_MAKEFILE)
config :
	@rm -f $(configfile)
	@touch $(configfile)
	@echo PREFIX: $(PREFIX)
	@echo "PREFIX := $(PREFIX)" >> $(configfile)
	@echo FLASHKITDIR: $(FLASHKITDIR)
	@echo "FLASHKITDIR := $(FLASHKITDIR)" >> $(configfile)
	@echo COPS_DIR: $(COPS_DIR)
	@echo "COPS_DIR := $(COPS_DIR)" >> $(configfile)
	@echo COPS_MAKEFILE: $(COPS_MAKEFILE)
	@echo "COPS_MAKEFILE := $(COPS_MAKEFILE)" >> $(configfile)
	@echo "CONFIGURED := 1" >> $(configfile)

build :
	@echo dummy >>/dev/null

install :
ifeq ($(CONFIGURED),1)
	@$(if $(wildcard $(FLASHKITDIR)),,mkdir $(FLASHKITDIR))
	@$(if $(wildcard $(FLASHKITDIR)/configs),,mkdir $(FLASHKITDIR)/configs)
	@$(if $(wildcard $(FLASHKITDIR)/cspsa),,mkdir $(FLASHKITDIR)/cspsa)
	@$(if $(wildcard $(PREFIX)/specialbootscripts),,mkdir $(PREFIX)/specialbootscripts)
	install -m 755 -t $(PREFIX)/ ./flasher
	install -m 644 -t $(PREFIX)/ ./flasher.bat
	install -m 644 -t $(PREFIX)/ ./readme-flasher
	install -m 755 -t $(PREFIX)/specialbootscripts/ ./specialbootscripts/*.sh
	install -m 644 -t $(PREFIX)/specialbootscripts/ ./specialbootscripts/*.bat
	install -m 644 -t $(FLASHKITDIR)/cspsa/ ./cspsa/*
	$(MAKE) -C $(COPS_DIR) -f $(COPS_DIR)/$(COPS_MAKEFILE) install-cops-data localstatedir=$(FLASHKITDIR)/cspsa/ COPS_PLATFORM_TYPE=$(COPS_PLATFORM_TYPE)
	cp -r --remove-destination ./configs/* $(FLASHKITDIR)/configs/
else
	@echo warning nothing to install
endif

clean :
ifeq ($(CONFIGURED),1)
	@rm -f $(PREFIX)/specialbootscripts/*.sh
	@rm -f $(PREFIX)/specialbootscripts/*.bat
	@rm -rf $(FLASHKITDIR)/cspsa/*
	@rm -rf $(FLASHKITDIR)/configs/*
	@rm -f $(PREFIX)/flasher
	@rm -f $(PREFIX)/flasher.bat
	@rm -f $(PREFIX)/readme-flasher
else
	@echo nothing to clean
endif

distclean : clean
	@rm -f $(configfile)

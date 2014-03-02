PACKAGE_NAME=lua-5.1.4
PACKAGE_SOURCE=$(CURDIR)/$(PACKAGE_NAME).tar.gz
PACKAGE_WEB=http://www.lua.org
PACKAGE_URL=http://www.lua.org/ftp/lua-5.1.4.tar.gz
PACKAGE_BUILD_ROOT=$(TEMPDIR)/$(PACKAGE_NAME)
PATCH1=$(CURDIR)/lua.patch
# Compile this package with small thumb code
LDFLAGS +=  $(STAGING_AREA)/usr/lib/libncurses.a
ifdef USE_THUMB
CFLAGS := -mthumb -mthumb-interwork $(CFLAGS)
endif

$(PACKAGE_BUILD_ROOT): $(PACKAGE_SOURCE) $(PATCH1) $(PATCH2)
	@echo "Building $(PACKAGE_NAME)"
	rm -rf $(PACKAGE_BUILD_ROOT)
	# This creates the PACKAGE_BUILD_ROOT
	cd $(TEMPDIR) && tar xvfz $(PACKAGE_SOURCE)
	cd $(PACKAGE_BUILD_ROOT) && patch -p1 < $(PATCH1)
	$(MAKE) linux --directory=$(PACKAGE_BUILD_ROOT)

build: $(PACKAGE_BUILD_ROOT)
	@echo "Built $(PACKAGE_NAME)"

install: build
	cd $(PACKAGE_BUILD_ROOT) && $(MAKE) install DESTDIR=$(STAGING_AREA)
	@$(STRIP_SYMBOLS) $(STAGING_AREA)/usr/bin/lua
	@$(STRIP_SYMBOLS) $(STAGING_AREA)/usr/bin/luac
	@$(STRIP_SYMBOLS) $(STAGING_AREA)/usr/bin/lua_static
	@$(STRIP_SYMBOLS) $(STAGING_AREA)/usr/bin/luac_static
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/bin 755 0 0
	@$(PACKAGE_FILE) /usr/bin/lua $(STAGING_AREA)/usr/bin/lua 755 0 0
	@$(PACKAGE_FILE) /usr/bin/luac $(STAGING_AREA)/usr/bin/luac 755 0 0

clean:
	@rm -rf $(PACKAGE_BUILD_ROOT)

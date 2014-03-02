PACKAGE_NAME=alsa-utils-1.0.17
PACKAGE_SOURCE=$(CURDIR)/$(PACKAGE_NAME).tar.bz2
PACKAGE_BUILD_ROOT=$(TEMPDIR)/$(PACKAGE_NAME)
PACKAGE_WEB=http://www.alsa-project.org/
PACKAGE_URL=ftp://ftp.alsa-project.org/pub/utils/alsa-utils-1.0.17.tar.bz2
# Compile this package with small thumb code
ifdef USE_THUMB
CFLAGS := -mthumb -mthumb-interwork $(CFLAGS)
endif

$(PACKAGE_BUILD_ROOT): $(PACKAGE_SOURCE) alsa-utils.mak
	@echo "Building $(PACKAGE_NAME)"
	rm -rf $(PACKAGE_BUILD_ROOT)
	# This creates the PACKAGE_BUILD_ROOT dir
	cd $(TEMPDIR) && tar xvfj $(PACKAGE_SOURCE)
	cd $(PACKAGE_BUILD_ROOT) && ./configure --prefix=/usr \
	--host=$(HOST_PREFIX) \
	--build=$(BUILD_PREFIX) \
	--target=$(TARGET_PREFIX) \
	--disable-alsatest \
	--disable-nls \
	--disable-rpath \
	--disable-largefile
	$(MAKE) --directory=$(PACKAGE_BUILD_ROOT)

build: $(PACKAGE_BUILD_ROOT)
	@echo "Built $(PACKAGE_NAME)"

install: build
	cd $(PACKAGE_BUILD_ROOT) && $(MAKE) install DESTDIR=$(STAGING_AREA)
	@$(STRIP_SYMBOLS) $(STAGING_AREA)/usr/bin/aconnect
	@$(STRIP_SYMBOLS) $(STAGING_AREA)/usr/bin/amixer
	@$(STRIP_SYMBOLS) $(STAGING_AREA)/usr/bin/aplay
	# alsaconf is a script and does not need stripping
	@$(STRIP_SYMBOLS) $(STAGING_AREA)/usr/sbin/alsactl
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/bin 755 0 0
	@$(PACKAGE_FILE) /usr/bin/amixer ${STAGING_AREA}/usr/bin/amixer 755 0 0
	@$(PACKAGE_FILE) /usr/bin/alsamixer ${STAGING_AREA}/usr/bin/alsamixer 755 0 0
	@$(PACKAGE_FILE) /usr/bin/aplay ${STAGING_AREA}/usr/bin/aplay 755 0 0
	@$(PACKAGE_LINK) /usr/bin/arecord aplay 775 0 0
	@$(PACKAGE_DIR) /usr/bin 755 0 0
	@$(PACKAGE_DIR) /usr/share/sounds 755 0 0
	@$(PACKAGE_DIR) /usr/share/sounds/alsa 755 0 0
	@$(PACKAGE_DIR) /usr/sbin 755 0 0
	@$(PACKAGE_FILE) /usr/sbin/alsaconf ${STAGING_AREA}/usr/sbin/alsaconf 755 0 0
	@$(PACKAGE_FILE) /usr/sbin/alsactl ${STAGING_AREA}/usr/sbin/alsactl 755 0 0

clean:
	@rm -rf $(PACKAGE_BUILD_ROOT)

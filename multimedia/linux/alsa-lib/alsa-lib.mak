PACKAGE_NAME=alsa-lib-1.0.17a
PACKAGE_SOURCE=$(CURDIR)/$(PACKAGE_NAME).tar.bz2
PACKAGE_BUILD_ROOT=$(TEMPDIR)/$(PACKAGE_NAME)
PACKAGE_WEB=http://www.alsa-project.org/
PACKAGE_URL=ftp://ftp.alsa-project.org/pub/lib/$(PACKAGE_NAME).tar.bz2

$(PACKAGE_BUILD_ROOT): $(PACKAGE_SOURCE) alsa-lib.mak
	@echo "Building $(PACKAGE_NAME)"
	rm -rf $(PACKAGE_BUILD_ROOT)
	# This creates the PACKAGE_BUILD_ROOT
	cd $(TEMPDIR) && tar xvfj $(PACKAGE_SOURCE)
	# If you add --with-softfloat compilation fails beyond 1.0.14rc3
	# This was reported to ALSA project as bug 0004021 (ELINWAL)
	cd $(PACKAGE_BUILD_ROOT) && ./configure --prefix=/usr \
	--build=$(BUILD_PREFIX) \
	--host=$(HOST_PREFIX) \
	--target=$(TARGET_PREFIX) \
	--disable-aload \
	--disable-instr \
	--disable-python \
	--disable-alisp \
	--disable-rpath \
	--with-debug=no \
	--enable-shared
	$(MAKE) --directory=$(PACKAGE_BUILD_ROOT) clean
	$(MAKE) --directory=$(PACKAGE_BUILD_ROOT)
	# Ignore these failures, executables are there
	$(MAKE) -i --directory=$(PACKAGE_BUILD_ROOT)/test latency
	$(MAKE) -i --directory=$(PACKAGE_BUILD_ROOT)/test pcm
	$(MAKE) -i --directory=$(PACKAGE_BUILD_ROOT)/test pcm_min

build: $(PACKAGE_BUILD_ROOT)
	@echo "Built $(PACKAGE_NAME)"

install: build
	cd $(PACKAGE_BUILD_ROOT) && $(MAKE) install DESTDIR=$(STAGING_AREA)
	install $(PACKAGE_BUILD_ROOT)/test/.libs/latency $(STAGING_AREA)/usr/bin
	install $(PACKAGE_BUILD_ROOT)/test/.libs/pcm $(STAGING_AREA)/usr/bin
	install $(PACKAGE_BUILD_ROOT)/test/.libs/pcm_min $(STAGING_AREA)/usr/bin
	$(STRIP_SYMBOLS) $(STAGING_AREA)/usr/lib/libasound.so.2.0.0
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/lib 755 0 0
	@$(PACKAGE_FILE) /usr/lib/libasound.so.2.0.0 $(STAGING_AREA)/usr/lib/libasound.so.2.0.0 755 0 0
	@$(PACKAGE_LINK) /usr/lib/libasound.so.2 libasound.so.2.0.0 755 0 0
	@$(PACKAGE_LINK) /usr/lib/libasound.so libasound.so.2.0.0 755 0 0
	@$(PACKAGE_DIR) /usr/bin 755 0 0
	@$(PACKAGE_FILE) /usr/bin/latency $(STAGING_AREA)/usr/bin/latency 755 0 0
	@$(PACKAGE_FILE) /usr/bin/pcm $(STAGING_AREA)/usr/bin/pcm 755 0 0
	@$(PACKAGE_FILE) /usr/bin/pcm_min $(STAGING_AREA)/usr/bin/pcm_min 755 0 0
	@$(PACKAGE_DIR) /usr/share 755 0 0
	@$(PACKAGE_DIR) /usr/share/alsa 755 0 0
	@$(PACKAGE_FILE) /usr/share/alsa/alsa.conf $(STAGING_AREA)/usr/share/alsa/alsa.conf 644 0 0
	@$(PACKAGE_FILE) /usr/share/alsa/smixer.conf $(STAGING_AREA)/usr/share/alsa/smixer.conf 644 0 0
	@$(PACKAGE_DIR) /usr/share/alsa/cards 755 0 0
	@$(PACKAGE_FILE) /usr/share/alsa/cards/aliases.conf $(STAGING_AREA)/usr/share/alsa/cards/aliases.conf 644 0 0
	@$(PACKAGE_DIR) /usr/share/alsa/pcm 755 0 0
	@$(PACKAGE_FILE) /usr/share/alsa/pcm/default.conf $(STAGING_AREA)/usr/share/alsa/pcm/default.conf 644 0 0

clean:
	@rm -rf $(PACKAGE_BUILD_ROOT)

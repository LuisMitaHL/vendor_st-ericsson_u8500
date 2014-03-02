PACKAGE_NAME=e2fsprogs-libs-1.40.11
PACKAGE_WEB=http://sourceforge.net/projects/e2fsprogs
PACKAGE_URL=http://downloads.sourceforge.net/e2fsprogs/$(PACKAGE_NAME).tar.gz
PACKAGE_SOURCE=$(CURDIR)/$(PACKAGE_NAME).tar.gz
PACKAGE_BUILD_ROOT=$(TEMPDIR)/$(PACKAGE_NAME)
# Compile this package with small thumb code
ifdef USE_THUMB
CFLAGS := -mthumb -mthumb-interwork $(CFLAGS)
endif

$(PACKAGE_BUILD_ROOT): $(PACKAGE_SOURCE) e2fsprogs-libs.mak
	@echo "Building $(PACKAGE_NAME)"
	rm -rf $(PACKAGE_BUILD_ROOT)
	# This creates the PACKAGE_BUILD_ROOT dir
	cd $(TEMPDIR) && tar xvfz $(PACKAGE_SOURCE)
	# cd $(PACKAGE_BUILD_ROOT) && patch -p1 < $(PATCH1)
	cd $(PACKAGE_BUILD_ROOT) && ./configure --prefix=/usr \
	--with-root-prefix="" \
	--build=$(BUILD_PREFIX) \
	--host=$(HOST_PREFIX) \
	--target=$(TARGET_PREFIX) \
	--disable-swapfs
	$(MAKE) --directory=$(PACKAGE_BUILD_ROOT)

build: $(PACKAGE_BUILD_ROOT)
	@echo "Built $(PACKAGE_NAME)"

install: build
	cd $(PACKAGE_BUILD_ROOT) && $(MAKE) install DESTDIR=$(STAGING_AREA)
	@echo "e2fsprogs-libs is just static libraries so nothing to package."

clean:
	@rm -rf $(PACKAGE_BUILD_ROOT)

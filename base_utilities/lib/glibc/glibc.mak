# The glibc package is a bit different from the other
# packages. It is delivered precompiled with the GCC
# toolchain (this is the case with both Codesourcery's
# toolchain and the crosstool GCC variant) and thus we
# only collect and strip and package the glibc libraries
# from the cross-compiler environment.
#
# We don't copy the include headers. We are already set
# up to use these from the cross compiler location.
#
PACKAGE_NAME=glibc
PACKAGE_WEB=http://www.gnu.org/software/libc/
PACKAGE_URL=http://ftp.gnu.org/gnu/glibc/

# First locate all the librarhy files for /lib and /usr/lib
GLIBC_LIBFILES=$(shell find $(CROSSGLIBC) -type f)
# Some dummy libraries exist here (contain linker directives)
# so we write this that match libfoo.so.0.0.0 etc.
GLIBC_USRLIBFILES=$(shell find $(CROSSGLIBC)/../usr/lib -maxdepth 1 -type f | grep '\.so\.')
# Using "^" as a field delimiter here since it will later be made into
# a make-rule, this rules out some other non-possible filename characters
# like ":". The assignment here gives a list of linktarget_linksource
# elements to be processed by the makerule GLIBC_LIBLINKS.
GLIBC_LIBLINKS=$(shell find $(CROSSGLIBC) -type l -printf "%f^%l\n")
GLIBC_USRLIBLINKS=$(shell find $(CROSSGLIBC)/../usr/lib -type l -printf "%f^%l\n")

.PHONY: build
build:
	@echo "No building of $(PACKAGE_NAME)"

.PHONY: package-libdirs
package-libdirs:
	$(PACKAGE_DIR) /lib 755 0 0
	$(PACKAGE_DIR) /usr 755 0 0
	$(PACKAGE_DIR) /usr/lib 755 0 0

# Complicated rule to copy, strip and package all files
.PHONY: $(GLIBC_LIBFILES)
$(GLIBC_LIBFILES):
	@export FNAME=`basename $@` && \
	echo "Copy, strip and package $${FNAME}" && \
	cp $@ $(STAGING_AREA)/lib && \
	$(STRIP_SYMBOLS) $(STAGING_AREA)/lib/$${FNAME} && \
	$(PACKAGE_FILE) /lib/$${FNAME} $(STAGING_AREA)/lib/$${FNAME} 755 0 0

.PHONY: $(GLIBC_USRLIBFILES)
$(GLIBC_USRLIBFILES):
	@export FNAME=`basename $@` && \
	echo "Copy, strip and package $${FNAME}" && \
	cp $@ $(STAGING_AREA)/usr/lib && \
	$(STRIP_SYMBOLS) $(STAGING_AREA)/usr/lib/$${FNAME} && \
	$(PACKAGE_FILE) /usr/lib/$${FNAME} $(STAGING_AREA)/usr/lib/$${FNAME} 755 0 0

# Complicated rules to register all links
.PHONY: $(GLIBC_LIBLINKS)
$(GLIBC_LIBLINKS):
	@export LINKTARGET=`echo $@ | cut -d^ -f1` && \
	export LINKSOURCE=`echo $@ | cut -d^ -f2` && \
	$(PACKAGE_LINK) /lib/$${LINKTARGET} $${LINKSOURCE} 755 0 0

.PHONY: $(GLIBC_USRLIBLINKS)
$(GLIBC_USRLIBLINKS):
	@export LINKTARGET=`echo $@ | cut -d^ -f1` && \
	export LINKSOURCE=`echo $@ | cut -d^ -f2` && \
	$(PACKAGE_LINK) /usr/lib/$${LINKTARGET} $${LINKSOURCE} 755 0 0

install: build package-libdirs $(GLIBC_LIBFILES) $(GLIBC_USRLIBFILES) $(GLIBC_LIBLINKS) $(GLIBC_USRLIBLINKS)
	@echo "Installed package $(PACKAGE_NAME)"

clean:
	@echo "No cleaning for $(PACKAGE_NAME)"

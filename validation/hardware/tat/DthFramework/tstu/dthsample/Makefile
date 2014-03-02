#
#***************************************************************
#
# Synopsis: DTHSAMPLE Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

#$(shell echo>&2 'PATH: WARNING: "$(PATH)" used.')

DEPENDENCES=

PACKAGE_NAME:=libdthsample.so
PACKAGE_PATH=$(INSTALLATION_PATH_EXE)/9pserver/plugin
PACKAGE_ARCHIVE=
PACKAGE_ALIAS=$(PACKAGE_NAME)
PACKAGE_EXTRAVERSION=
PACKAGE_EXEC_NAME=

BINARIES=dthsample.o

.PHONY: unpack patch prepare clean compile install

#all: clean compile install

deps: _deps

unpack: .unpacked
.unpacked:
	echo "\t-----> $(PACKAGE_NAME) : $@"
	# Do you tar xzvf here or anything
	touch $@

patch: .patched
.patched: .unpacked
	echo "\t-----> $(PACKAGE_NAME) : $@"
	#Apply a patch
	touch $@

prepare: .prepared
.prepared: deps .patched
	echo "\t-----> $(PACKAGE_NAME) : $@"
	#Preprocessing command
	touch $@

clean: 
	echo "\t-----> $(PACKAGE_NAME) : $@"
	rm -f *.o *.so
	rm -f .compiled .installed .patched .prepared .unpacked

#
# create one library "libdthsample.so"
#
compile: .compiled 
.compiled: .prepared $(BINARIES)
	echo "\t-----> $(PACKAGE_NAME) : $@"
	$(CC) -shared -Wl,-soname,$(PACKAGE_NAME) $(BINARIES) -o $(PACKAGE_NAME)
#$(STRIP) output/$(PACKAGE_NAME)
	touch $@

%.o : %.c : .prepared
	$(CC) -c -g -fPIC $(CFLAGS) $^ -o $(@)


install: deps .compiled
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
	################################################
	##### Put in LIBRARIES_IMAGES8DIR shared files
	################################################
	echo -e "-> LIBRAY_PATH  : $(LIBRARY_IMAGES_DIR)"
	cp $(PACKAGE_NAME) $(LIBRARY_IMAGES_DIR)/$(PACKAGE_NAME)
	chmod +w $(LIBRARY_IMAGES_DIR)/$(PACKAGE_NAME)
	################################################
	#Preparation of the Filesystem
	################################################
	if test ! -d $(ROOTFS_INSTALL_DIR)/lib; \
	then \
		gunzip $(IMAGES_INSTALL_DIR)/initrd.gz; \
		sudo mount -o loop $(IMAGES_INSTALL_DIR)/initrd $(ROOTFS_INSTALL_DIR); \
	fi;
	# create PACKAGE_PATH directory in RootFS if not existing
	if test ! -d  $(ROOTFS_INSTALL_DIR)/$(PACKAGE_PATH); \
	then \
			sudo mkdir -p $(ROOTFS_INSTALL_DIR)/$(PACKAGE_PATH); \
	fi;    
	# copy library in RootFS/PACKAGE_PATH
	sudo cp $(PACKAGE_NAME) $(ROOTFS_INSTALL_DIR)/$(PACKAGE_PATH)
	sudo chown root $(ROOTFS_INSTALL_DIR)/$(PACKAGE_PATH)/$(PACKAGE_NAME)
	# create /usr/lib directory in RootFS if not existing
	if test ! -d  $(ROOTFS_INSTALL_DIR)/usr/lib; \
	then \
			sudo mkdir -p $(ROOTFS_INSTALL_DIR)/usr/lib; \
	fi;
	# create link in /usr/lib  so that library is visible from path
	if test ! -L  $(ROOTFS_INSTALL_DIR)/usr/lib/$(PACKAGE_NAME); \
	then \
			sudo ln -s /$(PACKAGE_PATH)/$(PACKAGE_NAME) $(ROOTFS_INSTALL_DIR)/usr/lib/$(PACKAGE_NAME); \
	fi;
	#Unmount, gzip the image with new directory and files
	sudo umount $(ROOTFS_INSTALL_DIR)
	gzip $(IMAGES_INSTALL_DIR)/initrd
	touch .$@ed


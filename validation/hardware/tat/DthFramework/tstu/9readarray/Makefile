#
#**********************************************************************
#
# Synopsis: 9READARRAY Makefile- Linux Application Build Rules.
#
#**********************************************************************
#

include .config.mk

#$(shell echo>&2 'PATH: WARNING: "$(PATH)" used.')

DEPENDENCES=

PACKAGE_NAME:=9readarray
PACKAGE_PATH=$(INSTALLATION_PATH_EXE)/generalservices/9pserver
PACKAGE_ARCHIVE=
PACKAGE_ALIAS=$(PACKAGE_NAME)
PACKAGE_EXTRAVERSION=
PACKAGE_EXEC_NAME=

BINARIES=9writearray.o

.PHONY: unpack patch prepare clean compile install

#all: clean compile install

deps: _deps

unpack: .unpacked
.unpacked:
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
	# Do you tar xzvf here or anything
	touch $@

patch: .patched
.patched: .unpacked
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
	#Apply a patch
	touch $@

prepare: .prepared
.prepared: deps .patched
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
	o 
	touch $@

clean: 
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
	rm -f *.o $(PACKAGE_NAME)
	rm -f .compiled .installed .patched .prepared .unpacked


#
# create one executable "dth9pserver"
#
compile: .compiled 
.compiled: .prepared $(BINARIES)
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
	$(CC) $(BINARIES) $(LDFLAGS) -o output/$(PACKAGE_NAME)  -lspfs
#$(STRIP) output/$(PACKAGE_NAME)
	touch $@

%.o : %.c : .prepared
	$(CC) -c -g $(CFLAGS) $^ -o $(@)


install: deps .compiled
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
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
	# copy executable in RootFS/PACKAGE_PATH
	sudo cp $(PACKAGE_NAME) $(ROOTFS_INSTALL_DIR)/$(PACKAGE_PATH)
	sudo chown root $(ROOTFS_INSTALL_DIR)/$(PACKAGE_PATH)/$(PACKAGE_NAME)
	# create /usr/bin directory in RootFS if not existing
	if test ! -d  $(ROOTFS_INSTALL_DIR)/usr/bin; \
	then \
			sudo mkdir -p $(ROOTFS_INSTALL_DIR)/usr/bin; \
	fi;
	# create link in /usr/bin so that executable are visible from path
	if test ! -L  $(ROOTFS_INSTALL_DIR)/usr/bin/$(PACKAGE_NAME); \
	then \
			sudo ln -s /$(PACKAGE_PATH)/$(PACKAGE_NAME) $(ROOTFS_INSTALL_DIR)/usr/bin/$(PACKAGE_NAME); \
	fi;
	#Unmount, gzip the image with new directory and files
	sudo umount $(ROOTFS_INSTALL_DIR)
	gzip $(IMAGES_INSTALL_DIR)/initrd
	touch .$@ed


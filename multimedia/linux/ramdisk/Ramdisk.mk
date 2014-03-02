#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

MMPLATFORMS := $(MMPLATFORM) $(COMPATPLAT)

################################################################################
# Some platforms dependant settings (should be moved to mmenv at some point)

STRIP = $(CROSS_COMPILE)strip

KERNEL_PLATFORM := $(PLATFORM)

ifeq ($(MMENV_SYSTEM),android)
  OUTPUT_BASE_DIR=$(BUILD_INTERMEDIATES_DIR)
else
  OUTPUT_BASE_DIR=$(PWD)
endif

################################################################################
# The ramdisk image file we will create
EXT2FILE=$(PLATFORM)_initrd
RAMDISK=$(EXT2FILE).gz
# The directory added to the RAMDISK
ROOTFSDIR=$(OUTPUT_BASE_DIR)/$(PLATFORM)_rootfs_dir
SYMBOLSDIR=$(OUTPUT_BASE_DIR)/$(PLATFORM)_symbols_dir
# The linuxshmkimg command script we'll generate and use to create the ramdisk
RAMFS_SCRIPT=$(OUTPUT_BASE_DIR)/$(PLATFORM)_ramfs_target.txt
# The directory containing changes to target files from the BSP RPMS
SRCTARGETDIR=$(PLATFORM)_target

TAR_BASE := tar --ignore-failed-read 
TAR      := $(TAR_BASE) --exclude .svn
ifeq ($(RAMDISK_COPY_FILES_TO_ANDROID),true)
  COPY = $(PWD)/mmcopy -S $(STRIP)
  ROOTFSDIR=$(ANDROID_ROOTFS_DIR)
  SYMBOLSDIR=$(ANDROID_ROOTFS_DIR)/symbols
  ifeq ($(RAMDISK_COPY_FILES_TO_LBP_ROOTFS),true)
    ROOTFSDIR=$(LBP_ROOTFS_DIR)
    SYMBOLSDIR=$(abspath $(LBP_ROOTFS_DIR)/../symbols)
  else
    COPY_CHECK = -A
  endif
  TO_BE_CLEANED=
else
  COPY = $(PWD)/mmcopy -S $(STRIP)
  TO_BE_CLEANED=$(ROOTFSDIR) 
endif

ifdef VERBOSE
  COPY += -v
endif

################################################################################

ifeq ($(MMENV_SYSTEM),android)
  SYSTEM_PREFIX=system
else
  SYSTEM_PREFIX=.
endif
KERNEL_MODULES_DIR=$(SYSTEM_PREFIX)/lib

## Following part is dealing with platforms having DSP
ifneq ($(MYMMDSPCORE),)
 ifneq ($(MMENV_BOARD),x86)
  ## The NMF components repository on the target system - no SIA or SVA variants
  ifeq ($(MMENV_SYSTEM),android)
    NMF_REPO_ROOT_DIR=/system/usr/share/nmf/repository
  else
    NMF_REPO_ROOT_DIR=/usr/share/nmf/repository
  endif
  ## Name of directory in which we install on the target the MMDSP NMF repository
  LINUX_NMF_REPO=$(NMF_REPO_ROOT_DIR)/$(MYMMDSPCORE)
  ## Directory in which all components have installed their MMDSP NMF repository
  MMTOOLS_CHIP_REPO=$(MM_NMF_REPO_BOARD_DIR)
 endif # ifneq ($(MMENV_BOARD),x86)
endif # ifneq ($(MMDSPCORE),)

################################################################################

ifeq ($(RAMDISK_COPY_FILES_TO_ANDROID),true)
  all: rootfsdir
else
  ifeq ($(MMENV_BOARD),x86)
    NO_RAMDISK:=true
  endif
  ifeq ($(NO_RAMDISK),true)
    ALL_TARGET=rootfsdir
  else
    ALL_TARGET=$(RAMDISK)
  endif

  all: $(ALL_TARGET) generate_setup generate_sync

  generate_sync:
  ifneq ($(MMENV_BOARD),x86)
	@echo "Generating sync-$(PLATFORM).sh"
    ifneq ($(MMDSPCORE),)
	$(VERCHAR)echo "echo -e \"\\033[46;30;4mSyncing NMF repository\\033[0m\"" > sync-$(PLATFORM).sh
	$(VERCHAR)echo "rsync -mur  $(ROOTFSDIR)/usr/share/nmf $${ROOTFS}/usr/share/  --progress --include \"*.elf4nmf\" --include \"preload_*.txt\" "  >> sync-$(PLATFORM).sh
    endif
	$(VERCHAR)echo "echo -e \"\\033[46;30;4mSyncing libs\\033[0m\"" >> sync-$(PLATFORM).sh
	$(VERCHAR)echo "rsync -mur  $(ROOTFSDIR)/usr/lib $${ROOTFS}/usr/  --progress --include \"*.so\" " >> sync-$(PLATFORM).sh
	$(VERCHAR)echo "echo -e \"\\033[46;30;4mSyncing mmte and mmte_bellagio\\033[0m\"" >> sync-$(PLATFORM).sh
	$(VERCHAR)echo "rsync -mur $(ROOTFSDIR)/bin/mmte $${ROOTFS}/bin/mmte" >> sync-$(PLATFORM).sh
	$(VERCHAR)echo "rsync -mur $(ROOTFSDIR)/bin/mmte_bellagio $${ROOTFS}/bin/mmte_bellagio" >> sync-$(PLATFORM).sh
	$(VERCHAR)echo "echo -e \"\\033[46;30;4mSyncing file system...\\033[0m\"" >> sync-$(PLATFORM).sh
	$(VERCHAR)echo "sync" >> sync-$(PLATFORM).sh
	$(VERCHAR)echo "echo -e \"\\033[46;30;4mDone\\033[0m\"" >> sync-$(PLATFORM).sh
  endif

  generate_setup:
	@echo "Generating setup-$(PLATFORM).sh"
  ifeq ($(MMENV_BOARD),x86)
        # Generate csh helper scripts
	$(VERCHAR)echo "setenv ROOTFS $(ROOTFSDIR)" > setup-$(PLATFORM).csh
	$(VERCHAR)echo 'setenv LD_LIBRARY_PATH $${ROOTFS}/lib:$${ROOTFS}/usr/lib:$${ROOTFS}/usr/lib/ste_omxcomponents:/lib:/usr/lib:/mali/lib:/usr/lib/ste_omxcomponents' >> setup-$(PLATFORM).csh
	$(VERCHAR)echo 'setenv PATH $${ROOTFS}/bin:/sbin:/usr/sbin:/bin:/usr/bin' >> setup-$(PLATFORM).csh
	$(VERCHAR)echo 'setenv OMX_BELLAGIO_REGISTRY $${ROOTFS}/omxcomponents' >> setup-$(PLATFORM).csh
	$(VERCHAR)echo 'setenv OMX_BELLAGIO_LOADER_REGISTRY $${ROOTFS}/omxloaders' >> setup-$(PLATFORM).csh
	$(VERCHAR)echo 'setenv OMX_STE_ENS_COMPONENTS_DIR $${ROOTFS}/usr/lib/ste_omxcomponents/' >> setup-$(PLATFORM).csh
  endif
        # Generate bash helper scripts
	$(VERCHAR)echo "echo Setting up environment to use build result in $(ROOTFSDIR)" > setup-$(PLATFORM).sh
	$(VERCHAR)echo "export ROOTFS=$(ROOTFSDIR)" >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'source $${ROOTFS}/../update-'"$(PLATFORM).sh" >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'export LD_LIBRARY_PATH=$${ROOTFS}/lib:$${ROOTFS}/usr/lib:$${ROOTFS}/usr/lib/ste_omxcomponents:/lib:/usr/lib:/mali/lib:/usr/lib/ste_omxcomponents' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'export PATH=$${ROOTFS}/bin:/sbin:/usr/sbin:/bin:/usr/bin' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'export OMX_BELLAGIO_REGISTRY=$${ROOTFS}/network-omxcomponents' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'export OMX_BELLAGIO_LOADER_REGISTRY=$${ROOTFS}/network-omxloaders' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'export OMX_STE_ENS_COMPONENTS_DIR=$${ROOTFS}/usr/lib/ste_omxcomponents/' >> setup-$(PLATFORM).sh
        # Fixup some path to use from network (fixing the files as one in ramdisk are in ramdisk and any update will overide changes)
	$(VERCHAR)echo 'if [ -e $${ROOTFS}/omxloaders ] ; then cat $${ROOTFS}/omxloaders | sed "s,^/usr/lib/,$${ROOTFS}/usr/lib/,g" > $${ROOTFS}/network-omxloaders ; fi' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'if [ -e $${ROOTFS}/omxcomponents ] ; then cat $${ROOTFS}/omxcomponents | sed "s,^/usr/lib/,$${ROOTFS}/usr/lib/,g" > $${ROOTFS}/network-omxcomponents ; fi' >> setup-$(PLATFORM).sh
  ifneq ($(MMENV_BOARD),x86)
    ifneq ($(MMDSPCORE),)
       # As the CM driver is loading MMDSP firmware from /usr/share/nmf/repository directory only, create a link from board to work environment
	$(VERCHAR)echo "echo Setting up NMF MPC repository" >> setup-$(PLATFORM).sh
	$(VERCHAR)echo "if [ -h $(LINUX_NMF_REPO) ] ; then rm -rf $(LINUX_NMF_REPO) ; fi" >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'ln -s $${ROOTFS}'"$(LINUX_NMF_REPO) $(LINUX_NMF_REPO)" >> setup-$(PLATFORM).sh
       # Install CM driver
	$(VERCHAR)echo "echo Reinstalling NMF CM drivers" >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'rmmod -f cm > /dev/null 2>&1' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'insmod $${ROOTFS}/lib/modules/`uname -r`/extra/cm.ko' >> setup-$(PLATFORM).sh
    endif
	$(VERCHAR)echo "echo Installation of imaging drivers" >> setup-$(PLATFORM).sh
	$(VERCHAR)echo "if [ -h /usr/share/camera ] ; then rm -rf /usr/share/camera ; fi" >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'ln -s $${ROOTFS}/usr/share/camera /usr/share/camera' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'rmmod -f st_mmio camera_flash > /dev/null 2>&1' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'insmod $${ROOTFS}/lib/modules/`uname -r`/extra/st_mmio.ko' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'insmod $${ROOTFS}/lib/modules/`uname -r`/extra/camera_flash.ko' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo "echo Installation SBAG and pcache drivers" >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'rmmod -f pcache_moda9mp sbag_mod > /dev/null 2>&1' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'insmod $${ROOTFS}/lib/modules/`uname -r`/extra/pcache_moda9mp.ko' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'insmod $${ROOTFS}/lib/modules/`uname -r`/extra/sbag_mod.ko' >> setup-$(PLATFORM).sh
	$(VERCHAR)echo "lsmod" >> setup-$(PLATFORM).sh
	$(VERCHAR)echo 'if [ -d $${ROOTFS}/share/oprofile ] ; then mkdir -p /share ; cp -af $${ROOTFS}/share/oprofile /share ; fi' >> setup-$(PLATFORM).sh
  endif
	$(VERCHAR)echo "echo Environment setup done" >> setup-$(PLATFORM).sh
endif

CMD_POPULATE_DIRS=./genext2fs -d $(ROOTFSDIR) -x $(EXT2FILE) $(EXT2FILE)-tmp
CMD_CREATE_DEVICES=./genext2fs -D $(RAMFS_SCRIPT) -x $(EXT2FILE)-tmp $(EXT2FILE)
CMD_EXT2_TO_EXT3=tune2fs -j $(EXT2FILE)

$(RAMFS_SCRIPT):
	$(VERCHAR)touch $@

$(RAMDISK): rootfsdir $(RAMFS_SCRIPT)
	@echo "Generating $(RAMDISK)"
        #create /linuxrc as link to busybox
	$(VERCHAR)cd $(ROOTFSDIR) ; ln -fs /bin/busybox linuxrc ; mkdir -p dev/block/
	$(VERCHAR)if [ -d  $(SRCTARGETDIR) ] ; then $(TAR) -c -C $(SRCTARGETDIR) . | $(TAR) -x -C $(ROOTFSDIR) ; fi
	$(VERCHAR)if [ -e $(HOME)/.myrcSBB ] ; then cp -f $(HOME)/.myrcSBB $(ROOTFSDIR) ; fi
	$(VERCHAR)if [ -e $(HOME)/.credentials.txt ] ; then cp -f $(HOME)/.credentials.txt $(ROOTFSDIR) ; fi
	$(VERCHAR)cat devices.ramdisk.txt >> $(RAMFS_SCRIPT)
	$(VERCHAR)gunzip -c CortexA9-linux_base_initrd.gz > $(EXT2FILE)
        # First populate with dirs
	@echo "$(CMD_POPULATE_DIRS)"
	$(VERCHAR)$(CMD_POPULATE_DIRS) ; if [ $$? -ne 0 ] ; then echo "\033[1m\033[31m\n **** Error: failed to populate rootfs **** \033[0m\n" ; fi
        # Then create the devices
	@echo "$(CMD_CREATE_DEVICES)"
	$(VERCHAR)$(CMD_CREATE_DEVICES) ; if [ $$? -ne 0 ] ; then echo "\033[1m\033[31m\n **** Error: failed to create the devices **** \033[0m\n" ; fi
	rm -rf $(EXT2FILE)-tmp
        # turn FS into ext3
	@echo "$(CMD_EXT2_TO_EXT3)"
	$(VERCHAR)$(CMD_EXT2_TO_EXT3) ; if [ $$? -ne 0 ] ; then echo "\033[1m\033[31m\n **** Error: tune2fs failed **** \033[0m\n" ; fi
	cp -f $(EXT2FILE) rootfs.img
	gzip -f $(EXT2FILE)
	chmod 777 $(RAMDISK)
	$(VERCHAR)rm -rf $(RAMFS_SCRIPT)

%.dump:
	./dumpext2fs $*

clean:
	$(VERCHAR)rm -rf $(TO_BE_CLEANED) $(RAMFS_SCRIPT) $(RAMDISK) $(RAMDISK).content $(RAMDISK).dump $(RAMDISK)_unzip initrd.gz.tmp.1 initrd.gz.dir /tmp/initrd.gz.dir /tmp/initrd.gz.tmp.1 *_initrd

realclean: clean 

################################################################################
## ROOTFSDIR populating

ROOTFS_CONTENT = all_content export_dictionnaries

ifneq ($(MMTOOLS_CHIP_REPO),)
  ROOTFS_CONTENT += nmf_content
endif

ROOTFS_CONTENT += bellagio_omxcomponents

BELLAGIO_SEARCH=$(shell grep -c BELLAGIO_REGISTRY $(ROOTFSDIR)/etc/profile)

.PHONY: all_content nmf_content bellagio_omxcomponents

all_content:
# Add Exports for bellagio on lbp
ifeq ($(RAMDISK_COPY_FILES_TO_LBP_ROOTFS),true)
  ifeq ($(BELLAGIO_SEARCH),0)
	$(VERCHAR)if [ -e $(ROOTFSDIR)/etc/profile ] ; then echo "export OMX_BELLAGIO_REGISTRY=/omxcomponents" >> $(ROOTFSDIR)/etc/profile; echo "export OMX_BELLAGIO_LOADER_REGISTRY=/omxloaders">> $(ROOTFSDIR)/etc/profile; echo "export LD_LIBRARY_PATH=/lib:/usr/lib:/mali/lib:/usr/lib/ste_omxcomponents" >> $(ROOTFSDIR)/etc/profile; fi
  endif
endif
        # Dereference links as generic makefiles create links
	@for p in $(MMPLATFORMS) ; do for s in bin lib ; do \
	  $(COPY) -L $(MMBUILDIN)/$$s/$$p $(ROOTFSDIR)/$(SYSTEM_PREFIX)/$$s $(SYMBOLSDIR)/$(SYSTEM_PREFIX)/$$s ; \
        done ; done
	@for p in $(MMPLATFORMS) ; do \
	  $(COPY) -L $(MMBUILDIN)/lib/$$p/lib/modules $(ROOTFSDIR)/$(SYSTEM_PREFIX)/lib/modules $(SYMBOLSDIR)/$(SYSTEM_PREFIX)/lib/modules ; \
        done
	@for p in $(MMPLATFORMS) ; do \
	  $(COPY) -L $(COPY_CHECK) $(MMBUILDIN)/rootfs/$$p $(ROOTFSDIR) $(SYMBOLSDIR); \
        done
        ## We do not dereference links for install dir
	@for p in $(MMPLATFORMS) ; do \
	  $(COPY) $(COPY_CHECK) $(MMBUILDIN)/install/$$p $(ROOTFSDIR) $(SYMBOLSDIR) ; \
        done
        # Then copy kernel modules if exist
	@if [ -d $(MMROOT)/linux/kernel/$(KERNEL_PLATFORM) ] ; then \
	  $(COPY) $(COPY_CHECK) $(MMROOT)/linux/kernel/$(KERNEL_PLATFORM)/lib $(ROOTFSDIR)/lib $(SYMBOLSDIR)/lib ; fi
        ## Move the kernel modules libs if any
	@if [ -d $(ROOTFSDIR)/lib/lib/modules ] ; then \
	  $(COPY) $(COPY_CHECK) $(ROOTFSDIR)/lib/lib/modules $(ROOTFSDIR)/$(KERNEL_MODULES_DIR)/modules $(SYMBOLSDIR)/$(KERNEL_MODULES_DIR) ; \
	  rm -rf $(ROOTFSDIR)/lib/lib ; fi

ifneq ($(INSTALL_MM_VARIANT),)
  COPY_NMF_VARIANT = -V $(MMTOOLS_CHIP_REPO)$(INSTALL_MM_VARIANT)
endif

nmf_content: # Dereference links for now as OSI create links
	@$(COPY) -N $(COPY_NMF_VARIANT) $(MMTOOLS_CHIP_REPO) $(ROOTFSDIR)$(LINUX_NMF_REPO)


export_dictionnaries:
	tar zcf  $(ROOTFSDIR)/system/usr/share/ste_dicos.tgz -C $(MM_DICO_DIR) .

## Add the bellagio components to the component registry list
bellagio_omxcomponents:
	@rm -rf $(ROOTFSDIR)/$(SYSTEM_PREFIX)/omxcomponents;
	@if [ -f $(ROOTFSDIR)/$(SYSTEM_PREFIX)/lib/libste_displaysink.so ] ; \
          then touch $(ROOTFSDIR)/$(SYSTEM_PREFIX)/omxcomponents ; \
          echo "/$(SYSTEM_PREFIX)/lib/libste_displaysink.so" >> $(ROOTFSDIR)/$(SYSTEM_PREFIX)/omxcomponents ; \
        fi
	@if [ -f $(ROOTFSDIR)/$(SYSTEM_PREFIX)/lib/libhantrovideodec.so ] ; \
          then touch $(ROOTFSDIR)/$(SYSTEM_PREFIX)/omxcomponents ; \
          echo "/$(SYSTEM_PREFIX)/lib/libhantrovideodec.so" >> $(ROOTFSDIR)/$(SYSTEM_PREFIX)/omxcomponents ; \
        fi
	@if [ -f $(ROOTFSDIR)/$(SYSTEM_PREFIX)/lib/libste_enc_h264_hva.so ] ; \
          then touch $(ROOTFSDIR)/$(SYSTEM_PREFIX)/omxcomponents ; \
          echo "/$(SYSTEM_PREFIX)/lib/libste_enc_h264_hva.so" >> $(ROOTFSDIR)/$(SYSTEM_PREFIX)/omxcomponents ; \
        fi
	@if [ -f $(ROOTFSDIR)/$(SYSTEM_PREFIX)/lib/libste_dec_h264_hva.so ] ; \
          then touch $(ROOTFSDIR)/$(SYSTEM_PREFIX)/omxcomponents ; \
          echo "/$(SYSTEM_PREFIX)/lib/libste_dec_h264_hva.so" >> $(ROOTFSDIR)/$(SYSTEM_PREFIX)/omxcomponents ; \
        fi

## We populate the root dir with what has been build
rootfsdir: $(ROOTFS_CONTENT)

################################################################################

## Include valid related targets
-include MMValid.mk

################################################################################

p-%:
	@echo $*=$($*)

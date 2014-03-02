#Android makefile to build the reference manual as a part of Android Build

# Give other modules a nice, symbolic name to use as a dependent
.PHONY: build-refman clean-refman


PRIVATE_REFMAN_ARGS := -C $(TOOLS_PATH)/platform/RefMan KERNEL_FLAGS="ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) CROSS_PREFIX=arm-eabi" KERNELDIR=$(CURDIR)/kernel TOPLEVEL=$(CURDIR)/product/common PROJECTROOTDIR=$(CURDIR) TARGET_PRODUCT=$(TARGET_PRODUCT) ANDROID=true PRODUCT_OUT=$(abspath $(PRODUCT_OUT)/API_manual/RefMan) IMAGE_OUTPUT=$(abspath $(PRODUCT_OUT)/API_manual) UML_EXPORT_XML=$(REFMAN_SET_UML_EXPORT_XML)

ifeq ($(REFMAN_ENABLE_FEATURE_REFMAN_GEN),true)
ALL_DOCS += build-refman
endif

# Builds and installs the reference manual.
build-refman:
	$(MAKE) $(PRIVATE_REFMAN_ARGS) -f refman.mak all


# An Android clean removes the files built for the current HW configuration,
# such as u8500,
# while a clobber removes all built files (rm -rf $(OUT_DIR)).
# Reference manual only has one build tree, so clean and clobber will be
# the same.

clean clobber : clean-refman

clean-refman:
	$(MAKE) $(PRIVATE_REFMAN_ARGS) -f refman.mak distclean

installclean: installclean-refman

installclean-refman:
#	rm -f u-boot.bin u-boot.map u-boot.srec

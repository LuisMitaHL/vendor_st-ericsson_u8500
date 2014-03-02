#!/bin/bash

# Collect all loadmodules in a folder named after target product and kernel
# configuration
#
# The arguments to this script contain the environment variables that need
# to be set in order for this script to function properly, specifically
# $TOP and $KERNEL_DEFCONFIG

# Make any environment variables on the command line available.
for i in $@; do
	export $i
done

IMAGE_OUTPUT=${TOP}/images/${TARGET_PRODUCT}_android_${KERNEL_DEFCONFIG}
KERNEL_PATH=${KERNEL_OUTPUT}
FSIMAGE_PATH=../../${PRODUCT_OUT}
MODEM_PATH=../../${PRODUCT_OUT}/modem_images
USB_PC_DRIVERS_SET_DRIVERS=composite_device_driver/${USB_PC_DRIVERS_SET_DRIVERS}

echo ""
echo "Collecting flashkit and file system images to a common folder..."
echo ""

# Create destination directory
mkdir -p ${IMAGE_OUTPUT}

# Symlink file system images instead of copying them due to their sizes
# Symlink the rest of the binaries so that there is no reason to re-run
# collect-loadmodules.sh befor flashing in case a single modules has
# been re-built.

# Kernel must be symlinked to both in debug and non-debug variants
if [ -e ${KERNEL_PATH}/vmlinux ]; then
  ln -fs ../../${KERNEL_PATH}/vmlinux ${IMAGE_OUTPUT}
else
  echo "WARNING: ${KERNEL_PATH}/vmlinux not found"
fi
if [ -e ${PRODUCT_OUT}/uImage ]; then
  ln -fs ../../${PRODUCT_OUT}/uImage ${IMAGE_OUTPUT}
else
  echo "WARNING: ${PRODUCT_OUT}/uImage not found"
fi
if [ -e ${PRODUCT_OUT}/zImage ]; then
  ln -fs ../../${PRODUCT_OUT}/zImage ${IMAGE_OUTPUT}
else
  echo "WARNING: ${PRODUCT_OUT}/zImage not found"
fi

# When symlinking u-boot do not forget its environment
# image and startup graphic image
if [ -e ${PRODUCT_OUT}/u-boot.bin ]; then
  ln -fs ../../${PRODUCT_OUT}/u-boot.bin ${IMAGE_OUTPUT}
else
  echo "WARNING: ${PRODUCT_OUT}/u-boot.bin not found"
fi
if [ -e ${PRODUCT_OUT}/u-boot-env.bin ]; then
  ln -fs ../../${PRODUCT_OUT}/u-boot-env.bin ${IMAGE_OUTPUT}
else
  echo "WARNING: ${PRODUCT_OUT}/u-boot-env.bin not found"
fi

# Symlink lk images if they exist
# When symlinking lk do not forget its environment
if [ -e ${PRODUCT_OUT}/lk.bin ]; then
  ln -fs ../../${PRODUCT_OUT}/lk.bin ${IMAGE_OUTPUT}
fi
if [ -e ${PRODUCT_OUT}/lk_env.bin ]; then
  ln -fs ../../${PRODUCT_OUT}/lk_env.bin ${IMAGE_OUTPUT}
fi
if [ -e ${PRODUCT_OUT}/${SPLASH_IMAGE_PATH} ]; then
  if [[ $SPLASH_IMAGE_PATH == */* ]]; then
  SPLASH_DIRS=${SPLASH_IMAGE_PATH//\// }
  SPLASH_DIRS=($SPLASH_DIRS)
  for n in $(seq 0 $((${#SPLASH_DIRS[*]} - 2)))
  do
  SPLASH_PATH=${SPLASH_PATH}/${SPLASH_DIRS[n]}
  SPLASH_OUT=${SPLASH_OUT}/..
  mkdir -p ${IMAGE_OUTPUT}/${SPLASH_PATH}
  done
  fi
  ln -fs ../../${SPLASH_OUT}/${PRODUCT_OUT}/${SPLASH_IMAGE_PATH} ${IMAGE_OUTPUT}/${SPLASH_IMAGE_PATH}
else
  echo "WARNING: Splash image ${SPLASH_IMAGE_PATH} not found"
fi

ln -fs ${FSIMAGE_PATH}/system.img ${IMAGE_OUTPUT}/
ln -fs ${FSIMAGE_PATH}/ramdisk.img ${IMAGE_OUTPUT}/
ln -fs ${FSIMAGE_PATH}/userdata.img ${IMAGE_OUTPUT}/
ln -fs ${FSIMAGE_PATH}/cache.img ${IMAGE_OUTPUT}/
if [ -e ${PRODUCT_OUT}/boot.img ]; then
  ln -fs ${FSIMAGE_PATH}/boot.img ${IMAGE_OUTPUT}/
fi
if [ -e ${PRODUCT_OUT}/recovery.img ]; then
  ln -fs ${FSIMAGE_PATH}/recovery.img ${IMAGE_OUTPUT}/
fi
if [ -e ${PRODUCT_OUT}/modemfs.img ]; then
  ln -fs ${FSIMAGE_PATH}/modemfs.img ${IMAGE_OUTPUT}/
fi
if [ -e ${PRODUCT_OUT}/misc.img ]; then
  ln -fs ${FSIMAGE_PATH}/misc.img ${IMAGE_OUTPUT}/
fi
if [ -e ${PRODUCT_OUT}/hats.img ]; then
  ln -fs ${FSIMAGE_PATH}/hats.img ${IMAGE_OUTPUT}/
fi
# Symlink modem image directory
if [ -e ${PRODUCT_OUT}/modem_images ]; then
  ln -fs ${MODEM_PATH} ${IMAGE_OUTPUT}/
fi

# Symlink cspsa images if they exist. If they do not exist they should be taken from the HSC.
if [ ${DEFAULT_CSPSA_IMAGES} ]; then
	for image in ${DEFAULT_CSPSA_IMAGES//$DELIMITER/ };
	do
	ln -fs ../../${PRODUCT_OUT}/${image} ${IMAGE_OUTPUT}/
	done
fi

# Symlink ITP image to IMAGE_OUTPUT if it exist.
if [ -e ${TOP}/production/itp/itp_prebuilt/u8500 ]; then
  if [ -e ${TOP}/production/itp/itp_prebuilt/u8500/itp_app_U8500.bin ]; then
    ln -fs ../../production/itp/itp_prebuilt/u8500/itp_app_U8500.bin ${IMAGE_OUTPUT}/itp_U8500.bin
  fi
  if [ -e ${TOP}/production/itp/itp_prebuilt/u8500/itp_app_U8520.bin ]; then
    ln -fs ../../production/itp/itp_prebuilt/u8500/itp_app_U8520.bin ${IMAGE_OUTPUT}/itp_U8520.bin
  fi
fi

# Symlink to Windows USB Host Drivers if it exist.
if [ -e ${PRODUCT_OUT}/${USB_PC_DRIVERS_SET_DRIVERS} ]; then
  ln -fs ../../${PRODUCT_OUT}/composite_device_driver ${IMAGE_OUTPUT}
fi

FLASHKIT_COMPONENTS="signing boot flashkit loaders preflash_tool specialbootscripts flasharchive.xml flashlayout.txt sw_type_map_no_signing.txt sw_type_map.txt sw_type_map_hz3.txt sw_type_map_hz3_basic.txt config.list flasher.bat readme-flasher _filelist_linux.txt _filelist_uboot.txt _filelist_flashlayout.txt _filelist_kernel.txt _filelist_cspsa.txt _filelist_dntcert.txt _filelist_itp.txt"
for name in ${FLASHKIT_COMPONENTS};
do
	if [ -e ${FLASHKIT_INSTALL_PATH}/${name} ]; then
		ln -fs ../../${FLASHKIT_INSTALL_PATH}/${name} ${IMAGE_OUTPUT}
	else
		echo "INFORMATION: ${FLASHKIT_INSTALL_PATH}/${name} not found. If flashkit has been excluded from the build this is not an error."
	fi
done

#This needs to be copied otherwise the relative paths will be relative
#to the original file and not the symlinked one
if [ -e ${FLASHKIT_INSTALL_PATH}/flasher ]; then
	cp -fr ${FLASHKIT_INSTALL_PATH}/flasher ${IMAGE_OUTPUT}
else
	echo "INFORMATION: ${FLASHKIT_INSTALL_PATH}/flasher not found. If flashkit has been excluded from the build this is not an error."
fi
if [ -e ${FLASHKIT_INSTALL_PATH}/_filelist_complete.txt  ]; then
	cp -fr ${FLASHKIT_INSTALL_PATH}/_filelist_complete.txt  ${IMAGE_OUTPUT}
else
	echo "INFORMATION: ${FLASHKIT_INSTALL_PATH}/_filelist_complete.txt  not found. If flashkit has been excluded from the build this is not an error."
fi

# Symlink modem binary, debug elfs, and flashtools
if [ -e ${PRODUCT_OUT}/modem ]; then
  ln -fs ../../${PRODUCT_OUT}/modem ${IMAGE_OUTPUT}
fi

#Copy make_ext4fs needed to create modemfs from Windows in U9540
if [ -e ${HOST_OUT_EXECUTABLES}/make_ext4fs.exe ]; then
	cp -f ${HOST_OUT_EXECUTABLES}/make_ext4fs* ${IMAGE_OUTPUT}
else
	echo "INFORMATION: ${HOST_OUT_EXECUTABLES}/make_ext4fs.ex not found. If make_ext4fs.exe has been excluded from the build this is not an error."
fi

echo ""
echo "Kernel, u-boot, file system images and flashkit collected in:"
echo "${IMAGE_OUTPUT}/"
echo ""

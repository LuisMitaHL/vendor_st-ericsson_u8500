#!/bin/sh
#
# There are several variables that have been defined prior to executing
# this script (by apply_fs_patches.sh). All paths are relative to the
# Android top level directory which is also the current directory when
# this script is executed.
#
# TOPLEVEL is the absolute path to the Android top directory.
#
# VOLUME is the name of the file system volume, e.g. "system".
#
# VOLUMEDIR is the path to the directory tree whose contentes will make up the
# contentes of the named file system image.
#
# PATCHDIR is the path to the directory where this file system patch is located.
#
# KERNELDIR is the path to the kernel directory.
#
# INSTALL_MOD_PATH is the path to the root directory that will
# make up the system partition and thus needs to contain the kernel modules.

#
#touch ${VOLUMEDIR}/test-system-patch-applied

if [ -n "${HATS_PATH}" ]
then

HATS_RAMDISK=${TOPLEVEL}/${VOLUMEDIR}/../root
mkdir -p ${TOPLEVEL}/${VOLUMEDIR}
mkdir -p ${HATS_RAMDISK}/hats

echo "HATS tuning for $HATS_RAMDISK before FS generation..."

# Install HATS busybox into ramdisk partition
make -C ${TOPLEVEL}/${HATS_PATH}/busybox install_root

# Install rcS
cp  ${TOPLEVEL}/${HATS_PATH}/ramdisk/init.hats.rc ${HATS_RAMDISK}/init.hats.rc

fi

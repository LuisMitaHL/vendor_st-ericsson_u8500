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

HATS_ROOT=${TOPLEVEL}/${VOLUMEDIR}/hats

echo "HATS tuning for $HATS_ROOT before FS generation..."

# Copy kernel modules into HATS part
ln -sf /system/lib/modules ${HATS_ROOT}/lib/modules

# Link firmwares into HATS part
for file in `ls ${TOPLEVEL}/${VOLUMEDIR}/etc/firmware`
do
                ln -sf /system/etc/firmware/$file ${HATS_ROOT}/lib/firmware/$file
done

# Copy MM stuffs from ramdisk to system
mkdir -p ${HATS_ROOT}/usr/share/mm-valid
cp -R ${TOPLEVEL}/${VOLUMEDIR}/../root/usr/share/mm-valid/* ${HATS_ROOT}/usr/share/mm-valid/.
cp -R ${TOPLEVEL}/${VOLUMEDIR}/../root/omxcomponents ${TOPLEVEL}/${VOLUMEDIR}/.

fi

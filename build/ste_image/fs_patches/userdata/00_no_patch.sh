#!/bin/sh
#
# Example of a file system patch
#
# DON'T REMOVE THIS FILE OR YOU WILL GET AN ERROR MESSAGE
# ABOUT MISSING FILES IN APPLY_FS_PATCHES.SH
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
# VOLUMEDIR is the path to the directory tree whose contents will make up the
# contents of the named file system image.
#
# PATCHDIR is the path to the directory where this file system patch is located.
#
# KERNELDIR is the path to the kernel directory.
#
# INSTALL_MOD_PATH is the path to the root directory that will
# make up the system partition and thus needs to contain the kernel modules.

# Sample file system patch command:
#touch ${VOLUMEDIR}/sample-patch-applied

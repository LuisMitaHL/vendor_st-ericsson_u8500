#!/bin/sh
#
# Apply the file system patches for a specific file system volume.
#
# The arguments to this script contain the environment variables that need
# to be set in order for the scripts containing the patches to be applied.
#
# It is assumed that the following two variables be set using paths
# relative to the Android top level directory:
#
# PATCHDIR is the path to the directory containing the file system patches
# to be applied for the specific file system volume.
#
# VOLUMEDIR is the path to the directory tree containing the contents used
# to generate the file system image for the given volume. This path should
# as its only or trailing path component contain the name of the file
# system volume whose patches should be applied, e.g. "system", "userdata",
# etc.

# Make any environment variables on the command line available to the patches.
for i in $@; do
	export $i
done

# Retrieve the path to the directory tree and the file system volume name.
# Also make this environment variable available to the patches.
export VOLUME=`echo "${VOLUMEDIR}" | rev | cut -d\/ -f1 | rev`

# Apply filesystem patches to the output diretory tree.
for i in `ls ${PATCHDIR}/*.sh | sort -n`; do
	. $i
done

exit 0

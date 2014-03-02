#!/bin/sh
#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

## When no options are provided then default to Meego setup

MMINSTALLDIR=/usr/share/ste-mm-devel
MMHEADERDIR=/usr/include/ste-mm

MKDEFINES=""

## PLATFORM to build for is u8500_v2-linux
MKDEFINES="${MKDEFINES} PLATFORM=u8500_v2-linux"

## MMROOT set to upper dir of component's sources
MKDEFINES="${MKDEFINES} MMROOT=$RPM_BUILD_DIR/multimedia"

## Root dir under which we find our dependencies
MKDEFINES="${MKDEFINES} MMBUILDIN=$MMINSTALLDIR"

## Default install directory
MKDEFINES="${MKDEFINES} MMBUILDOUT=$RPM_BUILD_ROOT$MMINSTALLDIR"

## MM_MAKEFILES_DIR where to find installed make related files
MKDEFINES="${MKDEFINES} MM_MAKEFILES_DIR=$MMINSTALLDIR/mkfiles"

## Where to install the headers
MKDEFINES="${MKDEFINES} INSTALL_HEADER_BASEDIR=$RPM_BUILD_ROOT$MMHEADERDIR"

## Where to find the installed headers
MKDEFINES="${MKDEFINES} MM_HEADER_BASEDIR=$MMHEADERDIR"

## Building with makefiles backward compatibity to no
MKDEFINES="${MKDEFINES} MAKEFILES_BACKWARD_COMPATIBLE=no"

## Do not generate mminstall-files.txt
MKDEFINES="${MKDEFINES} MMINSTALL_GENERATE_MMINSTALL_FILES=no"

## Copy files instead of creating links
MKDEFINES="${MKDEFINES} MMINSTALL_USES_INSTALL=yes"

## Not using any build tools prefix
MKDEFINES="${MKDEFINES} CROSS_COMPILE= "

## Force meego defines
MKDEFINES="${MKDEFINES} BUILDING_FOR_MEEGO=yes"

## Directory where to find the kernel sources to build our modules
MKDEFINES="${MKDEFINES} KERNEL_BUILD_DIR=/lib/modules/`uname -r`/build"

## Not building MMDSP, RVCT and not running OSTTrace compiler
MKDEFINES="${MKDEFINES} FORCEBUILD=pack_8500_linux"

## Imaging configuration to use
MKDEFINES="${MKDEFINES} IMG_CONFIG=401"

## Enable makefiles verbosity
MKDEFINES="${MKDEFINES} VERBOSE=1"

#### Send out the build flags
echo -n $MKDEFINES

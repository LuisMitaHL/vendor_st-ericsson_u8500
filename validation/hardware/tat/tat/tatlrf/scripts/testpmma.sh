#! /bin/sh
#	Copyright (C) ST-Ericsson 2011
#	This script executes complete PMM access tests by performing predefined
#   operations and bound checking for the specified element and its children.
#	Author:	alain.crouzet-nonst@stericsson.com

# make use of dthadv.sh
. ./dthadv.sh

local ROOTDIR=""
if [ $# -eq 1 ]; then
	ROOTDIR=$1
elif [ $# -lt 1 ]; then
	ROOTDIR="RF/Param"
else
	echo "syntax: nrpmmacc.sh dth_dir"
	echo "if not set, dth_dir equals 'RF/Param' which will run test for all RF parameters"
    echo "the test take a few minutes for large amount of RF parameters to test"
	exit 1
fi

local PMMACCESS=$DTH9P/RF/Parameters/MemoryMgt/Action/Memory_access

date

# set mode WRITE else we may be enable to write data to PMM
dth_get $PMMACCESS
local OLDPMMACCESS=$VAR_GET_RESULT
dth_set $PMMACCESS 1

# run test for all RF parameters
./testdtha.sh $ROOTDIR

# restore PMM access
dth_set $PMMACCESS $OLDPMMACCESS

date

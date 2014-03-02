#!/bin/bash

#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2010 ST-Ericsson. All rights reserved.                *
# *  This code is ST-Ericsson proprietary and confidential.              *
# *  Any use of the code for whatever purpose is subject to              *
# *  specific written permission of ST-Ericsson SA.                      *
# *                                                                      *
# *  Author: Mattias Bramsvik <mattias.xx.bramsvik@stericsson.com>       *
# *                                                                      *
# ************************************************************************/

# ------------------------------------------------------------------------------
# - Local settings
# ------------------------------------------------------------------------------
NAME="u8500_default_cspsa"
MODEM_PATH="./nwm"
PFF_PATH=pff
PFF_MODEM_PATH=$PFF_PATH/0x00000000-0x0000FFFF.modem

# ------------------------------------------------------------------------------
# - Generic settings
# ------------------------------------------------------------------------------
MODEM_GDF_FILE=$NAME"_nwm.gdf"
BIN_NAME=$NAME".bin"
LIST_NAME=$NAME".gdf"

# ------------------------------------------------------------------------------
# - Executables
# ------------------------------------------------------------------------------
EXE_PATH="../cspsa-tools/"
NWM2GDF=$EXE_PATH"nwm2gdf"
GDF2PFF=$EXE_PATH"gdf2pff"
PFF2CSPSA=$EXE_PATH"pff2cspsa"
CSPSALIST=$EXE_PATH"cspsalist"

# ------------------------------------------------------------------------------
# - Assisting functions
# ------------------------------------------------------------------------------

function checkexit()
{
	local returnvale=$1

	if [ "$returnvale" -ne "0" ]; then
		echo ...failed...
	exit 1
	fi
}

function check_exe_exist()
{
	if [ ! -x $1 ]
	then
		echo "File '" $1 "' not found or is not executable. Please build Android before running this script."
	exit 1
	fi
}

function check_file_exist()
{
	if [ ! -f $1 ]
	then
		echo "File '" $1 "' not found."
	exit 1
	fi
}

# ------------------------------------------------------------------------------
# - Script start
# ------------------------------------------------------------------------------

# - Build CSPSA tools.
make -C $EXE_PATH -f Makefile all

# ------------------------------------------------------------------------------

# - Create RFHAL directory, should be available in modemfs as an empty directory
mkdir -p $MODEM_PATH/modemfs/RFHAL

# - Parse directory with NWM modem configuration files and create a GDF file.
check_exe_exist $NWM2GDF
[ ! -d $MODEM_PATH ] && { "Directory '" $MODEM_PATH "' not found";	exit 1; }
[ -f $MODEM_GDF_FILE ] && rm $MODEM_GDF_FILE
$NWM2GDF -o $MODEM_GDF_FILE $MODEM_PATH
checkexit $?
check_file_exist $MODEM_GDF_FILE

# ------------------------------------------------------------------------------

# - If PFF structure does not exist, create basic folders.
mkdir -p $PFF_MODEM_PATH
rm -f $PFF_MODEM_PATH/*
# - Parse NWM GDF file and generate PFF files.
check_exe_exist $GDF2PFF
$GDF2PFF -p $PFF_MODEM_PATH -n modem $MODEM_GDF_FILE
checkexit $?

# ------------------------------------------------------------------------------

# - Parse PFF file structure and create CSPSA binary image.
check_exe_exist $PFF2CSPSA
[ -f $BIN_NAME ] && rm $BIN_NAME
$PFF2CSPSA -t 0xFFFFE000 -f $BIN_NAME -z 0x100000 pff
checkexit $?
check_file_exist $BIN_NAME

# ------------------------------------------------------------------------------

# - Parse CSPSA binary image and create a text file with it's contents. The
#   list file can be used as manual check that the intended change got the proper
#   effect.
check_exe_exist $CSPSALIST
[ -f $LIST_NAME ] && rm $LIST_NAME
$CSPSALIST -o $LIST_NAME $BIN_NAME
checkexit $?
check_file_exist $LIST_NAME

# ------------------------------------------------------------------------------

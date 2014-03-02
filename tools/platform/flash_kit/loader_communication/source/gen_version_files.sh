#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Copyright (C) ST-Ericsson SA 2012
# License terms: 3-clause BSD license
# ------------------------------------------------------------------------------

#-------- Global vars init values -----------

#script version
SCRIPT_VERSION="v1.0"

#printout stream channels
error_channel=/dev/stderr
normal_channel=/dev/stdout
stream_channel=$normal_channel

#git revision info
VERSION=""

chdir=$3
path=$2
LCD_RC_PATH=$path/LcdVersion.cpp
LCM_RC_PATH=$path/lcm_version.c

# ------------------------------INTERNAL FUNCTIONS ----------------------------

function show_err()
{
  cat >> $stream_channel << INLINE_DOC

Syntax ERROR!
Try reading the script help info
$0 --help

INLINE_DOC
}

function get_help()
{
  cat >> $stream_channel << INLINE_DOC
================================ HELP ================================
The purpose of this script is generating source (.c or .cpp) files
that will hold information for the compatibility of the versions of
LCM and LCD .dll files. First input parameter is --lcm or --lcd
which tells the script which file to be generated.
Second parameter is the path where the file should be generated.
Third parameter is the location of the directory which holds the .git repository.

INLINE_DOC
}

function get_init_data()
{
   cd $chdir
   VERSION=`git describe --tags --always --long`
}

#
#  Loader Communication module
#
function process_lcm()
{
   cat > $LCM_RC_PATH << INLINE_DOC
/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2012
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 * @addtogroup ldr_communication_serv
 * @{
 */
/**
 * var char LCM_CurrentVersion[]
 * brief ASCII string holding the LCM version.
 * The LCM_CurrentVersion string is compared with
 * LCD_LCM_Compatibility[] in file LcdVersion.cpp in LCD code.
 */

INLINE_DOC
echo "char LCM_CurrentVersion[] = \"$VERSION\";" >> $LCM_RC_PATH
cat >> $LCM_RC_PATH << INLINE_DOC

/** @} */

INLINE_DOC
}

#
#  Loader Communication Driver
#
function process_lcd()
{
   cat > $LCD_RC_PATH << INLINE_DOC
/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#include "lcdriver_error_codes.h"
#include "LcmInterface.h"
#include "Error.h"
#ifdef _WIN32
#include "WinApiWrappers.h"
#else
#include "LinuxApiWrappers.h"
#include <dlfcn.h>
#define GetProcAddress dlsym
#endif
/**
 * var char LCD_LCM_Compatibility[]
 * brief ASCII string holding the LCD version.
 * This string is compared with the version of the LCM.
 * Current LCM version is defined in file lcm_version.c in LCM code.
 */
INLINE_DOC
echo "char LCD_LCM_Compatibility[] = \"$VERSION\";" >> $LCD_RC_PATH
cat >> $LCD_RC_PATH << INLINE_DOC

INLINE_DOC
}

# ------------------------------- main script routine -------------------------
get_init_data
case $1 in

		--lcm)
			process_lcm;;
		--lcd)
			process_lcd;;

		-h|?|--help)
			get_help;;
		*)
			stream_channel=$error_channel
			show_err;;
esac


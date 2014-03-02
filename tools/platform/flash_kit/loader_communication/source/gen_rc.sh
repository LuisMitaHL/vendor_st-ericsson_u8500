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

#current date
DATE=""

#git revision info
VERSION=""

path=$2
LCD_RC_PATH=$path/outLCDriver.rc
LCM_RC_PATH=$path/outLCM.rc

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
The purpose of this script is generating resource .rc files
that will be used by the resource compiler in order to
produce compiled resource files .res
This res files will be used by the linker when it will try to embedd
the version info into the executables.

INLINE_DOC
}

function get_init_data()
{
   DATE=`date`
   VERSION=`git describe --tags --always --long`
}

#
#  Loader Communication module
#
function process_lcm()
{
   cat > $LCM_RC_PATH << INLINE_DOC
1 VERSIONINFO
 FILEVERSION 1,0,0,1
 PRODUCTVERSION 1,0,0,1
 FILEFLAGSMASK 0x1fL
#ifdef _DEBUG
 FILEFLAGS 0x9L
#else
 FILEFLAGS 0x8L
#endif
 FILEOS 0x4L
 FILETYPE 0x2L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040904b0"
        BEGIN
INLINE_DOC
echo "            VALUE \"Comments\", \"Build date: $DATE\"" >> $LCM_RC_PATH
cat >> $LCM_RC_PATH << INLINE_DOC
            VALUE "CompanyName", "STEricsson AB"
            VALUE "FileDescription", "LCM Dynamic Link Library"
            VALUE "FileVersion", "1, 0, 0, 1"
            VALUE "InternalName", "Loader Communication Module"
            VALUE "LegalCopyright", "Copyright (C) STEricsson AB 2012"
            VALUE "ProductName", "CXA1104507 Loader Communication Module"
INLINE_DOC
echo "            VALUE \"ProductVersion\", \"$VERSION\"" >> $LCM_RC_PATH
cat >> $LCM_RC_PATH << INLINE_DOC
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x9, 1200
    END
END

INLINE_DOC

}

#
#  Loader Communication Driver
#
function process_lcd()
{
   cat > $LCD_RC_PATH << INLINE_DOC
1 VERSIONINFO
 FILEVERSION 1,0,0,1
 PRODUCTVERSION 1,0,0,1
 FILEFLAGSMASK 0x1fL
#ifdef _DEBUG
 FILEFLAGS 0x9L
#else
 FILEFLAGS 0x8L
#endif
 FILEOS 0x4L
 FILETYPE 0x2L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "000904b0"
        BEGIN
INLINE_DOC
echo "            VALUE \"Comments\", \"Build date: $DATE\"" >> $LCD_RC_PATH
cat >> $LCD_RC_PATH << INLINE_DOC
            VALUE "CompanyName", "STEricsson AB"
            VALUE "FileDescription", "LCDriver Dynamic Link Library"
            VALUE "FileVersion", "1, 0, 0, 1"
            VALUE "InternalName", "Loader Communication Driver"
            VALUE "LegalCopyright", "Copyright (C) STEricsson AB 2012"
            VALUE "ProductName", "CXC 173 0865,  LCDriver DLL"
INLINE_DOC
echo "            VALUE \"ProductVersion\", \"$VERSION\"" >> $LCD_RC_PATH
cat >> $LCD_RC_PATH << INLINE_DOC
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x9, 1200
    END
END

INLINE_DOC
}

# ------------------------------- main script routine -------------------------
get_init_data
case $1 in

		--lcm)
			echo "Generating LCM.rc file .."
			process_lcm;;
		--lcd)
			echo "Generating LCDriver.rc file."
			process_lcd;;

		-h|?|--help)
			get_help;;
		*)
			stream_channel=$error_channel
			show_err;;
esac


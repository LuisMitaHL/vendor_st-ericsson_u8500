#!/bin/bash
#-------------------------------------------------------------------------------
#                        (C) STEricsson
#    Reproduction and Communication of this document is strictly prohibited
#          unless specifically authorized in writing by STMicroelectronics.
#-------------------------------------------------------------------------------

FILE_CONF=""
DEBUG_VAR=0
FOLDER_LIST=""
HATS_FOLDER=$PWD
ROOTPATH=""
LOGFILE=$HATS_FOLDER/"log.txt"
RETURN_BACKFUNC=""

################################################################################
## Exit Function with error code
quit_func()
{
	if [ $# -eq 2 ] && [ -n $1 ]; then
		echo "Error $1 : Code $2"
		exit 0
	else
		echo "Error $0 : Code 99"
		exit -1
	fi
}

################################################################################
## Debug function
debug()
{
	if [ $DEBUG_VAR -eq 2 ]; then
		echo -E $@
		echo -E $@ >> $LOGFILE
	elif [ $DEBUG_VAR -eq 1 ]; then
		echo -E $@ >> $LOGFILE
	fi
}

################################################################################
## Help function
usage()
{
	echo "#################################################################"
	echo "##"
	echo "##	Clean_hats.sh script"
	echo "##"
	echo "#################################################################"
	echo "##"
	echo "##"
	echo "## Remove folders from HATS based on Kconfig options"
	echo "## "
	echo "## Clean_hats.sh -c <configs/*_defconfig> [-d<1|2>]"
	echo "## [-o <new_output_folder>] [-f <custom_file_list>]"
	echo "##"
	echo "##"
	echo "## Mandatory option(s):"
	echo "## -c : specify configuration file to use."
	echo "##"
	echo "## Optionnal:"
	echo "## -d<debug_level> : enable debug mode."
	echo "##		1->Display debug message"
	echo "##		2->dispaly and	store debug message"
	echo "##"
	echo "## -f : Text file which contains a list of file to remove"
	echo "##"
	echo "## -o : Output folder of the HATS clean."
	echo "##"
	echo "## -r : Remove git tree."
	echo "##"
	echo "#################################################################"
	exit 0
}

################################################################################
## Creation function to parse the folder which contains Kconfig
make_list()
{
	debug "> $FUNCNAME"
	
	list_temp=`find -L . -depth`
	if [ -n @list_temp[0] ]; then
		for i in $list_temp; do
			if [ -e $i/Kconfig ]; then
				debug "found $i/Kconfig"
				FOLDER_LIST+=$i/Kconfig
				FOLDER_LIST+=" "
			fi
		done

	else
		quit_func $FUNCNAME 1
	fi

	debug "< $FUNCNAME"
}

################################################################################
## Create a temporary directory to make clean into
copy_temp_directory()
{
	debug "> $FUNCNAME"
	if [ ! -d $1 ]; then
		debug "mkdir -p $1"
		mkdir -p $1
		debug "cp -RPf $HATS_FOLDER/../* $1"
		cp -RPf $HATS_FOLDER/../* $1
		cd $1
		ROOTPATH=$PWD/hats
	else
		quit_func $FUNCNAME 1
	fi
	debug "< $FUNCNAME"
}

################################################################################
## Remove empty folder after cleaning
remove_empty()
{
	debug "> $FUNCNAME"
	local list_empty=`find ../ -type d -empty`
	if [ -n @list_empty[0] ]; then
		for i in $list_empty; do
			if [ -d $i ]; then
				debug "rm -Rf $i"
				rm -Rf $i
			fi
		done
	fi
	debug "< $FUNCNAME"
}

################################################################################
## Remove files regarding Customer configuration
remove_cpo_list()
{
	debug "> $FUNCNAME"
	if [ -f $1 ]; then
		for dirtorm in $(cat $1); do
			debug "$dirtorm"
			if [ -e $ROOTPATH/$dirtorm ]; then
				debug "rm -Rf $ROOTPATH/$dirtorm"
				rm -Rf $ROOTPATH/$dirtorm
			fi
		done
	fi
	debug "< $FUNCNAME"
}

################################################################################
## Remove git tree
remove_git_tree()
{
	debug "> $FUNCNAME"
	for gitdir in $(find $ROOTPATH/../ -type d -name .git); do
		debug "rm -Rf $gitdir"
		rm -Rf $gitdir
	done
	for gitign in $(find $ROOTPATH/../ -name .gitignore); do
		debug "rm -f $gitign"
		rm -Rf $gitign
	done
	debug "< $FUNCNAME"
}

################################################################################
## Replace defconfig link by the configuration used and remove other
## Configuration files
replace_defconfig()
{
	debug "> $FUNCNAME"
	for fic in $(find configs -type f); do
		if [ $FILE_CONF = $fic ]; then
			link=`basename $fic`
			debug "ln -sf $link configs/defconfig"
			ln -sf $link configs/defconfig
		else
			debug "rm -f $fic"
			rm -f $fic
		fi
	done
	debug "< $FUNCNAME"
}

################################################################################
## Remove Backslash in folder name
remove_backslash()
{
	debug "> $FUNCNAME"
	if [ $# -eq 1 ] && [ -n $1 ]; then
		RETURN_BACKFUNC=`echo $1 | sed 's/.\///'`
	else
		quit_func $FUNCNAME 1
	fi
	debug "< $FUNCNAME"
}

################################################################################
## Get Kconfig file and line where arg is present
## Return file and line
get_file_line()
{
	debug "> $FUNCNAME"
	if [ $# -eq 1 ] && [ -n $1 ]; then
		remove_backslash $1
		local kconf_rename=$RETURN_BACKFUNC
		local researchfile=`find -L . -name "Kconfig" | xargs grep $kconf_rename`
		if [ -n "$researchfile" ]; then
			file=`echo $researchfile | cut -d ':' -f1`
			line=`echo $researchfile | cut -d ':' -f2`
			if [ -z $file ] && [ -z $line ]; then
				quit_func $FUNCNAME 2
			fi
		else
			quit_func $FUNCNAME 1
		fi
	fi
	debug "< $FUNCNAME"
}

################################################################################
## Remove the line from Kconfig file
remove_kconfig_line()
{
	debug "> $FUNCNAME"
	local kconfig_file=$1
	debug "$1 $line"
	if [ -n "$line" ] && [ -n "$kconfig_file" ]; then
		local grepline=`grep -n "$line" $kconfig_file | cut -d ":" -f1`
		debug "sed -i "${grepline}d" "$kconfig_file""
		sed -i "${grepline}d" "$kconfig_file"
	else
		quit_func $FUNCNAME 1
	fi
	debug "< $FUNCNAME"
}

################################################################################
## Remove foder
suppress_folder()
{
	debug "> $FUNCNAME"
	local direct="$1"
	if [ -d "$direct" ]; then
		if [  -d $direct/.git ]; then
			debug "git directory"
			debug "rm -rf "$direct/*""
			rm -f $direct/*
		else
			debug "rm -rf "$direct""
			rm -rf "$direct"
		fi
	else
		echo "Dir $direct not found."
#		quit_func $FUNCNAME 1
	fi
	debug "< $FUNCNAME"
}

################################################################################
## Remove link and folder associated
suppress_link()
{
	debug "> $FUNCNAME"
	if [ $# -eq 1 ] && [ -n "$1" ]; then
		local dir_link="$1"
		remove_backslash "$dir_link"
		local rename_dir=$RETURN_BACKFUNC
		debug "rm -f $ROOTPATH/$rename_dir"
		rm -f "$ROOTPATH/$rename_dir"
		cd ../
		debug "find . -type d -name $rename_dir | cut -d '/' -f 2-"
		local result=`find . -type d -name $rename_dir | cut -d '/' -f 2-`
		for resdir in $result; do
			if [ -e $resdir/Kconfig ]; then
				debug "suppress_folder "$ROOTPATH/../$resdir""
				suppress_folder "$ROOTPATH/../$resdir"
			fi
		done
		cd $ROOTPATH
	else
		quit_func $FUNCNAME 1
	fi
	debug "< $FUNCNAME"
}

################################################################################
## Main Script beginning
while getopts c:d:f:ho:r opt
do
	case $opt in
	c) echo "Config file=$OPTARG"
		FILE_CONF="$OPTARG"
		;;
	d) echo "Debug activated"
		DEBUG_VAR=$OPTARG
		;;
	f) echo "File list=$OPTARG"
		CPO_FILE_LIST="$OPTARG"
		;;
	h) usage
		;;
	o) echo "Output folder=$OPTARG"
		OUT_FOLDER="$OPTARG"
		;;
	r) echo "Git tree will be removed"
		REMOVE_GIT_TREE=1
		;;
	\?) usage		
		;;
	esac
done

echo "Start HATS clean up"
debug "FILE_CONF= $FILE_CONF"
if [ -f "$FILE_CONF" ]; then
rm -f $LOGFILE
debug "CLEANUP"
make distclean >/dev/null 2>&1
make defconfig
if [ -n "$OUT_FOLDER" ]; then
copy_temp_directory $OUT_FOLDER
else
	ROOTPATH=$PWD
fi
cd $ROOTPATH
debug "ROOTPATH=$ROOTPATH"
make_list
for kconf in $FOLDER_LIST; do
	debug "START ----------- : $kconf"
	dir=`dirname $kconf`
	filter=`grep $dir $ROOTPATH/$FILE_CONF`
	if [ -z "$filter" ]; then
		debug "REMOVE ----------- : $kconf"
		get_file_line $kconf
		remove_kconfig_line $file
		if [ -h $dir ]; then
			debug "suppress_link "$dir""
			suppress_link "$dir"
		else
			remove_backslash $dir			 
			partial_name=`basename $RETURN_BACKFUNC`
			debug "partial name=$partial_name"
			result=`find -L . -maxdepth 3 -name $partial_name | cut -d '/' -f 2-`
			debug "result=$result"
			debug "suppress_folder "$ROOTPATH/$result""
			suppress_folder "$ROOTPATH/$result"
		fi
	fi
done
debug "replace_defconfig"
replace_defconfig
if [ -n "$CPO_FILE_LIST" ]; then
	debug "remove_cpo_list $CPO_FILE_LIST"
	remove_cpo_list $CPO_FILE_LIST
	rm -Rf $ROOTPATH/$CPO_FILE_LIST
fi
if [ a$REMOVE_GIT_TREE == a1 ];then
	remove_git_tree
fi
debug "remove_empty"
remove_empty
#Clean up
rm -f $ROOTPATH/$0
cd $HATS_FOLDER
else
	usage
fi


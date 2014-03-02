#!/usr/bin/env bash
# first parameter: base path where to create hw folders
# second parameter: relative path from base path to bootimages

function printusage()
(
	echo "Usage: mkhwconf.sh [OPTION]... "
	echo "Generate hw specific flashkit filelists from a config.list file."
	echo
	echo  "   -d base folder to create the hardware specific folders in"
	echo  "   -b prefix to add in the filelist for bootcomponents"
	echo  "   -l prefix to add in the filelist for loaders"
	echo  "   -m prefix to add in the filelist for modemfiles"
	echo  "   -n comment out the files in the filelist for modemfiles"
)

if [ $# -lt 2 ]; then
	printusage
	exit 1
fi

thisdir=`dirname "$0"`
configlistpath=`readlink -f "$thisdir"`/config.list

basepath=""
bootprefix=../../boot
loaderprefix=../../loaders
modemprefix=../../modem_images

while [ "$1" != "" ]; do
	case $1 in
		-d)
			shift
			basepath=$1
			;;
		-b)
			shift
			bootprefix=$1
			;;
		-l)
			shift
			loaderprefix=$1
			;;
		-m)
			shift
			modemprefix=$1
			;;
		-n)
			nomodem=yes
			;;
		*)
			printusage
			exit 1
			;;
	esac
	shift
done

echo "Generating hw boot configs"
echo -n "from: "
pwd

if [ ! -d $basepath ]; then
	echo "base folder does not exists ($basepath)"
	exit 1
fi

echo Creating profiles in $basepath
echo bootprefix: $bootprefix
echo loaderprefix: $loaderprefix
echo modemprefix: $modemprefix

#Current Working Hardware
chw=""
curpath=""
filelistbootpath=""
filelistloaderpath=""
filelistmodempath=""
markfilepath=""
filelistsignpackage=""

while read line
do
	line=`echo "$line"| tr -d '\\n\\r'| sed 's/^ *//g'| sed 's/ *$//g'`

	if [ `echo -n "$line" | wc -m` -eq 0 ]; then
		continue
	fi

	echo $line | grep --quiet -e "\([:space:]*\[[a-zA-Z0-9_]\+\][:space:]*\)\|\([[:space:]a-zA-Z0-9_]\+=[[:space:]a-zA-Z0-9_]\+\)"
	if [ 1 -eq $? ]; then
		echo "Invalid line in bootfiles config list ($line)"
		exit 1
	fi

	name=`echo "$line"| cut -d'=' -f1| sed 's/^ *//g'| sed 's/ *$//g'`
	value=`echo "$line"| cut -d'=' -f2| sed 's/^ *//g'| sed 's/ *$//g'`

	newhwfound=0
	echo $name|grep --quiet '^\[[a-zA-Z0-9_]\+\]$' && newhwfound=1

	if [ $newhwfound -eq 1 ]; then
		chw=`echo $value | sed 's/^\[\([a-zA-Z_0-9]\+\)\]$/\1/g'`
		curpath=$basepath/$chw
		if [ ! -d $curpath ]; then
			mkdir $curpath
		fi
		filelistbootpath=$curpath/_filelist_bootfiles.txt
		filelistbippath=$curpath/_filelist_bipfiles.txt
		filelistxloaderpath=$curpath/_filelist_x-loader.txt
		filelistloaderpath=$curpath/_filelist_loader.txt
		filelistmodempath=$curpath/_filelist_modem.txt
		markfilepath=$curpath
		filelistsignpackage=$curpath/_signpackages.txt
		rm -f $filelistbootpath
		touch $filelistbootpath
		rm -f $filelistbippath
		touch $filelistbippath
		rm -f $filelistxloaderpath
		touch $filelistxloaderpath
		rm -f $filelistloaderpath
		touch $filelistloaderpath
		rm -f $filelistmodempath
		touch $filelistmodempath
		rm -f $markfilepath
		touch $markfilepath
		rm -f $filelistsignpackage
		touch $filelistsignpackage
	elif [ -n "$filelistbootpath" ]; then
		case "$name" in
			"loader")
				echo $name=$loaderprefix/$value >> $filelistloaderpath
				;;
			"x-loader")
				echo $bootprefix/$value >> $filelistxloaderpath
				echo $name=$bootprefix/$value.ssw >> $filelistbippath
				;;
			"issw")
				echo $name=$bootprefix/$value >> $filelistbippath
				;;
			"loader-bip")
				echo $name=$bootprefix/$value >> $filelistloaderpath
				;;
			"loader-prcmu")
				echo $name=$bootprefix/$value >> $filelistloaderpath
				;;
			"loader-meminit")
				echo $name=$bootprefix/$value >> $filelistloaderpath
				;;
			"mark-hw")
				echo Do not remove this file >> $markfilepath/$value
				;;
			"ipl"|"modem")
				nonex=`echo $value | grep nonexistant`
				if [[ ! -z "$nomodem" || ! -z $nonex ]]; then
					echo -n "#" >> $filelistmodempath
				fi
				echo $name=$modemprefix/$value >> $filelistmodempath
				;;
			"boot_image_user")
				echo boot_image=$value >> $filelistbootpath
				;;
			"boot_image"|"mem_init"|"power_management")
				echo $name=$bootprefix/$value >> $filelistbootpath
				;;
			"rootsignpackage"|"authsignpackage")
				echo $name $value >> $filelistsignpackage
				;;
			*)
				echo "Invalid entry found"
				;;
		esac
	else
		echo "Invalid line in bootfiles config list ($line)"
	fi

done <$configlistpath

echo "finished"


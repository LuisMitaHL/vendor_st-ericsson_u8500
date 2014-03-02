#!/usr/bin/env bash

function createflashlayoutlist()
(
	if [ -z "$1" ]; then
		echo "no path to where to store filelist supplied"
		exit 1
	fi

	if [ -z "$2" ]; then
		echo "Path to flashlayout not specified"
		exit 1
	else
		flashlayoutpath="$2"
	fi

	filelistpath=$1/_filelist_flashlayout.txt
	rm -f $filelistpath
	touch $filelistpath

	echo -n "flashlayout = " >> $filelistpath
	echo $flashlayoutpath >>$filelistpath
)

function createitplist()
(
	if [ -z "$1" ]; then
		echo "no path to where to store filelist supplied"
		exit 1
	fi

	if [ -z "$2" ]; then
		echo "Path to itp not specified"
		exit 1
	else
		itppath="$2"
	fi

	filelistpath=$1/_filelist_itp.txt
	rm -f $filelistpath
	touch $filelistpath

	echo -n "itp.bin = " >> $filelistpath
	echo $itppath >>$filelistpath
)

function createdntcertfilelist()
(
	if [ -z "$1" ]; then
		echo "no path to where to store filelist supplied"
		exit 1
	fi

	if [ -z "$2" ]; then
		echo "Path to dntcert not specified"
		exit 1
	else
		dntcertpath="$2"
	fi

	filelistpath=$1/_filelist_dntcert.txt
	rm -f $filelistpath
	touch $filelistpath

	echo -n "dntcert = " >> $filelistpath
	echo $dntcertpath >>$filelistpath
)

function createlinuxfilelist()
(
	if [ -z "$1" ]; then
		echo "no path to where to store filelist supplied"
		exit 1
	fi

	if [ -z "$2" ]; then
		echo "Path to systemimage not specified"
		exit 1
	else
		systemimgpath="$2"
	fi

	if [ -z "$3" ]; then
		echo "Path to userdata not specified"
		exit 1
	else
		userdataimgpath="$3"
	fi

	if [ -z "$4" ]; then
		echo "Path to cacheimage not specified"
		exit 1
	else
		cacheimgpath="$4"
	fi

	if [ -z "$5" ]; then
		echo "Path to modemfs not specified"
		exit 1
	else
		modemfsimgpath="$5"
	fi

	if [ -z "$6" ]; then
		echo "Path to miscimage not specified"
		exit 1
	else
		miscimgpath="$6"
	fi

	filelistpath=$1/_filelist_linux.txt
	rm -f $filelistpath
	touch $filelistpath

	echo -n "system.img = " >> $filelistpath
	echo $systemimgpath >>$filelistpath

	echo -n "userdata.img = " >> $filelistpath
	echo $userdataimgpath >>$filelistpath

	echo -n "cache.img = " >> $filelistpath
	echo $cacheimgpath >>$filelistpath

	echo -n "misc.img = " >> $filelistpath
	echo $miscimgpath >>$filelistpath

	if $includemodem ; then
		echo -n "modemfs.img = " >> $filelistpath
		echo $modemfsimgpath >>$filelistpath
	fi
)

function createubootfilelist()
(
	if [ -z "$1" ]; then
		echo "no path to where to store filelist supplied"
		exit 1
	fi

	if [ -z "$2" ]; then
		echo "Path to uboot not specified"
		exit 1
	else
		ubootpath="$2"
	fi

	if [ -z "$3" ]; then
		echo "Path to uboot-env not specified"
		exit 1
	else
		ubootenvpath="$3"
	fi

	if [ -z "$4" ]; then
		echo "Path to splash screen not specified"
		exit 1
	else
		splashpath="$4"
	fi

	filelistpath=$1/_filelist_uboot.txt
	rm -f $filelistpath
	touch $filelistpath

	echo -n "u-boot.bin = " >> $filelistpath
	echo $ubootpath >>$filelistpath

	echo -n "u-boot-env.bin = " >> $filelistpath
	echo $ubootenvpath >>$filelistpath

	echo -n "splash = " >> $filelistpath
	echo $splashpath >>$filelistpath
)

function createcspsalist()
(
	if [ -z "$1" ]; then
		echo "no path to where to store filelist supplied"
		exit 1
	fi

	if [ -z "$2" ]; then
		echo "Path to cspsa bin not specified"
		exit 1
	else
		cspsapath="$2"
	fi

	filelistpath=$1/_filelist_cspsa.txt
	rm -f $filelistpath
	touch $filelistpath

	echo -n "cspsa.bin = " >> $filelistpath
	echo $cspsapath >>$filelistpath
)

function createkernellist()
(
	if [ -z "$1" ]; then
		echo "no path to where to store filelist supplied"
		exit 1
	fi

	if [ -z "$2" ]; then
		echo "Path to boot.img not specified"
		exit 1
	else
		bootimgpath="$2"
	fi

	if [ -z "$3" ]; then
		echo "Path to recovery.img not specified"
 		exit 1
 	else
		recoveryimgpath="$3"
 	fi

	filelistpath=$1/_filelist_kernel.txt
	rm -f $filelistpath
	touch $filelistpath

	echo -n "boot.img = " >> $filelistpath
	echo $bootimgpath >>$filelistpath

	echo -n "recovery.img = " >> $filelistpath
	echo $recoveryimgpath >>$filelistpath

)


echo "Creating filelist to kernel, u-boot and flashlayout"

if [ $# -lt 2 ]; then
	echo "incorrect input"
	exit 1
fi

to_parent=""
from=""

bootimgpath=../../boot.img
recoveryimgpath=../../recovery.img
systemimgpath=../../system.img
userdataimgpath=../../userdata.img
cacheimgpath=../../cache.img
miscimgpath=../../misc.img
modemfsimgpath=../../modemfs.img
includemodem=true

ubootpath=../../u-boot.bin
ubootenvpath=../../u-boot-env.bin
splashpath=../../splash.bin

flashlayoutpath=../../flashlayout.txt

itppath=../../itp.bin
dntcertpath=../../dntcert.bin

cspsapath=../../u8500_default_cspsa.bin


while [ "$1" != "" ]; do
	case $1 in
		-d)
			shift
			to_parent=$1
			;;
		-l)
			shift
			bootimgpath=$1
			;;
		-s)
			shift
			systemimgpath=$1
			;;
		-u)
			shift
			userdataimgpath=$1
			;;
		-c)
			shift
			cacheimgpath=$1
			;;
		-g)
			shift
			miscimgpath=$1
			;;
		-j)
			shift
			recoveryimgpath=$1
			;;
		-m)
			shift
			modemfsimgpath=$1
			;;
		-n)
			shift
			includemodem=$1
			;;
		-b)
			shift
			ubootpath=$1
			;;
		-e)
			shift
			ubootenvpath=$1
			;;
		-f)
			shift
			flashlayoutpath=$1
			;;
		-p)
			shift
			splashpath=$1
			;;
		-i)
			shift
			itppath=$1
			;;
		-t)
			shift
			dntcertpath=$1
			;;
		-a)
			shift
			cspsapath=$1
			;;
		*)
			echo "$0 Unknown parameter"
			;;
	esac
	shift
done

if [ ! -d $to_parent ]; then
	echo "destination folder does not exists ($to_parent)"
	exit 1
fi

	createitplist $to_parent/$to/ $itppath
	createflashlayoutlist $to_parent/$to/ $flashlayoutpath
	createubootfilelist $to_parent/$to/ $ubootpath $ubootenvpath $splashpath
	createlinuxfilelist $to_parent/$to/ $systemimgpath $userdataimgpath $cacheimgpath $modemfsimgpath $miscimgpath
	createdntcertfilelist $to_parent/$to/ $dntcertpath
	createcspsalist $to_parent/$to/ $cspsapath
	createkernellist $to_parent/$to/ $bootimgpath $recoveryimgpath
echo "finished"

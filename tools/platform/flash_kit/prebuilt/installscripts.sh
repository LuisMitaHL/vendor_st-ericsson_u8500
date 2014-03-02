#!/usr/bin/env bash

function createxlauncherlist()
(
	xlauncherpath="../../ramload_images/normal.bin"

	if [ -z "$1" ]; then
		echo "no path to where to store filelist supplied"
		exit 1
	fi

	if [ "$2" != "" ]; then
		xlauncherpath="$2"
	fi

	filelistpath=$1/_filelist_xlauncher.txt
	rm -f $filelistpath
	touch $filelistpath

	echo -n "normal = " >> $filelistpath
	echo $xlauncherpath >>$filelistpath
)

echo "Copying flash kit prebuilt files"

if [ $# -lt 4 ]; then
	echo "incorrect input"
	exit 1
fi

to_parent=""
from=""
cspsabinpath=""
xlauncherbinpath=""

while [ "$1" != "" ]; do
	case $1 in
		-s)
			shift
			from=$1
			;;
		-d)
			shift
			to_parent=$1
			;;
		-x)
			shift
			xlauncherbinpath=$1
			;;
		*)
			echo "$0: Unknown parameter"
			;;
	esac
	shift
done

if [ ! -d $to_parent ]; then
	echo "destination folder does not exists ($to_parent)"
	exit 1
fi

if [ ! -d $from ]; then
	echo "source folder does not exists ($from)"
	exit 1
fi

for to in `ls $to_parent`
do
	install -m 755 -t $to_parent/$to/ $from/*.sh
	install -m 644 -t $to_parent/$to/ $from/*.bat
	createxlauncherlist $to_parent/$to/ $xlauncherbinpath
done

echo "finished"
exit 0

#!/usr/bin/env bash

usage()
{
	echo Usage: $0 listfile destdir basedir
	exit 1
}

if [ -z "$1" ]; then
	usage
fi

if [ -z "$2" ]; then
        usage
fi

if [ -z "$3" ]; then
        usage
fi

destdir=$2/
sourcelist=$1
inbasedir=$3/
tempfile=`mktemp`

sort $sourcelist >$tempfile
uniq $tempfile >$sourcelist

rm $tempfile

echo copy files listed in $sourcelist to $destdir
IFS=$'\n'

cat $sourcelist|while read filename; do
	if [ -z "$filename" ]; then
		continue
	fi

	finaldir=$destdir`echo $filename | awk -v basedir=$inbasedir '{ sub(basedir,"",$$0);if(match($$0, /(.+\/).+/, arr)) { print arr[1] } }'`
	mkdir -p $finaldir

	cp -f $filename $finaldir
done



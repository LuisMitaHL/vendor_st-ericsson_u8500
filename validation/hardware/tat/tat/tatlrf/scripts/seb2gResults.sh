#! /bin/sh
#	Copyright (C) ST-Ericsson 2011
#	Prints HATS RF Single BER 2G results for one or all pages.
#	This script is member of HATS unitary test tools.
#	Author:	alain.crouzet-nonst@stericsson.com

usage()
{
	echo "$0 [page]"
	echo "page: a number within 1-4. If the argument is missing then all pages"
	echo "are printed."
}

page_result()
{
	slotn=`echo "$1" | awk -F/ '{ print $NF }'`
	echo "-- $slotn --"

	dth_get_children "$1/Result" "name"

	echo "-- END $slotn --"
}


. ./dthadv.sh

# $1 is the page number.
page=$1

if [ $# -le 1 ]
then

	# $1 is the page number. if not defined, prints all pages.

	if [ $# -eq 1 ]
	then
		# unitary
		page=$1
		page_result $DTH9P/RF/BER/2G/SLOT$page
	else
		# global
		for slot in `ls $DTH9P/RF/BER/2G/SLOT* -d`
		do
			page_result $slot
		done
	fi
else
	usage
	exit 1
fi


#!/bin/bash

outdir=$ANDROID_PRODUCT_OUT
configlist=$outdir/config.list
modemprefix=$outdir/modem_images
modem=""
signpackage=""

#parses config list to find the hw specific components
#fills the variables "modem" with the correct files
#for this hw which is in config.list file
function parse_variable()
{
	local found=0
	local basepath=`dirname "$configlist"`

	while read line
	do
		# Look for HW passed to function
		local hw=`echo $line | grep -e "\[$1\]"`
		if [ $hw ]; then
			#echo $hw : configuration according to $configlist
			# Found it, read next line
			found=1
			continue
		fi

		if [ $found -eq 1 ]; then
			# loader and bip must be recreated after this
			loader_created=0
			biphandled=0
			loader_biphandled=0

			# Check for new record. Exit if found
			local finish=`echo $line | grep -e [[]`
			if [ $finish ]; then
				break
			fi

			local name=`echo "$line"| cut -d'=' -f1| sed 's/^ *//g'| sed 's/ *$//g'`
			local value=`echo "$line"| cut -d'=' -f2| sed 's/^ *//g'| sed 's/ *$//g'`

			pushd "$basepath" >>/dev/null
			case "$name" in
			"modem")
				if [ "$value" != "nonexistant" ]; then
					modem=$value
					if [ ! -f "$modem" ] ; then
						modem=`echo $modemprefix/$value`
					fi
				fi
				;;
			"rootsignpackage")
				signpackage=$value
				;;
			*)
				;;
			esac

			popd >>/dev/null
		fi
	done <"$configlist"

	if [ $found -eq 0 ]; then
		echo "Hardware $1 is not supported" >&2
		exit 1
	fi
}

if [ ! -f "$configlist" ]; then
	echo "Cannot find configlist: $configlist" >&2
	exit 1
fi

if [ "$1" == "UNKNOWN" ]; then
	echo "No hardware specified" >&2
	exit 1
fi

parse_variable $1
echo "MODEMIMG=$modem"
echo "SIGNPACKAGE=$signpackage"


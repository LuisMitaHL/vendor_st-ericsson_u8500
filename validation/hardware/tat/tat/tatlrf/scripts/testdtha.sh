#! /bin/sh
#	Copyright (C) ST-Ericsson 2011
#	This script executes DTH access regression tests by performing
#	predefined operations and bound checking for the specified element and
#	its children.
#	Author:	alain.crouzet-nonst@stericsson.com

local ROOTDIR=""
if [ $# -lt 1 ]; then
	echo "testdtha.sh <dth_dir>"
	exit 1
fi

# make use of dthadv.sh
. ./dthadv.sh
# make use of teststat.sh
. ./teststat.sh

format_dth_value()
{
	VAR_FORMAT_DTH_VALUE=`(echo - | awk -v K=$1 '{ printf("%.5f\n",K)}')`
}

# write an array and test it has been successfully written.
# if the write failed then a message is echoed.
# the array is NOT restored after the call.
attempt_dth_write_array()
{
	local DIRNAME=$1
	local STRARRAYWRI=$2

#	echo "dth_set $DIRNAME $STRARRAYWRI"
	dth_set $DIRNAME "$STRARRAYWRI"

	# read the updated value, which must be the new one else the test
	# has failed
	dth_get $DIRNAME
	local STRARRAYUPD=$VAR_GET_RESULT
	local ARRAYUPD=${STRARRAYUPD//;/ }
#	echo "Array updated: $ARRAYUPD"

	# must compare normalized value, not the value strings because the
	# number of decimals may defer!
	local ARRAYWRI=${STRARRAYWRI//;/ }
#	echo "Written array: $ARRAYWRI"

	local INDEX=1
	for TMP_ATTEMPT_DTH_WRITE_ARRAY_SUBVAL in $ARRAYWRI
	do
		format_dth_value $TMP_ATTEMPT_DTH_WRITE_ARRAY_SUBVAL
		local VALSRC=$VAR_FORMAT_DTH_VALUE

		format_dth_value `echo "$ARRAYUPD"|cut -d ' ' -f $INDEX`
		local VALUPD=$VAR_FORMAT_DTH_VALUE

		if [ "$VALSRC" != "$VALUPD" ]; then
			test_FAIL "DTH write $STRARRAYWRI to $DIRNAME." "$3Read: $STRARRAYUPD"
			return 1
		fi
		INDEX=`expr $INDEX + 1`
	done

	return 0
}

# write a scalar value and test it has been successfully written.
# if the write failed then a message is echoed
# the scalar is NOT restored after the call.
attempt_dth_write()
{
	local DIRNAME=$1
	local VALWRI=$2

#	echo "dth_set $DIRNAME $VALWRI"
	dth_set $DIRNAME "$VALWRI"

	# read the updated value, which must be the new one else the test
	# has failed
	dth_get $DIRNAME

	format_dth_value $VAR_GET_RESULT
	local VALUPD=$VAR_FORMAT_DTH_VALUE

	format_dth_value $VALWRI
	local VALWRIF=$VAR_FORMAT_DTH_VALUE

	if [ $VALWRIF != $VALUPD ]; then
		test_FAIL "DTH write $VALWRI to $DIRNAME." "$3Read: $VALUPD"
	fi
}

dth_nearest_value()
{
	local DIRNAME=$1
	VAR_DTH_NEAREST_VAL=`(echo - | awk  -v K=$2 '{ print K+1}')`

	dth_range $DIRNAME
	local RESRANGE=$?

	if [ 0 = "$RESRANGE" -a 0 != `expr $VAR_DTH_RANGE_MAX \< $VAR_DTH_NEAREST_VAL` ]; then
		VAR_DTH_NEAREST_VAL=`(echo - | awk  -v K=$2 '{ print K-1}')`
	fi
	# TODO: what if range is ]-1,1[ or lesser ?
}

ROOTDIR=$DTH9P/$1

# for each DTH parameter do...
for file in `find $ROOTDIR -name value`
do
	if [ -r "$file" ]; then

		local DIRNAME=`dirname $file`

		# read the current value
#		dth_gete "$DIRNAME" "fpath"
		dth_get "$DIRNAME"
		local VALINI=$VAR_GET_RESULT

		# increment the value by one.
		# in case of a array, perform the operation both globaly and unitary
		# and checks the value written by performing a global/unitary read.
		if [ "" != "`echo "$VALINI"|grep ";"`" ]; then
			# array
			local VALWRI=""
			local VALMIN=""
			local VALMAX=""

			local ARRVAL=$(echo $VALINI | tr ";" "\n")
			for TMP_TESTDTHA_SH_SUBVAL in $ARRVAL
			do
				dth_nearest_value $DIRNAME $TMP_TESTDTHA_SH_SUBVAL
				if [ -n "$VALWRI" ]; then
					VALWRI="$VALWRI;$VAR_DTH_NEAREST_VAL"
					VALMIN="$VALMIN;$VAR_DTH_RANGE_MIN"
					VALMAX="$VALMAX;$VAR_DTH_RANGE_MAX"
				else
					VALWRI=$VAR_DTH_NEAREST_VAL
					VALMIN=$VAR_DTH_RANGE_MIN
					VALMAX=$VAR_DTH_RANGE_MAX
				fi
			done

			attempt_dth_write_array $DIRNAME "$VALWRI"

			# write to limits
			attempt_dth_write_array $DIRNAME "$VALMIN" "Write minimum. "
			attempt_dth_write_array $DIRNAME "$VALMAX" "Write maximum. "

			# restore the value before test
			dth_set $DIRNAME "$VALINI"
		else
			# scalar

			# write the current value incremented by 1 or decremented by 1 if
			# max.
			dth_nearest_value $DIRNAME $VALINI
			local VALWRI=$VAR_DTH_NEAREST_VAL

			attempt_dth_write $DIRNAME "$VALWRI"

			# write to limits
			attempt_dth_write $DIRNAME "$VAR_DTH_RANGE_MIN" "Write minimum. "
			attempt_dth_write $DIRNAME "$VAR_DTH_RANGE_MAX" "Write maximum. "

			# restore the value before test
			dth_set $DIRNAME "$VALINI"
		fi
	fi
done

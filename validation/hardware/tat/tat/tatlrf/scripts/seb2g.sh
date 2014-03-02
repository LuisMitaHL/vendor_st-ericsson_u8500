#! /bin/sh
#	Copyright (C) ST-Ericsson 2011
#	This file is HATS RF main script for Single BER 2G.
#	This script is member of HATS unitary test tools.
#	Author:	alain.crouzet-nonst@stericsson.com

usage()
{
	echo "$0 command"
	echo "command can be:"
	echo "   0 or store: store 2G BER list"
	echo "   1 or start: start BER"
	echo "   2 or result: get 2G results, but the value are not printed"
	echo "   3 or stop: stop BER"
	echo "   4 or reset: reset DUT list"
	# remaining codes are supported by a different script:
	# seb2gStatus.sh for 5 (status)
	echo "other BER operations: check the appropriate script"
}

is_number()
{
	local var=$1
	if [ "$(echo $var | grep "^[ [:digit:] ]*$")" ]
	then
		return 0
	fi

	return 1
}

if [ $# -eq 1 ]
then
	is_number $1
	if [ $? -eq "0" ]; then
		./dthadv.sh RF/BER/2G/Command/Action r $1
	else
		case $1 in
		store)
			./dthadv.sh RF/BER/2G/Command/Action r 0
			;;
		start)
			./dthadv.sh RF/BER/2G/Command/Action r 1
			;;
		result)
			./dthadv.sh RF/BER/2G/Command/Action r 2
			;;
		stop)
			./dthadv.sh RF/BER/2G/Command/Action r 3
			;;
		reset)
			./dthadv.sh RF/BER/2G/Command/Action r 4
			;;
		*)
			echo "invalid command"
			exit 1
		esac
	fi

	./dthadv.sh RF/BER/2G/Command/Action/Out_CmdStatus g
else
	usage
fi


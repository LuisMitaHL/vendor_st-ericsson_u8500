#!/bin/bash

T32_NODE=""
T32_CONNECTIONMODE=""
file_T32NODE=$SCRIPTPATH/T32NODE.txt

t32ip=$1
if [ "$t32ip" == '' ]; then
	echo "Please enter IP address or select number :"
	cat $file_T32NODE

	# read user input
	read t32ip
fi

# default
if [ "$t32ip" == '' ]; then
	t32ip=1
fi

if grep "^$t32ip:" $file_T32NODE; then
	T32_NODE=`grep "^$t32ip:" $file_T32NODE | cut -f 2 -d'>' | sed 's/ //g'`
else
	T32_NODE="$t32ip"
fi

t32mode=$2

# force connection mode if it is not provided by command line
# => option not useful in practice (to be remove ?)
if [ "$t32mode" == '' ]; then
	t32mode=1
fi
while [[ $t32mode -lt 1 ]] || [ $t32mode -gt 4 ]; do
	echo "Please select connection mode:"
	echo -e "\033[1m#   ConnectionMode\033[0m"
	echo -e "\033[1m1\033[0m:  <nothing>"
	echo -e "\033[1m2\033[0m:  AUTOABORT"
	echo -e "\033[1m3\033[0m:  AUTOCONNECT"
	echo -e "\033[1m4\033[0m:  AUTORETRY"

	# read user input
	read t32mode
	# default
	if [ "$t32mode" == '' ]; then
		t32mode=1
		echo "default selected ($t32mode)"
	fi
done

if [[ $t32mode = 1 ]] ; then
	T32_CONNECTIONMODE=""
elif [[ $t32mode = 2 ]] ; then
	T32_CONNECTIONMODE="CONNECTIONMODE=AUTOABORT"
elif [[ $t32mode = 3 ]] ; then
	T32_CONNECTIONMODE="CONNECTIONMODE=AUTOCONNECT"
else
	T32_CONNECTIONMODE="CONNECTIONMODE=AUTORETRY"
fi

echo "T32_NODE=$T32_NODE"
echo "$T32_CONNECTIONMODE"

export T32_NODE
export T32_CONNECTIONMODE

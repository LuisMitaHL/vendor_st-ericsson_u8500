#! /bin/sh

usage()
{
	echo "syntax: $0 page"
	echo "page: a number within 1-5"
}

if [ $# -le 0 ]; then
	usage
	exit 1
fi

. ./dthadv.sh ""

# $1 is the page number.
page=$1

path=$DTH9P/"RF/BER/3G/SLOT"$page

# next are in order:
# $2 band select. 0 to unset the page
band=$2
if [ "$band" -eq "" ]; then
	dth_set_by_user $path/DlChannel/In_Band $band
	band=$VAR_DTH_USER_VALUE
else
	dth_set $path/DlChannel/In_Band $band
fi

if [ "$band" -ne "0" ]
then
	dth_set_auto $path/DlChannel/In_Uarfcn $3
	dth_set_auto $path/DlChannel/In_NbrBits $4
	dth_set_auto $path/UlChannel/In_TxLevel $5
else
	echo "unset page $page"
fi


#! /bin/sh

usage()
{
	echo "syntax: $0 page"
	echo "page: a number within 1-4"
}

if [ $# -le 0 ]; then
	usage
	exit 1
fi

. ./dthadv.sh ""

# $1 is the page number.
page=$1

path=$DTH9P/"RF/BER/2G/SLOT"$page

# next are in order:
# $2 band select. 0 to unset the page
band=$2
if [ "$band" -eq "" ]; then
	dth_set_by_user $path/In_RfBand $band
	band=$VAR_DTH_USER_VALUE
else
	dth_set $path/In_RfBand $band
fi

if [ "$band" -ne "0" ]
then
	dth_set_auto $path/In_ChannelType $3
	dth_set_auto $path/In_RxSlotPattern $4
	dth_set_auto $path/In_CodedMode $5
	dth_set_auto $path/In_Codec $6
	dth_set_auto $path/In_NbrBlocksBits $7
	dth_set_auto $path/In_TxSlotPattern $8
	dth_set_auto $path/In_Modulation $9
	dth_set_auto $path/In_TxDataType $10
	dth_set_auto $path/In_RxTxLevel $11
	dth_set_auto $path/In_Arfcn $12
else
	echo "unset page $page"
fi


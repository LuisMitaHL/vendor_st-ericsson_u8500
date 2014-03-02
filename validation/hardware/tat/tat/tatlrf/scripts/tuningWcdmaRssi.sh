#! /bin/sh

if [ $# -ne 5 ]         
then                    
	echo "$0 runs WCDMA RSSI tuning"
	echo "arg1 => mid channel (1 .. 1023)"
	echo "arg2 => low channel (1 .. 1023)"
	echo "arg3 => high channel (1 .. 1023)"
	echo "arg4 => sample number (1..255)"
	echo "arg5 => path selection 0=main, 1=diversity"
	exit 1
fi

MID_CHANNEL=$1
LOW_CHANNEL=$2
HIGH_CHANNEL=$3
SAMPLE_NBR=$4
PATH_SEL=$5

cd /mnt/DTH/RF/Tuning/WCDMA/RSSI

echo $MID_CHANNEL | dthfilter -w u16 > In_RxChanelMid/value
echo $LOW_CHANNEL | dthfilter -w u16 > In_RxChanelLow/value
echo $HIGH_CHANNEL | dthfilter -w u16 > In_RxChanelHigh/value
echo $SAMPLE_NBR | dthfilter -w u8 > In_SampleNbr/value
echo $PATH_SEL | dthfilter -w u32 > In_PathSelection/value
echo 0 | dthfilter -w u32 > value
cat value

echo "WCDMA RSSI tuning done."

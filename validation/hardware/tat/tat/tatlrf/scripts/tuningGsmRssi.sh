#! /bin/sh

if [ $# -ne 3 ]         
then                    
	echo "$0 starts GSM RSSI RX"
	echo "arg1 => RF Band (0=850MHz, 1=900MHz, 2=1800MHz, 3=1900MHz)"
	echo "arg2 => Channel (1 .. 1023)"
	echo "arg3 => Level (0.0 .. -100.0)"
	exit 1
fi

RF_BAND=$1
CHANNEL=$2
LEVEL=$3

cd /mnt/DTH/RF/Tuning/GSM/RSSI/Start_RX

echo $RF_BAND | dthfilter -w u32 > /mnt/DTH/RF/Tuning/GSM/RSSI/In_RFBand/value
echo $CHANNEL | dthfilter -w u16 > In_RxChannel/value
echo $LEVEL | dthfilter -w s32 > In_RxSignalLevel/value
echo 0 | dthfilter -w u32 > value
cat value

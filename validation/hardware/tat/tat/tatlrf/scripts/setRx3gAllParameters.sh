#! /bin/sh

if [ $# -ne 6 ]         
then                    
	echo "$0 set all parameters for Start/Stop RX 3G."
	echo "arg1 => Feq Band"
	echo "arg2 => Rx UARFCN channel"
	echo "arg3 => Afc Table Selection"
	echo "arg4 => Afc Value"
	echo "arg5 => Agc Mode"
	echo "arg6 => Expected Power Level"
	exit 1
fi

FEQ_BAND=$1
RX_UARFCN=$2
AFC_TABLE_SELECTION=$3
AFC_VALUE=$4
AGC_MODE=$5
RX_EXPECTED_POWER_LEVEL=$6
	

cd /mnt/DTH/RF/Rx3G/StartRx3G

echo $FEQ_BAND | dthfilter -w u32 > In_FreqBand/value
echo $RX_UARFCN | dthfilter -w u16 > In_RxUarfcnChannel/value
echo $AFC_TABLE_SELECTION | dthfilter -w u8 > In_AfcTabSelect/value
echo $AFC_VALUE | dthfilter -w u32 > In_AfcValue/value
echo $AGC_MODE | dthfilter -w u8 > In_AgcMode/value
echo $RX_EXPECTED_POWER_LEVEL | dthfilter -w s16 > In_ExpectedPower/value

echo "ALL RX3G PARAMETERS ARE SET."

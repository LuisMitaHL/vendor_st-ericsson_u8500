#! /bin/sh

if [ $# -ne 7 ]         
then                    
	echo "$0 Set all parameters for Start/Stop RX 2G5."
	echo "arg1 => Freq Band: GSM850(0), GSM900(1), GSM1800(2), GSM1900(3)"
	echo "arg2 => Rx Channel: u16"
	echo "arg3 => Rx Operation Mode: u16"
	echo "arg4 => Afc Table Selection: TUNED(0) ABS_MANUAL(1) PMM_MANUAL(2)"
	echo "arg5 => Afc Value: u32"
	echo "arg6 => Agc Mode: AUTOMATIC(0) MANUAL(1)"
	echo "arg7 => Expected Power Level: s16"
	exit 1
fi

FREQ_BAND=$1
RX_CHANNEL=$2
RX_OPERATION_MODE=$3
AFC_TABLE_SELECTION=$4
AFC_VALUE=$5
AGC_MODE=$6
RX_EXPECTED_POWER_LEVEL=$7
	

cd /mnt/DTH/RF/Rx2G5/StartRx2G5

echo $FREQ_BAND | dthfilter -w u32 > In_FreqBand/value
echo $RX_CHANNEL | dthfilter -w u16 > In_RxChannel/value
echo $RX_OPERATION_MODE | dthfilter -w u8 > In_RxOperationMode/value
echo $AFC_TABLE_SELECTION | dthfilter -w u8 > In_AfcTabSelect/value
echo $AFC_VALUE | dthfilter -w u32 > In_AfcValue/value
echo $AGC_MODE | dthfilter -w u8 > In_RxAgcMode/value
echo $RX_EXPECTED_POWER_LEVEL | dthfilter -w s16 > In_ExpectedPower/value

echo "ALL PARAMETERS ARE SET."

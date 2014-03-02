#! /bin/sh

if [ $# -ne 5 ]         
then                    
	echo "$0 set all parameters for Start/Stop TX 2G5."
	echo "arg1 => FeqBand"
	echo "arg2 => Tx UARFCN channel"
	echo "arg3 => AfcTableSelection"
	echo "arg4 => AfcValue"
	echo "arg5 => TxPowerLevel"
	exit 1
fi

FEQ_BAND=$1
TX_UARFCN=$2
AFC_TABLE_SELECTION=$3
AFC_VALUE=$4
TX_POWER_LEVEL=$5
	

cd /mnt/DTH/RF/Tx3G/StartTx3G

echo $FEQ_BAND | dthfilter -w u32 > In_FreqBand/value
echo $TX_UARFCN | dthfilter -w u16 > In_TxUarfcnChannel/value
echo $AFC_TABLE_SELECTION | dthfilter -w u8 > In_AfcTabSelect/value
echo $AFC_VALUE | dthfilter -w u32 > In_AfcValue/value
echo $TX_POWER_LEVEL | dthfilter -w u16 > In_TxPowerLevel/value

echo "ALL TX3G PARAMETERS ARE SET."

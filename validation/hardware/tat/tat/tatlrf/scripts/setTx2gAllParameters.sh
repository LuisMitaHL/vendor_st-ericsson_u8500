#! /bin/sh

if [ $# -ne 9 ]         
then                    
	echo "$0 Set all parameters for Start/Stop TX 2G5."
	echo "arg1 => FreqBand: GSM850(0), GSM900(1), GSM1800(2), GSM1900(3)"
	echo "arg2 => TxChannel: u16"
	echo "arg3 => TxOperationMode: u8"
	echo "arg4 => AfcTableSelection: TUNED(0) ABS_MANUAL(1) PMM_MANUAL(2)"
	echo "arg5 => AfcValue: u32"
	echo "arg6 => TxModulationType: GMSK(0) 8PSK(1)"
	echo "arg7 => DataType: 0(0), 1(1), RAND(2), PN9(3), DUMMY(4)"
	echo "arg8 => TxPowerlevelUnit: PCL(0), DBM(1)"
	echo "arg9 => TxPowerLevel: u16"
	exit 1
fi

FREQ_BAND=$1
TX_CHANNEL=$2
TX_OPERATION_MODE=$3
AFC_TABLE_SELECTION=$4
AFC_VALUE=$5
TX_MODULATION_TYPE=$6
DATA_TYPE=$7
TX_POWER_LEVEL_UNIT=$8
TX_POWER_LEVEL=$9
	

cd /mnt/DTH/RF/Tx2G5/StartTx2G5

echo $FREQ_BAND | dthfilter -w u32 > In_FreqBand/value
echo $TX_CHANNEL | dthfilter -w u16 > In_TxChannel/value
echo $TX_OPERATION_MODE | dthfilter -w u8 > In_TxOperationMode/value
echo $AFC_TABLE_SELECTION | dthfilter -w u8 > In_AfcTableSelection/value
echo $AFC_VALUE | dthfilter -w u32 > In_AfcValue/value
echo $TX_MODULATION_TYPE | dthfilter -w u8 > In_TxModulationType/value
echo $DATA_TYPE | dthfilter -w u8 > In_Data_type/value
echo $TX_POWER_LEVEL_UNIT | dthfilter -w u8 > In_TxPowerLevelUnit/value
echo $TX_POWER_LEVEL | dthfilter -w u16 > In_TxPowerLevel/value

echo "ALL PARAMETERS ARE SET."

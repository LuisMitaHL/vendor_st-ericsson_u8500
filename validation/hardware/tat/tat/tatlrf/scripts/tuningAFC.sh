#! /bin/sh

if [ $# -ne 2 ]         
then                    
	echo "$0 runs DCXO AFC tuning"
	echo "arg1 => default AFC (-32768..32767)"
	echo "arg2 => default Coarse (0..32767)"
	exit 1
fi

DEFAULT_AFC=$1
DEFAULT_COARSE=$2
	
ROOT_DTH=/mnt/DTH/RF/Tuning/DCXO/AFC/Run	

echo $DEFAULT_AFC | dthfilter -w s16 > $ROOT_DTH/In_Afc/value
echo $DEFAULT_COARSE | dthfilter -w u16 > $ROOT_DTH/In_Coarse/value
"echo 0 | dthfilter -w u32 > value"
cat $ROOT_DTH/value

echo "DCXO AFC tuning done."

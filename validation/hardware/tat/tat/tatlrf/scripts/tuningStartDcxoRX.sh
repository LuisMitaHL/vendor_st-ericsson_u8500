#! /bin/sh

if [ $# -ne 4 ]         
then                    
	echo "$0 starts DCXO RX tuning"
	echo "arg1 => RF band frequency (1=850MHz, 2=900MHz, 3=1800MHz, 4=1900MHz)"
	echo "arg2 => RX channel (0..1023)"
	echo "arg3 => default AFC (-32768..32767)"
	echo "arg4 => default AGC (0..32767)"
	exit 1
fi

ROOT_DTH=/mnt/DTH/RF/Tuning/DCXO/RX/Start	
	
echo $1 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo $2 | dthfilter -w u16 > $ROOT_DTH/In_Channel/value
echo $3 | dthfilter -w s16 > $ROOT_DTH/In_Afc/value
echo $4 | dthfilter -w u16 > $ROOT_DTH/In_Agc/value
cat $ROOT_DTH/value

echo "DCXO RX tuning start"

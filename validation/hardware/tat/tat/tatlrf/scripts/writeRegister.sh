#! /bin/sh

if [ $# -ne 2 ]         
then                    
	echo "$0 writes data into RFIC register."
	echo "arg1 => addr: address of the register to read."
	echo "arg2 => data: data to write in the RFIC register."
	exit 1
fi

ADRESS=$1
DATA=$2	

cd /mnt/DTH/RF/RFIC/Register
echo $ADRESS | dthfilter -w u16 > In_Address/value
echo $DATA | dthfilter -w u16 > In_Out_Data/value
echo 1 | dthfilter -w u32 > value
cat value
echo "DATA WRITE IN REGISTER."

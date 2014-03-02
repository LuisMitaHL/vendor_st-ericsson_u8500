#! /bin/sh

if [ $# -ne 1 ]         
then                    
	echo "$0 reads data from RFIC register."
	echo "arg1 : address of the register to read in decimal"
	exit 1
fi

ADRESS=$1
	
cd /mnt/DTH/RF/RFIC/Register
echo $ADRESS | dthfilter -w u16 > In_Address/value
echo 0 | dthfilter -w u32 > value
cat value
echo "DATA READ in register : "
cat In_Out_Data/value | dthfilter -r u16

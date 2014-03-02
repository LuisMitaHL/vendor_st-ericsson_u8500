#! /bin/sh

echo "SET CLOCK FREQUENCY"

if [ $# -le 0 ]         
then                    
	echo "arg0 : 	clock value:"
	echo "		0 : 1,01 Mhz"
	echo "		1 : 3,20 Mhz"
	echo "		2 : 3,84 Mhz"
	echo "		3 : 4,80 Mhz"
	exit
fi

echo $1 | dthfilter -w u8 > /mnt/DTH/SMC/ClockFrequency/SetClockFrequency/ClockFrequency/value

cat /mnt/DTH/SMC/ClockFrequency/SetClockFrequency/value

echo "PCLOCK FREQUENCY SET"

#! /bin/sh

echo "STOP CLOCK"

if [ $# -le 0 ]         
then                    
	echo "arg0 : 	clock value:"
	echo "		0 : Stop High"
	echo "		1 : Stop Low"
	exit
fi

echo $1 | dthfilter -w u8 > /mnt/DTH/SMC/CardClock/CardClockStop/SetClockStop/value

cat /mnt/DTH/SMC/CardClock/CardClockStop/value

echo "CLOCK STOPPED"

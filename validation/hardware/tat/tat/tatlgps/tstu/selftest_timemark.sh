#! /bin/sh

echo "GPS SELFT TEST 'TIMEMARK'"

if [ $# -le 0 ]         
then                    
	echo "		0 : ON"
	echo "		1 : OFF"
	exit
fi

echo $1 | dthfilter -w u32 > /mnt/9p/DTH/GPS/SelfTest/GpsTimeMark/value
cat /mnt/9p/DTH/GPS/SelfTest/GpsTimeMark/value

echo "SELF TEST ENDED"

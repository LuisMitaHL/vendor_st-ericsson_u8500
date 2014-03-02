#! /bin/sh

echo "ACTIVATE CARD"

if [ $# -le 0 ]         
then                    
	echo "		0 : 1,2 V"
	echo "		1 : 1,8 V"
	echo "		2 : 3,0 V"
	exit
fi

echo $1 | dthfilter -w u8 > /mnt/DTH/SMC/Card_Activation/Activation/Voltage/value
echo 0 | dthfilter -w u8 > /mnt/DTH/SMC/Card_Activation/Activation/value

cat /mnt/DTH/SMC/Card_Activation/Activation/value

echo "ACTIVATION ENDED"

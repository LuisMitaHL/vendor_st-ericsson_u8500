#! /bin/sh

echo "SET PPS"

if [ $# -le 1 ]         
then                    
	echo "arg0 : Fi Value : 1 to 9"
	echo "arg1 : Di Value : 1 to 9"
	echo "arg2 : T Value : 0 or 1" 
	exit
fi

echo $1 | dthfilter -w u8 > /mnt/DTH/SMC/Set_PPS/PPS/F/value
echo $2 | dthfilter -w u8 > /mnt/DTH/SMC/Set_PPS/PPS/D/value
echo $3 | dthfilter -w u8 > /mnt/DTH/SMC/Set_PPS/PPS/T/value

cat /mnt/DTH/SMC/Set_PPS/PPS/value

echo "PPS SET"

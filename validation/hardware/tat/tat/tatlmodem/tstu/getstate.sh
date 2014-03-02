#! /bin/sh

if [ $# -ne 0]         
then                    
	echo "$0 get modem state: this command need no parameter"
	exit 1
fi

cd /mnt/9p/DTH/MODEM/Context/State
echo "Modem state: "
cat value | dthfilter -r u32

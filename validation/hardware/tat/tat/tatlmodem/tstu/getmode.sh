#! /bin/sh

if [ $# -ne 0]         
then                    
	echo "$0 get modem mode: this command need no parameter"
	exit 1
fi

cd /mnt/9p/DTH/MODEM/Context/Mode
echo "Modem mode: "
cat value | dthfilter -r u32

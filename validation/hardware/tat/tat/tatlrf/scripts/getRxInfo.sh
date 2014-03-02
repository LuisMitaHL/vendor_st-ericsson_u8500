#! /bin/sh

if [ $# -ne 0]         
then                    
	echo "$0 Rf RX RSSI info read: this command need no parameter"
	exit 1
fi

ADRESS=$1
	
cd /mnt/DTH/RF/GetRX/GetRfInfo
cat value
echo "RF RSSI INFO READ "
cat Out_RSSI/value | dthfilter -r s16

#!/bin/sh
#monitor msa execution
#If msa daemon is stopped, it must be restarted to ensure
#communication with the modem

while [ 1 ]; do

if [ $(ps | grep /system/bin/msa | grep -v grep | wc -l ) -lt 1 ]
then
	exec /system/bin/logwrapper /system/bin/msa -l stdout &
fi
sleep 10
done

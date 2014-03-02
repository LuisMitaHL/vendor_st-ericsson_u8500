#!/bin/sh
case $1 in
start) 
	echo "Starting StartISI"
	ISI_MAJOR=`grep IPC_ISA /proc/devices | busybox awk '{print $1}'`
	[ -n "$ISI_MAJOR" ] && mknod /dev/isi c "$ISI_MAJOR" 0
;;
stop) 	
	echo "Stopping StartISI..."
	echo "Nothing to be done"
;;
esac

exit

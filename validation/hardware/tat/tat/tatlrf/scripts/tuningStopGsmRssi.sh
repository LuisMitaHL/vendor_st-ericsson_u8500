#! /bin/sh

cd /mnt/DTH/RF/Tuning/GSM/RSSI/Start_RX

echo 1 | dthfilter -w u32 > value
cat value

echo "Stop GSM RSSI tuning"

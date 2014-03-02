#! /bin/sh

ROOT_DTH=/mnt/DTH/RF/Tuning/DCXO/RX/Stop

"echo 1 | dthfilter -w u32 > $ROOT_DTH/value"
cat $ROOT_DTH/value

echo "Stop DCXO RX tuning"

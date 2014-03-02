#! /bin/sh

echo "DEACTIVATE CARD"

echo 1 | dthfilter -w u8 > /mnt/DTH/SMC/Card_Activation/Activation/value

cat /mnt/DTH/SMC/Card_Activation/Activation/value

echo "DEACTIVATION ENDED"

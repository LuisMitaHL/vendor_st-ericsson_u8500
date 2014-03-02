#! /bin/sh

echo "MODEM INIT ..."  
cd /mnt/DTH/RF/Isi/IsiLinkMngt
echo 1 | dthfilter -w u32 > value
cat value


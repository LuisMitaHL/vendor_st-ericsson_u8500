#! /bin/sh

echo "MODEM INIT ..."  
mknod /dev/isi c 252 0
cd /mnt/DTH/SMC/Isi/IsiLinkMngt
echo 1 | dthfilter -w u32 > value
cat value


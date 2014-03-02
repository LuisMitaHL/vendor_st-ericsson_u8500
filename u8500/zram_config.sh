#!/data/busybox/bin/

#This sets the zram disk size to 7 percent of the total available memory
sh /data/busybox/setup.sh ;
mem_value=0
value_zram=0
perc_value=0
value_zram1=0
value_zram2=0
#Get the total available from meminfo
mem_value=`cat /proc/meminfo | /data/busybox/bin/grep MemTotal | /data/busybox/bin/awk '{printf("%s\n",$2)}'`
#calculate 7 percent
perc_value=`/data/busybox/bin/echo "7 100" | /data/busybox/bin/awk '{printf$1/$2}'`
value_zram=`/data/busybox/bin/echo "$mem_value $perc_value" | /data/busybox/bin/awk '{printf("%d\n",$1*$2)}'`
#calculate the zram size
value_zram1=`/data/busybox/bin/echo "$value_zram" | /data/busybox/bin/awk '{printf("%d\n",$1/2000)}'`
value_zram2=`/data/busybox/bin/echo "$value_zram" | /data/busybox/bin/awk '{printf("%d\n",$1/2000)}'`

#set the size to individual zram devices
echo $(($value_zram1*1024*1024)) > /sys/block/zram0/disksize
/data/busybox/bin/mkswap /dev/block/zram0
/data/busybox/bin/swapon /dev/block/zram0

echo $(($value_zram2*1024*1024)) > /sys/block/zram1/disksize
/data/busybox/bin/mkswap /dev/block/zram1
/data/busybox/bin/swapon /dev/block/zram1

/system/bin/sh


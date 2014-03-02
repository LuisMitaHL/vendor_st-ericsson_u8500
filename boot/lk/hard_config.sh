#!/bin/sh

MAJOR_VERSION=3

path_toolsbox_tmp=`find $2 \( ! -name \*~ \)|grep /toolsbox/rules.mk |grep hbts |grep validation`
if [ "$path_toolsbox_tmp" = "" ]; then
echo "#define LOADER_WO_HBTS"  >>$1
fi

#find /local/frq09473/views/hbts_033_002 \( ! -name \*~ \)|grep config_hard.opt > tmp.tmp
path_config_hard=`find $2 \( ! -name \*~ \)|grep /hbts_hard_config.opt$`

#echo -n >`dirname $1`/autoconf.mk
#echo -n >`dirname $1`/autoconf.tmp

#for option in `cat hard_config.opt | sed '/#/d'` ; do
for option in `cat $path_config_hard | sed '/#/d'` ; do
echo "#define "$option |sed 's/'"="'/'"\ "'/g' >>$1

#echo $option |sed '/=/d' >>`dirname $1`/autoconf.tmp
done
echo "#define MAJOR_VERSION $MAJOR_VERSION"  >>$1

if [ "$path_toolsbox_tmp" != "" ]; then
     path_splash_image=`dirname $path_config_hard`/splash_image.bmp
     `dirname $path_toolsbox_tmp`/gen_fw_header.py $path_splash_image  >`dirname $1`/splash.h
     if [ -f `dirname $path_toolsbox_tmp`/av8100_fw_v3.bin ] ; then
       `dirname $path_toolsbox_tmp`/gen_fw_header.py `dirname $path_toolsbox_tmp`/av8100_fw_v3.bin  >`dirname $1`/av8100_fw.h
     fi

fi
#path_autoconf=`dirname $1`/autoconf.tmp
#for flag in `cat $path_autoconf` ; do
#echo $flag":=y" >>`dirname $1`/autoconf.mk

#done
#rm $path_autoconf

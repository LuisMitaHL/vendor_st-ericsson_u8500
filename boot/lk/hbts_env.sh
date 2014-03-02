#!/bin/sh

cd $1
pwd


path_toolsbox_tmp=`find $2 \( ! -name \*~ \)|grep /toolsbox/rules.mk |grep hbts |grep validation`

if [ "$path_toolsbox_tmp" != "" ]; then
path_toolsbox=`dirname $path_toolsbox_tmp`
echo "path toolsbox= $path_toolsbox"

if [ ! -d "toolsbox" ]; then
mkdir toolsbox
fi
cd toolsbox

for option in `ls $path_toolsbox` ; do

if [ ! -f $option ]; then
ln -s ../../../validation/hardware/hbts/toolsbox/$option .

fi
done

cd ..
else
if [ "$3" = "clean-lk" ]; then
mkdir toolsbox
touch toolsbox/rules.mk
fi
fi


path_selftests_tmp=`find $2 \( ! -name \*~ \)|grep /self_tests/rules.mk |grep hbts |grep validation`

if [ "$path_selftests_tmp" != "" ]; then
path_selftests=`dirname $path_selftests_tmp`
echo "path selftests= $path_selftests"

if [ ! -d "self_tests" ]; then
mkdir self_tests
fi
cd self_tests

for option in `ls $path_selftests` ; do
if [ ! -f $option ]; then
ln -s ../../../validation/hardware/hbts/self_tests/$option .
fi
done

cd ..
else
if [ "$3" = "clean-lk" ]; then
mkdir self_tests
touch self_tests/rules.mk
fi
fi

if [ "$3" != "clean-lk" ]; then

path_config_hard=`find $2 \( ! -name \*~ \)|grep /hbts_hard_config.opt$`

if [ ! -d $2/$3 ]; then
mkdir -p $2/$3
fi
echo -n >$2/$3/autoconf.mk
echo -n >$2/$3/autoconf.tmp

for option in `cat $path_config_hard | sed '/#/d'` ; do
echo $option |sed '/=/d' >>$2/$3/autoconf.tmp
done

path_autoconf=$2/$3/autoconf.tmp
for flag in `cat $path_autoconf` ; do
echo $flag":=y" >>$2/$3/autoconf.mk

done
rm $path_autoconf

fi

#!/bin/sh
#some available version status on 2 juil 2010
#patch_p20091009/     prod6.5.3/           prod6.5.3_p20090424/ prod6.5.3_p20090527/ ref_patch_p20090717/ ref_patch_p20091125/ ref_patch_p20100310/
#prod6.5.3_p20090527 works fine but require fix (cp /sw/lauterbach/trace32/ref_patch_p20100310/linux* /sw/lauterbach/trace32/prod6.5.3_p20090527)
#others does hangs on my ubuntu
#TRACE32_VERSION=prod6.5.3_p20090527 ../../imaging/ite/lauterbach/mmte_linux_launch.sh  e09050017370

echo $PWD|grep 'multimedia/shared/lauterbach' || { printf "run this script from multimedia/shared/lauterbach directory\n"; exit 1; }

. ../mmenv/lauterbach.sh


if [ ! -d ${T32SYS} ]; then
        printf "T32SYS environment variable point to ${T32SYS} which isn't a directory, please check your env\n";
        exit 1;
fi;

if [ -z "${1}" ]; then
        printf "lauterbach name or ip address is mandatory as first arg\n";
        exit 1;
fi;

if [ -n "${TRACE32_VERSION}" ]; then
        export T32SYS=/sw/lauterbach/trace32/${TRACE32_VERSION}
        export PATH=${T32SYS}:${PATH}
        unset LD_LIBRARY_PATH
fi;



#${T32SYS}/t32marm

perl -S ../launcher/launcher.pl --debug_port=T32LINUX:${1} --platform=href_v1-linux --config DBG_USER=../../imaging/ite/lauterbach/axf/load_mmte-linux.cmm

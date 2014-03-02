#!/bin/sh

###########################################################
# 	Copyright (C) ST-Ericsson SA 2012. All rights reserved.
# 	This code is ST-Ericsson proprietary and confidential.
# 	Any use of the code for whatever purpose is subject to
# 	specific written permission of ST-Ericsson SA.
#############################################################

abspath=`readlink -f $0`
mydir=`dirname $abspath`
MEDIA=
TARGET=
PLATFORM=
BUILD_TOP=
OS=
PRODUCT=

## Check if some arguments are provided
if [ $# = 1 ] ; then
    TARGET=$1
elif [ $# = 2 ] ; then
    TARGET=$1
    MEDIA=$2
elif [ $# = 3 ] ; then
    TARGET=$1
    MEDIA=$2
    PRODUCT=$3
else
  echo "Error: invalid number of arguments"
  echo "Usage: tools.sh [PLATFORM] [MEDIA]"
  echo "   Take 1 or 2 arguments "
  echo "   Where PLATFORM u8500 or u9540"
  echo "   Where MEDIA is audio/video/imaging"
  echo "   Will then source file sourceme under [MEDIA]/makefiles"
  return 1
fi

echo "abspath: $mydir"

#TARGET=`echo $TARGET_PRODUCT | tr -s '_' ' ' | awk '{print $2}'`

# Check if we are calling from Android or LBP environment
if [ -z $ANDROID_BUILD_TOP ] ; then
  # Not in Android file tree, then assumin LBP environment
  if [ -z $LBP_BUILD_TOP ] ; then
    echo "Can not guess environment neither ANDROID_BUILD_TOP nor LBP_BUILD_TOP variables are defined"
    return 1
  fi

  echo "Assuming LBP environment"
  OS="linux"
  BUILD_TOP=$LBP_BUILD_TOP
else
  echo "Found ANDROID environment"
  OS="android"
  BUILD_TOP=$ANDROID_BUILD_TOP
fi


if [ "$PRODUCT" = "android" ] ; then
  OUT_STE_PLATFORM=`basename $ANDROID_PRODUCT_OUT`
elif [ "$PRODUCT" = "lbp" ] ; then
  OUT_STE_PLATFORM=$TARGET
elif [ "$PRODUCT" = "x86" ] ; then
  OUT_STE_PLATFORM=$PRODUCT
fi

if [ "$PRODUCT" = "" ] ; then
  ABS_PRODUCT_OUT=$ANDROID_PRODUCT_OUT
else
  ABS_PRODUCT_OUT=${LBP_BUILD_TOP}/out/${OUT_STE_PLATFORM}
fi



if [ "$TARGET" == "u8500" ] ; then
  PLATFORM="${TARGET}_v2-${OS}"
elif [ "$TARGET" == "u9540" ] ; then
  PLATFORM="${TARGET}_v1-${OS}"
fi

if [ $PLATFORM == "" ] ; then
  echo "Error: PLATFORM is unknown!!"
  return 1;
fi






export PLATFORM

export TOOLS_PATH=$BUILD_TOP/vendor/st-ericsson/tools

################################################################################
# Support for MMDSP compilation
################################################################################

export DSPTOOLS=$TOOLS_PATH/host/mmdsp
export PATH=$DSPTOOLS/bin/Linux:$PATH
export MCPROJECT=$DSPTOOLS/target/project/nomadik/STn8500/audio
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DSPTOOLS/lib/Linux


################################################################################
# Support for ARM RVCT compilation
################################################################################

ARM_TOOLSET_INSTALL_ROOT_DIR="/sw/st/gnu_compil/comp/arm/rvds"
ARM_TOOLS_INSTALL_PATH=

if [ -z $MY_ARM_TOOLS_PATH] ; then
  # try to guess path for same toolset installed under /sw/st/gnu_compil/comp/arm/rvds
  echo "Try to guess same ARM toolset from $ARM_TOOLSET_INSTALL_ROOT_DIR to get access to full toolset"
  guess_toolset=`cd $TOOLS_PATH/host/rvct;git log -1 | grep build | sed "s,.*RVCT\(.*\),\1,g" | tr " " "*" | xargs find -L $ARM_TOOLSET_INSTALL_ROOT_DIR -maxdepth 2 -type d -name`

  if [ ! -d $guess_toolset ] ; then
    echo "Can not guess toolset, please provide variable MY_ARM_TOOLS_PATH"
    return 1
  fi
  ARM_TOOLS_INSTALL_PATH=$guess_toolset
else
  echo "Using ARM toolset from: $MY_ARM_TOOLS_PATH"
  ARM_TOOLS_INSTALL_PATH=$MY_ARM_TOOLS_PATH
fi
tmp="$ARM_TOOLS_INSTALL_PATH/linux/RVDS/Contents"
arm_info=`find -L $tmp -maxdepth 3 -name 'enable.xml' | sed "s,$tmp,,g" | xargs dirname | tr "/" " "`

echo "Configuring ARM toolset"
echo "$ARM_TOOLS_INSTALL_PATH/linux/bin/linux-pentium/armenv -r $ARM_TOOLS_INSTALL_PATH/linux/ -p RVDS Contents $arm_info -v platform linux-pentium --sh"
eval `$ARM_TOOLS_INSTALL_PATH/linux/bin/linux-pentium/armenv -r $ARM_TOOLS_INSTALL_PATH/linux/ -p RVDS Contents $arm_info -v platform linux-pentium --sh`

# Use site license whenever possible to reduce number of access to license server
# License with RCVT 4.0 is held for 20s when site license is available
export RVCT40_CCOPT="--fastlicense --sitelicense"
export RVCT40_LINKOPT="--fastlicense --sitelicense"
export RVCT40_ASMOPT="--fastlicense --sitelicense"

DOMAINE_NAME=`domainname`
# Grenoble
if [ "$DOMAINE_NAME" = "vdp1net" ] ; then
 ARMLMD_LICENSE_FILE="8224@gnx333:8224@gnx334:8224@gnx335:$ARMLMD_LICENSE_FILE"
elif [ "$DOMAINE_NAME" = "spa.nis.st" ] ; then
# SOPHIA
 ARMLMD_LICENSE_FILE="1744@margaux.spa.st.com:1744@sophiatmp.spa.st.com:1744@graves.spa.st.com:$ARMLMD_LICENSE_FILE"
elif [ "$DOMAINE_NAME" = "lud.stericsson.com" ] ; then
# LUND
 ARMLMD_LICENSE_FILE="5223@ldlic11.lud.stericsson.com:5223@ldlic12.lud.stericsson.com:5223@ldlic13.lud.stericsson.com:$ARMLMD_LICENSE_FILE"
elif [ "$DOMAINE_NAME" = "NIS.DLH.ST.COM" ] ; then
# NOIDA
 ARMLMD_LICENSE_FILE="8232@delec1rd.dlh.st.com:8232@delec2rd.dlh.st.com:8232@delec3rd.dlh.st.com:$ARMLMD_LICENSE_FILE"
elif [ "$DOMAINE_NAME" = "falcon" ] ; then
# BANGALORE
 ARMLMD_LICENSE_FILE="8232@bngs003.bnr.st.com:8232@bngs002.bnr.st.com:8232@bngs001.bnr.st.com:$ARMLMD_LICENSE_FILE"
elif [ "$DOMAINE_NAME" = "nxplms" ] ; then
# LE-MANS
 ARMLMD_LICENSE_FILE="1702@lme3st01.lme.st.com:$ARMLMD_LICENSE_FILE"
else
    echo "I was not able to determin from which STE site I am called !!"
    echo "ARM license server variable ARMLMD_LICENSE_FILE will not be set"
    return 1;
fi

if [ -z $ARMLMD_LICENSE_FILE ] ; then
 export ARMLMD_LICENSE_FILE
fi


################################################################################
# Support for XP70 SIA COMPILATION
################################################################################

################################################################################
# From STxP70.sh
SX=${TOOLS_PATH}/host/xp70_sia
export SX

# Needed only when building from STWorkench
#STWORKBENCH:=$(SX)/stworkbench
#export STWORKBENCH
#STJRE:=$(SX)/jre
#export STJRE

################################################################################
# From STxP70_environment.sh
SXARCHITECTURE=stxp70v3
export SXARCHITECTURE

################################################################################
# From toolset_version.sh
SXVER=4.0.0
export SXVER

################################################################################
# From compiler_version.sh
STXP70CC_VERSION=4.2
export STXP70CC_VERSION

################################################################################
# From STxP70_addon_environment.sh
# Normally not needed since only used when building extensions
# Assumption is that XP70 extensions are already build if used in the toolset we are providing in the XP70 toolset GIT
#PATH:=$(SX)/corxpert/stxp70v3/bin:${PATH}

################################################################################
# From STxP70_environment.sh
# Normally not needed since only used when building extensions
# Assumption is that XP70 extensions are already build if used in the toolset we are providing in the XP70 toolset GIT
#PATH:=$(STJRE)/bin:$(PATH)
#PATH:=$(SX)/gnu/4.1.2/i686-pc-linux-gnu/bin:$(PATH)
#PATH:=$(SX)/gnu/4.1.2/bin:$(PATH)
PATH=${PATH}:${SX}/bin
PATH=${PATH}:${SX}/stxp70cc/${STXP70CC_VERSION}/bin

# Normally not needed since only used when building extensions
# Assumption is that XP70 extensions are already build if used in the toolset we are providing in the XP70 toolset GIT
# LD_LIBRARY_PATH:=$(SX)/reconftoolkit/lib:$(LD_LIBRARY_PATH)
#LD_LIBRARY_PATH:=$(SX)/reconftoolkit/cmodel/host/lib:$(LD_LIBRARY_PATH)

LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${SX}/bin
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${SX}/stxp70cc/${STXP70CC_VERSION}/lib/cmplrs



##########################
# Support for Lauterbach
###########################
export LAUTERBACH_VER ref_patch_p20110303

export T32SYS=/sw/lauterbach/trace32/${LAUTERBACH_VER}
export LD_LIBRARY_PATH=${T32SYS}:${LD_LIBRARY_PATH}
export T32TMP=/tmp
export T32ID=T32
if [ -z $T32CONFIG ] ; then
  export T32CONFIG="${HOME}/.t32/config.t32,"
fi

export PATH=${T32SYS}/bin/pc_linux:${PATH}
if [ "$DOMAINE_NAME" = "vdp1net" ] ; then
  export RLM_LICENSE=27010@gnx10201
fi
alias=t32mm='t32marm -c $T32CONFIG'

#t32_fonts_setup
#xset +fp /sw/lauterbach/trace32/${LAUTERBACH_VER}/fonts
#xset fp rehash



# for some legacy
export MMROOT=$BUILD_TOP/vendor/st-ericsson/multimedia
export MMBUILDOUT=$ABS_PRODUCT_OUT/obj/mmbuildout
export MM_MAKEFILES_DIR=$MMBUILDOUT/mkfiles
export PERLLIB=$MMROOT/shared/makefiles/perllibs:$PERLLIB

if [ "$MEDIA" != "" ] ; then
  if [ -e "$MMROOT/$MEDIA/makefiles/sourceme" ] ; then
    echo "sourcing Audio environment"
    source $MMROOT/$MEDIA/makefiles/sourceme.sh
  fi
fi

#use PERL5LIB since MM android makefile override PERLLIB
export PERL5LIB=$PERLLIB


source $BUILD_TOP/vendor/st-ericsson/multimedia/shared/mmenv/strip_var.sh PATH LD_LIBRARY_PATH MANPATH INFOPATH PERLLIB PERL5LIB



echo MMROOT   =$MMROOT
echo PLATFORM =$PLATFORM
echo DSPTOOLS =$DSPTOOLS
echo ac: `which ac`
echo ARM RVDS=$ARM_TOOLS_INSTALL_PATH
echo MCPROJECT=$MCPROJECT
echo MMBUILDOUT=$MMBUILDOUT
echo MM_MAKEFILES_DIR=$MM_MAKEFILES_DIR

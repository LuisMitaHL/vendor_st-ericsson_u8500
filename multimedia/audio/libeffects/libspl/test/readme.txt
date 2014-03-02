for M4 or CA9 testing
---------------------
mv  ../../../tools/ARM/sim  ../../../tools/ARM/sim_sav
cp -R -f sim ../../../tools/ARM/.
ln -s ca9sim cm4sim
setMMROOT
uncomment all Makefiles CORE=mmdsp and NO_CA9=1


for M4 testing
--------------

type u8500_v2-linux
type M4
which armcc returns /sw/st/gnu_compil/comp/arm/rvds/4.1-sp1-rvct-build-631/linux/RVCT/Programs/4.1/561/linux-pentium/armcc
cd ../..
make clean
make
cd effect/test
spl.t -M4

for CA9 testing
-------------------

type u8500_v2-linux
type CA9
which armcc returns /sw/st/gnu_compil/comp/arm/rvds/4.0-sp3-rvct-build-697/linux/RVCT/Programs/4.0/650/linux-pentium/armcc
cd ../..
make clean
make
cd effect/test
spl.t -CA9




###### CURRENT AVGMIPS RESULTS 1 Feb. 2012 using spl.t ######


test	MMDSP(mmdspsim)	CA9(ca9sim)		M4(cm4sim)	M4/MMDSP
------------------------------------------------------------
0		1.70			5.51			4.36		2.6
1		2.94			9.62			7.68		2.6
2		1.72			5.78			4.61		2.7
3		1.69			5.54			4.52		2.7
4		1.94			6.04			5.31		2.7
5		2.43			6.05			5.05		2.1 *
6		6.25			17.93			15.05		2.4
7		2.42			6.06			5.01		2.1 *
8		2.28			6.78			5.98		2.6
9		3.78			8.53			7.79		2.1 *
10		2.59			5.99			5.19		2.0 *

* MIPS saved due to PowerAttackTime=PowerReleaseTime=0 requires no longlong*int multiplications




################################# ENVIRONMENT INFORMATION ################################################




need to add the following 2 commands to /home/glassw/.mytcsh
-------------------------------------------------------------------------------------------------------------
alias M4 'setenv SUPPORT_CORTEXM4_STANDALONE true && source /prj/mobileaudio/users/glassw/myRVDS40env.csh'
alias CA9 'setenv SUPPORT_CORTEXM4_STANDALONE false && source /prj/mobileaudio/users/glassw/myRVDS40env.csh'

/prj/mobileaudio/users/glassw/myRVDS40env.csh
------------------------------------------------------------------------------------------------------------

#!/bin/csh
# ###########################################################
# RVDS 4.0 Environment Variables
# Copyright 2006 ARM Limited. All rights reserved.
#
# To set up the environment variables for RVDS 4.0, source this file
# ########################################################### 
# ###########################################################
# 	Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# 	Support for STE ARMCC installation
# 	This code is ST-Ericsson proprietary and confidential.
# 	Any use of the code for whatever purpose is subject to
# 	specific written permission of ST-Ericsson SA.
#############################################################

switch (`uname`)
    case "Linux":
        set platform=linux-pentium
    breaksw

	default	
  	echo "Linux platform only supported"
endsw

if ( $SUPPORT_CORTEXM4_STANDALONE == "true" ) then
  set ARM_INSTALL_PATHS="/sw/st/gnu_compil/comp/arm/rvds/4.1-sp1-rvct-build-631"
else
  set ARM_INSTALL_PATHS="/sw/st/gnu_compil/comp/arm/rvds/4.0-sp3-rvct-build-697"
endif
set installRoot=
set armenv=
foreach _ipath (${ARM_INSTALL_PATHS})
  if ( -e $_ipath/linux/bin/${platform}/armenv ) then
    set installRoot=${_ipath}/linux
    set armenv=${_ipath}/linux/bin/${platform}/armenv
    break
  endif
end
if ( $armenv == "" ) then
  echo "Unable to find arm compiler on your system"
else
  if ( $SUPPORT_CORTEXM4_STANDALONE == "true" ) then
    eval `${armenv} -r ${installRoot} -p RVDS Contents 4.1 158 -v platform ${platform} --csh`
  else
	eval `${armenv} -r ${installRoot} -p RVDS Contents 4.0 619 -v platform ${platform} --csh`
  endif
endif









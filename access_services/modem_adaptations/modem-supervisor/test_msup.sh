#!/bin/sh
#===============================================================================
#
#          FILE:  test_msup.sh
# 
#         USAGE:  ./test_msup.sh 
# 
#   DESCRIPTION:  
# 
#        AUTHOR: Philippe Begnic (), philippe.begnic@stericsson.com
#       COMPANY: ST-Ericsson
#       CREATED: 03/11/2010 14:13:15 CET
#===============================================================================
# Launch Modem supervisor
#
echo " ========= Test Modem Supervisor ========== "
/system/bin/modem-supervisor &

/system/bin/phonetTestApp -t 46 15 1 5 & 
/system/bin/shmnetlnktestapp 2 &

echo " Generate a Modem Reset using test_mte tool "
# /system/bin/test_mte --assert --group_id=l23 &

echo " Check traces on the terminal "






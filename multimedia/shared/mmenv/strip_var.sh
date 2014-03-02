###########################################################
# 	Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# 	This code is ST-Ericsson proprietary and confidential.
# 	Any use of the code for whatever purpose is subject to
# 	specific written permission of ST-Ericsson SA.
#############################################################

#Usage: source ./strip_var.sh VAR1 VAR2 ...
#strip all variables given in parameter, with respect of order within each one

for ENV in $* ; do
  for v in `env | grep "^$ENV=" | cut -d'=' -f2 | tr : ' '` ; do
	if [ ! -z $tmppath ] ; then 
	    echo $tmppath | tr : '\n' | grep -e '^'${v}'$' > /dev/null
	    if [ $? != 0 ] ; then 
          tmppath=${tmppath}:$v  ; 
        fi
	else
	    tmppath=$v
	fi
  done
  if [ ! -z $tmppath ] ; then
	export $ENV=$tmppath
	tmppath=""
  fi
done



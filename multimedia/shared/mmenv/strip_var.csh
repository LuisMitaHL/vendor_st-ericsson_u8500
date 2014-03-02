###########################################################
# 	Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# 	This code is ST-Ericsson proprietary and confidential.
# 	Any use of the code for whatever purpose is subject to
# 	specific written permission of ST-Ericsson SA.
#############################################################

#Usage: source ./strip_var.csh VAR1 VAR2 ...
#strip all variables given in parameter, with respect of order within each one

foreach ENV ( $* )
    foreach v ( `printenv $ENV | tr : ' '` )
	if ( $?tmppath ) then 
	    echo $tmppath | tr : '\n' | grep -e '^'${v}'$' > /dev/null
	    if ( $? != 0 ) set tmppath=${tmppath}:$v
	else
	    set tmppath=$v
	endif
    end
    if ( $?tmppath ) then
	setenv $ENV $tmppath
	unset tmppath
    endif
end

#!/bin/sh
#	Copyright (C) ST-Ericsson 2011
#	This file provides functions and calls to access a DTH element.
#	It is a framework to ease writting of HATS applications test scripts.
#	Author:	alain.crouzet-nonst@stericsson.com
export DTH9P	# root path of 9P server filesystem

# sets DTH environment for script
dth_env()
{
	if [ "$DTH9P" == "" ]
	then
		echo "initializing DTH 9P test toolkit"
		DTH9P=`mount | grep 9p | awk '{ print $3 }'`/DTH
	fi

	#echo "DTH 9P filesystem: $DTH9P"
}

# return the type name of a DTH element
# $1: path and name of the DTH element
# $VAR_DTH_TYPE: type name returned: s8, u8, s16, u16, s32, u32, s64, u64,
# float or file.
# return 1 on bad argument count
dth_type()
{
	if [ $# -lt 1 ]; then
		echo "not enough argument for dth_type"
		return 1
	fi

	# reading file $p/type will return a string describing the type name
	# but if the element is an array, the primitive type will be followed by
	# (col)(row) arguments. We only need to return the primitive type.
	local p=$1
	VAR_DTH_TYPE=`cat $p/type | awk -F\( '{ print $1 }'`
}

# return name of a DTH element in $VAR_DTH_NAME
dth_name()
{
	# get human readable name for element
	VAR_DTH_NAME=`grep -i "label.*:" "$1/info"`
	if [ -n "$VAR_DTH_NAME" ]
	then
		VAR_DTH_NAME=`echo "$VAR_DTH_NAME" | awk '{ print substr($0, index($0, ":")+1) }'`
	fi
}

# read the value of a file assuming its type is known.
# $1: path and name of the file to read
# $2: type name to decode the file
# $VAR_READ_OUTPUT: value returned
dth_read()
{
	if [ $# -lt 2 ]; then
		echo "not enough argument for dth_read"
		return 1
	fi

	local p=$1
	local t=$2

	if [ $t == "file" -o $t == "string" ]; then
		# just an octet stream, cat is enough
		VAR_READ_OUTPUT=`cat $p`
	else
		# need to convert to human readable
		VAR_READ_OUTPUT=`cat $p | dthfilter -r $t`
	fi
}

# retrieve the value of a DTH element
# the DTH element must not be executable: the method to exec or get the value
# of the element is the same so attempt to read value from an executable will
# trigger the executable, not reading its value.
# $1: path and name of the DTH element
# $VAR_GET_RESULT: global output to return obtained value on get
# return 1 on bad argument count
# return 2 if executable
dth_get()
{
	local p=$1
	dth_type $p

	#echo "dth_get $@"
	unset VAR_GET_RESULT

	if [ $# -lt 1 ]; then
		echo "not enough argument for dth_get"
		return 1
	elif [ ! -f $p/value ]; then
		echo "$1 is not a DTH element"
		return 2
	fi

	if [ ! -x $p/value ]
	then
		dth_read $p/value $VAR_DTH_TYPE
		VAR_GET_RESULT=$VAR_READ_OUTPUT
	else
		echo "dth_get: cannot read executable"
		return 3
	fi

    return 0
}

# get and echo value issued
dth_gete()
{
	local p=$1

	# get element's value
	dth_get $p

	if [ "$?" -ne "0" ]; then
		return $?
	fi

	local prefix=""
	if [ $# -gt 1 ]
	then
		case $2 in
		name)
			dth_name $p
			prefix="$VAR_DTH_NAME: "
			;;
		fpath)
			prefix="$p: "
			;;
		esac
	fi

	if [ -n "$VAR_GET_RESULT" ]; then
		echo "$prefix $VAR_GET_RESULT"
	else
		echo "$prefix no value or error occurred!"
	fi
}

dth_get_children()
{
	local dir=$1
	for file in `find $dir -name value`
	do
		if [ -r "$file" ]; then
			dth_gete `dirname $file` "$2"
		fi
	done
}

# sets the value of a writable DTH element
# $1: path and name of the DTH element
# $2: value to set, which should match the DTH argument's specification
# return 1 on bad argument count
# return 2 if the pathname specified does not exist (DTH type 'file' only)
dth_set()
{
	#echo "dth_set $@"

	local p=$1
	local v=$2

	if [ $# -lt 2 ]; then
		echo "not enough argument for dth_set"
		return 1
	fi

	dth_type $p

	if [ $VAR_DTH_TYPE == "file" -o $VAR_DTH_TYPE == "string" ]
	then
		# $v has to be a filename to serve as source
		if [ -f $v ]; then
			cat $v > $p/value
		else
			echo "dth_set: \$2 is to be an existing filename"
			return 2
		fi
	else
		echo "$v" | dthfilter -w $VAR_DTH_TYPE > $p/value
	fi

	return 0
}

# execute a DTH action
# generaly, the command to perform should have been set before.
# Prefer dth_set_exec() which performs both operations, if the action has
# commands.
# $1: path and name of the DTH element
# return 1 on bad argument count
# return 2 if not executable
dth_exec()
{
	#echo "dth_exec $@"

	if [ $# -lt 1 ]; then
		echo "not enough argument for dth_exec"
		return 1
	fi

	if [ -x "$1/value" ]; then
		cat "$1/value"
	else
		echo "dth_exec: not executable"
		return 2
	fi
}

# sets command of an action before executing it.
# $1: path and name of the DTH element
# $2: value to set, which should match the DTH argument's specification
dth_set_exec()
{
	#echo "dth_set_exec $@"

	local p=$1
	local v=$2

	dth_set $p $v
	dth_exec $p
}

# Return min and max of the DTH type.
# $1: DTH type name
# return if the DTH type range is defined then 0 is returned and
# VAR_DTH_RANGE_MIN and VAR_DTH_RANGE_MAX are filled. Else, 1 is returned and
# VAR_DTH_RANGE_MIN and VAR_DTH_RANGE_MAX are both set to 0.
dth_type_range()
{
	VAR_DTH_RANGE_MIN=0
	VAR_DTH_RANGE_MAX=0

	case $1 in
	s8)
		VAR_DTH_RANGE_MIN=-128
		VAR_DTH_RANGE_MAX=127
		;;
	u8)
		VAR_DTH_RANGE_MIN=0
		VAR_DTH_RANGE_MAX=255
		;;
	s16)
		VAR_DTH_RANGE_MIN=-32768
		VAR_DTH_RANGE_MAX=32767
		;;
	u16)
		VAR_DTH_RANGE_MIN=0
		VAR_DTH_RANGE_MAX=65535
		;;
	s32)
		VAR_DTH_RANGE_MIN=-2147483648
		VAR_DTH_RANGE_MAX=2147483647
		;;
	u32)
		VAR_DTH_RANGE_MIN=0
		VAR_DTH_RANGE_MAX=4294967295
		;;
	s64)
		VAR_DTH_RANGE_MIN=-9223372036854775808
		VAR_DTH_RANGE_MAX=9223372036854775807
		;;
	u64)
		VAR_DTH_RANGE_MIN=0
		VAR_DTH_RANGE_MAX=18446744073709551615
		;;
	float)
		# a float is encoded as a signed 32 bits so its limits are the same
		VAR_DTH_RANGE_MIN=-2147483648
		VAR_DTH_RANGE_MAX=2147483647
		;;
	*)
		return 1
	esac

	return 0
}

# Return min and max of the DTH element.
# $1: path and name of the DTH element
# return if the element's range is defined then 0 is returned and
# VAR_DTH_RANGE_MIN and VAR_DTH_RANGE_MAX are filled. Else, 1 is returned and
# VAR_DTH_RANGE_MIN and VAR_DTH_RANGE_MAX are both set to 0.
dth_range()
{
	local p=$1

	dth_type $p
	local DTHTYPE=$VAR_DTH_TYPE
	dth_type_range $DTHTYPE

#	echo "$p theorical range: $VAR_DTH_RANGE_MIN-->$VAR_DTH_RANGE_MAX"

	if [ -f $p/min -a -f $p/max ]
	then
		local DTHTYPEMIN=$VAR_DTH_RANGE_MIN
		local DTHTYPEMAX=$VAR_DTH_RANGE_MAX

		if [ "$DTHTYPE" = "float" ]
		then
			dth_read $p/min "s32"
			VAR_DTH_RANGE_MIN=$VAR_READ_OUTPUT

			dth_read $p/max "s32"
			VAR_DTH_RANGE_MAX=$VAR_READ_OUTPUT
		else
			dth_read $p/min $DTHTYPE
			VAR_DTH_RANGE_MIN=$VAR_READ_OUTPUT

			dth_read $p/max $DTHTYPE
			VAR_DTH_RANGE_MAX=$VAR_READ_OUTPUT
		fi

#		echo "$p user range: $VAR_DTH_RANGE_MIN-->$VAR_DTH_RANGE_MAX"

		if [ 1 == `expr $VAR_DTH_RANGE_MIN \>= $VAR_DTH_RANGE_MAX` ]; then
			echo "ERROR: $p -> minimum exceeds maximum!"
		else
			if [ 1 == `expr $VAR_DTH_RANGE_MIN \< $DTHTYPEMIN` ]; then
				echo "warning: $p min out of bound. minimum should be $DTHTYPEMIN!"
				VAR_DTH_RANGE_MIN=$DTHTYPEMIN
			fi

			if [ 1 == `expr $VAR_DTH_RANGE_MAX \> $DTHTYPEMAX` ]; then
				echo "warning: $p max out of bound. maximum should be $DTHTYPEMAX!"
				VAR_DTH_RANGE_MAX=$DTHTYPEMAX
			fi
		fi
	fi

	return 0
}

dth_check_bounds()
{
	local ROOTDIR=$1
	for FILE in `find $ROOTDIR -name value`
	do
		if [ -r "$FILE" ]; then
			dth_range `dirname $FILE`
		fi
	done
}

# more advanced helpers

# Ask user to enter a value then sets it for the DTH element.
# In case the element is a file, the pathname of the file will be asked else
# it is the value. A string separated by semicolomn is to be passed for array.
# $1: path and name of the DTH element
# If the function succeeds then $VAR_DTH_USER_VALUE is set to the value
# entered by user.
dth_set_by_user()
{
	#echo "dth_set_by_user $@"

	local p=$1

	# get human readable name for element
	dth_name $p
	local label=$VAR_DTH_NAME

	# adapt prompt message to type
	dth_type $p
	if [ $VAR_DTH_TYPE != "file" ]; then
		prompt="Enter value for"
	else
		prompt="Enter pathname for"
	fi

	additional=""
	if [ -f $p/enum ]; then
		additional=`cat $p/enum`
	elif [ -f $p/min -a -f $p/max ]
	then
		if [ "$VAR_DTH_TYPE" = "float" ]
		then
			dth_read $p/min "s32"
			local min=$VAR_READ_OUTPUT
			dth_read $p/max "s32"
			local max=$VAR_READ_OUTPUT
		else
			dth_read $p/min $VAR_DTH_TYPE
			local min=$VAR_READ_OUTPUT
			dth_read $p/max $VAR_DTH_TYPE
			local max=$VAR_READ_OUTPUT
		fi

		additional="range: [$min..$max]"
	fi

	# prompt for user
	echo ""
	echo -e "$prompt \033[1m\033[34m $label\033[0m"
	if [ -n "$additional" ]; then
		echo "$additional"
	fi

	read user_input

	# handle response
	if [ $user_input ]
	then
		VAR_DTH_USER_VALUE=$user_input

		#echo "you typed: $user_input"
		dth_set $p $user_input
	else
		return 2
	fi
}

# switch between interactive and declarative set if the value is defined or not.
# this is a convinient way to handle dandling values.
# $1: path of the DTH element
# $2: value to set. If absent then interactive mode is triggered and
# $VAR_DTH_USER_VALUE is set to the value entered.
# return 1 on bad argument count
dth_set_auto()
{
	#echo "dth_set_auto $@"

	if [ $# -eq 1 ]; then
		dth_set_by_user $1
	elif [ $# -eq 2 ]; then
		dth_set $1 $2
	else
		echo "dth_set_auto: wrong number of arguments"
	fi
}

# Main program
#echo $@

# setup environment
dth_env

# handle command line arguments, if not only called for setting purpose
if [ $# -ge 2 ]
then
	# $1 gives the element's relative path
	path=$DTH9P/$1
	#echo $path

	# $2 type of operation:
	#     g(et)      retreive information of a readable DTH element
	#     s(et)      sets value of a writable DTH element
	#     e(xec)     execute a DTH operation (DTH element must have -x
	#                privilege)
	#     i(nteractive)
	#                ask for the value to set then sets it. Entering null aborts
	#                the operation
	#     a(uto)     switch between s and i mode according of the presence of
	#                the value to assign.
	#     r(un)      set a command and executes it
	#     c(hildren) retrieve information of all children of a parent element
	#                (recursive)
	case $2 in
	g)
		dth_get $path
		echo "$VAR_GET_RESULT"
		;;
	s)
		# $3 is the value to set which must match the DTH's definition (array,
		# type, etc)
		dth_set $path $3
		;;
	e)
		dth_exec $path
		;;
	i)
		dth_set_by_user $path
		;;
	a)
		dth_set_auto $path $3
		;;
	r)
		dth_set_exec $path $3
		;;
	c)
		dth_get_children $path $3
		;;
	*)
		# other?
		echo "wrong operation $2"
	esac
fi


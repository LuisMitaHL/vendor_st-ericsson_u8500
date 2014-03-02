#! /bin/sh

echo "Access audio param"

dth=`mount | grep 9p | awk '{ print $3 }'`/DTH
audio=$dth/AUDIO/Param

case $1 in
set)

    if [ $# -ge 3 ]
    then
        echo "set $2 to $3"
        echo $3 | dthfilter -w u32 > $audio/$2/value
    else
        echo 'error: not enough arguments'
    fi
;;

get)
    if [ $# -ge 2 ]
    then
        echo "get $2: "
        cat $audio/$2/value | dthfilter -r u32
    else
        echo 'error: not enough arguments'
    fi
;;

*)
	echo 'syntax: action param [value]'
	echo ' param:  name of the audio parameter'
    echo ' action: "get" or "set"'
	echo ' value:  value to set'
esac

#! /bin/sh

dth=`mount | grep 9p | awk '{ print $3 }'`/DTH
audio=$dth/AUDIO/AudioLoop/AudioLoop

case $1 in
start)

    if [ $# -eq 2 ]
    then
        echo "Start audio loop"
        echo $2 | dthfilter -w u32 > $audio/Interface/value
        echo 0 | dthfilter -w u32 > $audio/value
        cat $audio/value
    else
        echo 'error: interface not specified'
    fi
    ;;

stop)
    echo "Stop audio loop"
    echo 1 | dthfilter -w u32 > $audio/value
    cat $audio/value
    ;;

get)
    echo "Audio loop interface:"
    cat $audio/Interface/value | dthfilter -r u32
    ;;

*)
    echo 'syntax: action [value]'
	echo ' action:  "start", "stop" or "get"'
	echo ' value:   if start, specify interface: 0=DB8500, 1=AB8500 digital, 2=AB8500 analog'
esac

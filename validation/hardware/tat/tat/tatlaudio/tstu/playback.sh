#! /bin/sh

echo "Start or stop playback"

dth=`mount | grep 9p | awk '{ print $3 }'`/DTH
audio=$dth/AUDIO/Playback/Playback

case $1 in
start)
    echo "Start playback"
    echo 0 | dthfilter -w u32 > $audio/value
    cat $audio/value
    ;;

stop)

    echo "Stop playback"
    echo 1 | dthfilter -w u32 > $audio/value
    cat $audio/value
    ;;
*)
    echo 'syntax: action'
	echo ' action: "start" or "stop"'
esac

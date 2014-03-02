#! /bin/sh

dth=`mount | grep 9p | awk '{ print $3 }'`/DTH
audio=$dth/AUDIO

case $1 in
start)

    echo "Init TATLAUDIO"

    # set device0
    echo `./setup.sh set device 0`

    # set source volume to max level
    echo `./setup.sh set source_vol 10`

    # set source to Line in
    echo `./setup.sh set source 0`

    # set sink volume to mid level
    echo `./setup.sh set sink_vol 5`

    # set sink to headset
    echo `./setup.sh set sink 0`
    ;;

stop)
    echo "Uninit TATLAUDIO"

    # stop playback or audio loop
    echo `./audio_loop.sh stop`
    ;;

*)
    echo 'syntax: action'
    echo 'Initialize or deainitialize TATLAUDIO for audio services'
	echo ' action:  "start" or "stop"'
esac

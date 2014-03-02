#! /bin/sh

echo "Get/set audio setting"

dth=`mount | grep 9p | awk '{ print $3 }'`/DTH
audio=$dth/AUDIO/Setup


case $1 in
set)
    if [ $# -eq 3 ]
    then
        case $2 in
        device)
            echo $3 | dthfilter -w u32 > $audio/Device/Device/value
            ;;

        source)
            echo $3 | dthfilter -w u32 > $audio/Source/Source/value
            ;;

        source_vol)
            echo $3 | dthfilter -w u32 > $audio/Source/Volume/value
            ;;

        sink)
            echo $3 | dthfilter -w u32 > $audio/Sink/Sink/value
            ;;

        sink_vol)
            echo $3 | dthfilter -w u32 > $audio/Sink/Volume/value
            ;;

        connector)
            echo $3 | dthfilter -w u32 > $audio/AVConnector/AVConnector/value
            ;;
        *)
            echo 'error: invalid property or not specified'
        esac
    else
        echo 'error: value not specified';
    fi
;;

get)

    case $2 in
    device)
        cat $audio/Device/Device/value | dthfilter -r u32
        ;;

    source)
        cat $audio/Source/Source/value | dthfilter -r u32
        ;;

    source_vol)
        cat $audio/Source/Volume/value | dthfilter -r u32
        ;;

    sink)
        cat $audio/Sink/Sink/value | dthfilter -r u32
        ;;

    sink_vol)
        cat $audio/Sink/Volume/value | dthfilter -r u32
        ;;

    connector)
        cat $audio/AVConnector/AVConnector/value | dthfilter -r u32
        ;;
    *)
        echo 'error: invalid property or not specified'
    esac
;;

*)
    echo 'syntax: operation property [value]'
    echo ''
    echo '  operation: "get" or "set"'
    echo ''
    echo '  properties:'
    echo '  - device:    0= MAIN, 1= HDMI'
	echo '  - source:    0= Analog Line in'
    echo '               1= Analog Microphone 1B'
    echo '               2= Analog Microphone 2'
    echo '               3= Digital Microphone 1 & 2'
    echo '               4= Digital Microphone 3 & 4'
    echo '               5= Digital Microphone 5 & 6'
    echo '               6= Analog Microphone 1A'
	echo '  - source_vol: 0..10'
	echo '  - sink:      0= headset'
    echo '               1= earset'
    echo '               2= hands free'
    echo '               3= vibrator 1'
    echo '               4= vibrator 2'
	echo '  - sink_vol:  0..10'
	echo '  - connector: 0= Audio'
    echo '               1= Video'
esac

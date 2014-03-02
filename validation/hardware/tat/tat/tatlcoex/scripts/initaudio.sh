#!/bin/sh

echo "configure audio by playing one file"
/usr/bin/aplay -qS /usr/share/sounds/alsa/Front_Left.wav &
sleep 2
exit

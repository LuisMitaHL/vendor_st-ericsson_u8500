#!/bin/sh

#enable MSP2

sk-i2c 0 W 0x70 0x11 2 4 3 1 0 1 0
sk-i2c 0 W 0x70 0x10 2
wavconv -d hdmi /usr/share/sounds/alsa/Front_Right.wav fhdmi.wav 
aplay -S -D hw:0,1 fhdmi.wav&

exit

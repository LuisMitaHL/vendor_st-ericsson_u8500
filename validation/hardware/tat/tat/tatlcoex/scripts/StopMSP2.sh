#!/bin/sh

#disable MSP2

pkill aplay
rm -rf fhdmi.wav
sk-i2c 0 W 0x70 0x11 2 4 3 1 0 0 0
sk-i2c 0 W 0x70 0x10 2

exit


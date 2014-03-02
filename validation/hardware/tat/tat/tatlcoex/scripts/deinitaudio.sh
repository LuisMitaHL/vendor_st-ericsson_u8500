#!/bin/sh

read toto
# stop playbackon DB8500
ps| grep "aplay -qS"| grep -v grep |busybox awk -F' ' '{print $1}'| xargs kill -2
#stop AB8500 bus clock
sk-ab W 0xD1b 0x00
exit

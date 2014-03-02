#!/bin/sh
#
# This script will test record and play.
#
# Args: <device> <samplerate> <channels>
#
# ADM-FT-Rec-Play
# This test will record from the specified device for about 12 seconds, and then
# play on HSetOut. During recording, it will first change volume every second,
# and then mute/unmute every second.
#
# ADM-FT-Parallel-Rec-Play
# This test will record audio in parallel from the specified device, and then
# play the recorded audio in parallel on HSetOut. The recordings will be about
# 12 seconds, and the second recording will start 5 seconds after the first one.
#

dev=$1; sr=$2; ch=$3;

echo "### ADM-FT-Rec-Play"
ste-adm-test record /tmp/rec-$dev-$sr-$ch $dev -ch $ch -sr $sr -bytes $(( $sr * $ch * 2 * 13 )) >/tmp/rec-$dev-$sr-$ch.txt &

sleep 1
ste-adm-test setappvol $dev -1000; sleep 1;
ste-adm-test setappvol $dev 0; sleep 1;
ste-adm-test setappvol $dev -1000; sleep 1;
ste-adm-test setappvol $dev 0; sleep 1;
ste-adm-test setappvol $dev -1000; sleep 1;
ste-adm-test setappvol $dev 0; sleep 1;
ste-adm-test setappvol $dev MUTE; sleep 1;
ste-adm-test setappvol $dev 0; sleep 1;
ste-adm-test setappvol $dev MUTE; sleep 1;
ste-adm-test setappvol $dev 0; sleep 1;
ste-adm-test setappvol $dev MUTE; sleep 1;

cat /tmp/rec-$dev-$sr-$ch.txt
ls -l /tmp/rec-$dev-$sr-$ch

sleep 2
ste-adm-test play /tmp/rec-$dev-$sr-$ch HSetOut -ch $ch -sr $sr

sleep 2

echo "### ADM-FT-Parallel-Rec-Play"
echo "First recording started..."
ste-adm-test record /tmp/parallel-rec-1-$dev-$sr-$ch $dev -ch $ch -sr $sr -bytes $(( $sr * $ch * 2 * 13 )) >/tmp/parallel-rec-1-$dev-$sr-$ch.txt &
sleep 5
echo "Second recording started..."
ste-adm-test record /tmp/parallel-rec-2-$dev-$sr-$ch $dev -ch $ch -sr $sr -bytes $(( $sr * $ch * 2 * 13 )) >/tmp/parallel-rec-2-$dev-$sr-$ch.txt &
sleep 12

cat /tmp/parallel-rec-1-$dev-$sr-$ch.txt
ls -l /tmp/parallel-rec-1-$dev-$sr-$ch
cat /tmp/parallel-rec-2-$dev-$sr-$ch.txt
ls -l /tmp/parallel-rec-2-$dev-$sr-$ch

sleep 2
ste-adm-test play /tmp/parallel-rec-1-$dev-$sr-$ch HSetOut -ch $ch -sr $sr &
ste-adm-test play /tmp/parallel-rec-2-$dev-$sr-$ch HSetOut -ch $ch -sr $sr &

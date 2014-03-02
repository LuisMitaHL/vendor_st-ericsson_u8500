#!/bin/sh

ste-adm-test map Mic PHF_MDRC1_IN
ste-adm-test record /dev/null Mic -sr 48000 -ch 1 -bytes $((2*48000*2)) -mintime 1800 -maxtime 2200 -no_stereo_silence

ste-adm-test map Mic PHF_MDRC2_IN
ste-adm-test record /dev/null Mic -sr 48000 -ch 1 -bytes $((2*48000*2)) -mintime 1800 -maxtime 2200 -no_stereo_silence

ste-adm-test map Mic PHF_MDRC3_IN
ste-adm-test record /dev/null Mic -sr 48000 -ch 1 -bytes $((2*48000*2)) -mintime 1800 -maxtime 2200 -no_stereo_silence

ste-adm-test map Mic PHF_MDRC4_IN
ste-adm-test record /dev/null Mic -sr 48000 -ch 1 -bytes $((2*48000*2)) -mintime 1800 -maxtime 2200 -no_stereo_silence

dd if=/dev/zero of=/tmp/silence.pcm bs=$((2*48000*2*2)) count=1
ste-adm-test map Speaker PHF_MDRC5_OUT
ste-adm-test play /tmp/silence.pcm Speaker -maxtime 2200 -mintime 1800

ste-adm-test map Speaker PHF_MDRC6_OUT
ste-adm-test play /tmp/silence.pcm Speaker -maxtime 2200 -mintime 1800

ste-adm-test map Speaker PHF_MDRC7_OUT
ste-adm-test play /tmp/silence.pcm Speaker -maxtime 2200 -mintime 1800

ste-adm-test map Speaker PHF_MDRC8_OUT
ste-adm-test play /tmp/silence.pcm Speaker -maxtime 2200 -mintime 1800






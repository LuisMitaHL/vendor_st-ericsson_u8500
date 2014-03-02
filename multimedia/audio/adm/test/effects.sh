#!/bin/sh

dd if=/dev/zero of=/tmp/silence.pcm bs=$((2*48000*2*2)) count=1

echo "### ADM-FT-DRC1"
ste-adm-test map Speaker drc_HSetOut1 || { echo "### FAILED ###"; return 2; }
ste-adm-test play /tmp/silence.pcm Speaker -maxtime 2200 -mintime 1800 || { echo "### FAILED ###"; return 2; }

echo "### ADM-FT-DRC2"
ste-adm-test map Speaker drc_HSetOut2 || { echo "### FAILED ###"; return 2; }
ste-adm-test play /tmp/silence.pcm Speaker -maxtime 2200 -mintime 1800 || { echo "### FAILED ###"; return 2; }

echo "### ADM-FT-TEQ1"
ste-adm-test map Speaker teq_HSetOut1 || { echo "### FAILED ###"; return 2; }
ste-adm-test play /tmp/silence.pcm Speaker -maxtime 2200 -mintime 1800 || { echo "### FAILED ###"; return 2; }

echo "### ADM-FT-SPL1"
ste-adm-test map Speaker spl_HSetOut1 || { echo "### FAILED ###"; return 2; }
ste-adm-test play /tmp/silence.pcm Speaker -maxtime 2200 -mintime 1800 || { echo "### FAILED ###"; return 2; }

echo "### Virtual surround on HSetOut, 48 kHz"
ste-adm-test map HSetOut HSetOutVS || { echo "### FAILED ###"; return 2; }
ste-adm-test play /tmp/silence.pcm -ch 6 -sr 48000 HSetOut || { echo "### FAILED ###"; return 2; }

echo "### Downmix 5.1 -> 2, HSetOut, 48 kHz"
ste-adm-test map HSetOut REF_HSetOut || { echo "### FAILED ###"; return 2; }
ste-adm-test play /tmp/silence.pcm -ch 6 -sr 16000 HSetOut || { echo "### FAILED ###"; return 2; }


ste-adm-test map HSetOut REF_HSetOut || { echo "### FAILED ###"; return 2; }
ste-adm-test map Speaker REF_Speaker || { echo "### FAILED ###"; return 2; }








rm -f /tmp/mmte_pipe  /tmp/StartCSCallBg.sh
mkfifo /tmp/mmte_pipe
cat  >/tmp/StartCSCallBg.sh  <<ENDCAT

do /usr/local/MMTE_scripts/StartCSCall.ate OMX.ST.AFM.NOKIA_AV_SOURCE OMX.ST.AFM.NOKIA_AV_SINK

ENDCAT

mmte_bellagio 0</tmp/mmte_pipe 1>/dev/null 2>&1 &
cat /tmp/StartCSCallBg.sh >/tmp/mmte_pipe
sleep 3

#sk-ab w 0xd05 0x87
#sk-ab W 0xd07 0xB4
#sk-ab W 0xd06 0x00
#sk-ab W 0xd14 0x0
#sk-ab W 0xd1f 0x22
#sk-ab W 0xd02 0x20


#sk-ab W 0xd3f 0xC0
sk-ab W 0xd43 0x1F

#sk-ab W 0x1014 0x4
#sk-ab W 0x1024 0x4
#sk-ab W 0x1034 0x4

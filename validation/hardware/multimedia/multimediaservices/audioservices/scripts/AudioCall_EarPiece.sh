rm -f /tmp/mmte_pipe  /tmp/StartCSCallBg.sh
mkfifo /tmp/mmte_pipe
cat  >/tmp/StartCSCallBg.sh  <<ENDCAT

do /usr/local/MMTE_scripts/StartCSCall.ate OMX.ST.AFM.MULTI_MIC OMX.ST.AFM.EARPIECE

ENDCAT

mmte_bellagio 0</tmp/mmte_pipe 1>/dev/null 2>&1 &
cat /tmp/StartCSCallBg.sh >/tmp/mmte_pipe
sleep 3

sk-ab w 0xd08 0x40
sk-ab w 0xd09 0x40
sk-ab w 0xd0a 0x33

# mandatory to ear ringtone on some board
sk-ab w 0xd03 0x80
sk-ab w 0xd05 0x87
sk-ab w 0xd07 0x04
sk-ab w 0xd14 0x5E
sk-ab w 0xd1f 0x12
sk-ab w 0xd3f 0xE7
sk-ab w 0xd47 0x00
sk-ab w 0xd4f 0x00
sk-ab w 0xd64 0x04

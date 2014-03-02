rm -f /tmp/mmte_pipe  /tmp/StartCSCallBg.sh
mkfifo /tmp/mmte_pipe
cat  >/tmp/StartCSCallBg.sh  <<ENDCAT

do /usr/local/MMTE_scripts/StartCSCall.ate OMX.ST.AFM.MULTI_MIC OMX.ST.AFM.IHF

ENDCAT

mmte_bellagio 0</tmp/mmte_pipe 1>/dev/null 2>&1 &
cat /tmp/StartCSCallBg.sh >/tmp/mmte_pipe
sleep 3

#sk-ab w 0xd33 0x48
#sk-ab w 0xd03 0xfc
sk-ab w 0xd49 0x00
sk-ab w 0xd4a 0x00
#sk-ab w 0xd40 0xc0
#sk-ab w 0xd3c 0x0
#sk-ab w 0xd09 0xc
#sk-ab w 0xd3b 0x0
#sk-ab w 0xd3d 0x84
#sk-ab w 0xd08 0xc


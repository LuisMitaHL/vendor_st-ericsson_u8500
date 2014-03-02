
# Script's arguments:
#   $1 : start/stop action

echo AudioCallmpc_Earpiece.sh $1

case $1 in 
0)
nkill mmte_bellagio
rm -f /tmp/mmte_pipe /tmp/StartCSCallBg.ate
sleep 5
echo stopping modem loopback, ready?
;;

1)
rm -f /tmp/mmte_pipe  /tmp/StartCSCallBg.ate
mkfifo /tmp/mmte_pipe
cat  >/tmp/StartCSCallBg.ate  <<ENDCAT
do /usr/local/MMTE_scripts/cscall_mpc_avsourcesink.ate 0 2 2 5000 8000 multimic 0xFFFF earpiece
ENDCAT
mmte_bellagio 0</tmp/mmte_pipe 1>/dev/null 2>&1 &
cat /tmp/StartCSCallBg.ate
cat /tmp/StartCSCallBg.ate >/tmp/mmte_pipe
#DA1 Slots configuration
amixer -q cset iface=MIXER,name='Digital Interface DA 1 From Slot Map' 'SLOT8'
sleep 2
;;

2)
#proper solution but can't make it works  :-(
rm -f /tmp/StopCSCallBg.ate
cat  >/tmp/StopCSCallBg.ate  <<CMD
quit
CMD
cat /tmp/StopCSCallBg.ate
cat /tmp/StopCSCallBg.ate >>/tmp/mmte_pipe
;;

*)
echo invalid argument
;;

esac


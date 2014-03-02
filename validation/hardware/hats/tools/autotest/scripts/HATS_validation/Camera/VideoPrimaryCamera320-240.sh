#!/bin/sh
# Author: Guillaume Camus
export LOGFILE=/var/log/Video_320-240_Primary_Camera.log

if [ -f $LOGFILE ]; then
  rm $LOGFILE
fi

echo " " | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE
echo "****************** Take Video 320*240 Primary camera tests ******************" | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE


#-----------------Start MMTE
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/StartMmte.txt
rm -f /var/log/StartMmte.log
if [ -z "`mount | grep UserFS2`" ]; then
  echo "| Mount UserFS2 :........................................ FAILED |" | tee -a $LOGFILE
  exit
fi
rm -f /mnt/UserFS2/f*

#-----------------Preview primary camera
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Preview_Primary_320-240.txt
sleep 2
rm -f /var/log/Preview_Primary_320-240.log

#-----------------Take Video 320*240
rm -f /mnt/UserFS2/*.bmp
rm -f /mnt/UserFS2/f*
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/StartVideo.txt
TakeVidPrim1=$(grep "Take_Video" /var/log/StartVideo.log | cut -d"." -f 27)
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Pause_Video.txt
PauseVideo=$(grep "Pause_Video" /var/log/Pause_Video.log | cut -d"." -f 27)
ls /mnt/UserFS2
ls /mnt/UserFS2/ > /usr/bin/HATS_validation/Camera/DTHscripts/checklog.log
NBframe1=$(grep -c ^"f" /usr/bin/HATS_validation/Camera/DTHscripts/checklog.log)
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Resume_Video.txt
ResumeVideo=$(grep "Resume_Video" //var/log/Resume_Video.log | cut -d"." -f 27)
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Stop_Video.txt
sleep 3
#StopVideo=$(grep "Stop_Video" //var/log/Stop_Video.log | cut -d"." -f 27)
Stop_Video=$(grep "**END TEST  Stop_Video" /usr/bin/HATS_validation/Camera/DTHscripts/Stop_Video.log | cut -d"." -f 27)
ls /mnt/UserFS2
ls /mnt/UserFS2/ > /usr/bin/HATS_validation/Camera/DTHscripts/checklog.log
NBframe2=$(grep -c ^"f" /usr/bin/HATS_validation/Camera/DTHscripts/checklog.log)

if [ $NBframe1 -lt $NBframe2 ]
	then
	TakeVidPrim2="PASSED"
	else
	TakeVidPrim2="FAILED"
fi

if [ "$TakeVidPrim1" == "FAILED" ] || [ "$TakeVidPrim2" == "FAILED" ] || [ "$Stop_Video" == "FAILED" ] || [ "$PauseVideo" == "FAILED" ] || [ "$ResumeVideo" == "FAILED" ]
	then
	TakeVidPrim="!!!FAILED!!!"
	else
	TakeVidPrim="***PASSED***"
fi
rm -f /var/log/StartVideo.log
rm -f /var/log/Pause_Video.log
rm -f /var/log/Resume_Video.log
rm -f /var/log/Stop_Video.log
rm -f /mnt/UserFS2/f*

#-----------------Stop Preview and stop Idle
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Stop_Preview-idle.txt

#-----------------Stop MMTE
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/StopMmte.txt


# Verdict result
echo " "
echo "Take Video from primary sensor 320*240:......................... $TakeVidPrim" | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE > //dev/null
echo "*****************************************************************************" | tee -a $LOGFILE > //dev/null

echo " "
echo "Tests relsuts have been saved in $LOGFILE"
echo " "

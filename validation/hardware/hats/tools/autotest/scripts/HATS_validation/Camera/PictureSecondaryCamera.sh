#!/bin/sh
# Author: Guillaume Camus
export LOGFILE=/var/log/Secondary_Camera.log

if [ -f $LOGFILE ]; then
  rm $LOGFILE
fi
echo " " | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE
echo "******************* Take Picrtures Secondary camera tests *******************" | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE

# Start MMTE mount UserFS2
cd /

#-----------------Start MMTE
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/StartMmte.txt
rm -f /var/log/StartMmte.log
if [ -z "`mount | grep UserFS2`" ]; then
  echo "| Mount UserFS2 :........................................ FAILED |" | tee -a $LOGFILE
  exit
fi

#-----------------Preview primary camera
sh //usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Preview_Secondary_320-240.txt
sleep 2
CheckVerdict=$(grep "Initialize_secondary" /var/log/Preview_Secondary_320-240.log | cut -d"." -f 27)
if [ "$CheckVerdict" == "PASSED" ]
	then
	InitSecond="***PASSED***"
	else
	InitSecond="!!!FAILED!!!"
fi
CheckVerdict=$(grep "Set_preview" /var/log/Preview_Secondary_320-240.log | cut -d"." -f 27)
if [ "$CheckVerdict" == "PASSED" ]
	then
	SetPreSecond="***PASSED***"
	else
	SetPreSecond="!!!FAILED!!!"
fi
CheckVerdict=$(grep "Set_picture" /var/log/Preview_Secondary_320-240.log | cut -d"." -f 27)
if [ "$CheckVerdict" == "PASSED" ]
	then
	SetPicSecond="***PASSED***"
	else
	SetPicSecond="!!!FAILED!!!"
fi
rm -f /var/log/Preview_Secondary_320-240.log



#-----------------Take Picture in single shot
rm -f /mnt/UserFS2/*.bmp
rm -f /mnt/UserFS2/f*
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Picture_Single.txt
TakePicSing1=$(grep "Take_picture_primary_single_shot" /var/log/Picture_Single.log | cut -d"." -f 27)
ls /mnt/UserFS2
ls /mnt/UserFS2 > /usr/bin/HATS_validation/Camera/DTHscripts/checklog.log
NBPicture=$(grep -c ".bmp"$ //usr/bin/HATS_validation/Camera/DTHscripts/checklog.log)
if [ "$NBPicture" == "1" ]
	then
	TakePicSing2="PASSED"
	else
	TakePicSing2="FAILED"; echo "No good number of pictures saved: $NBPicture needs 1 picture";
fi
if [ "$TakePicSing1" == "PASSED" ] && [ "$TakePicSing2" == "PASSED" ]
	then
	TakePicSing="***PASSED***"
	else
	TakePicSing="!!!FAILED!!!"
fi
rm -f /var/log/Picture_Single.log

#-----------------Take Picture in finite Burst mode
rm -f /mnt/UserFS2/*.bmp
rm -f /mnt/UserFS2/f*
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Picture_Finite-Burst.txt
TakePicFinBurst1=$(grep "Take_picture_primary_finite_burst" /var/log/Picture_Finite-Burst.log | cut -d"." -f 27)
ls /mnt/UserFS2
ls /mnt/UserFS2 > /usr/bin/HATS_validation/Camera/DTHscripts/checklog.log
NBframe=$(grep -c ^"f" /usr/bin/HATS_validation/Camera/DTHscripts/checklog.log)
if [ "$NBframe" == "5" ]
	then
	TakePicFinBurst2="PASSED"
	else
	TakePicFinBurst2="FAILED"; echo "No good number of frame saved: $NBframe needs 5 frames";
fi
if [ "$TakePicFinBurst1" == "PASSED" ] && [ "$TakePicFinBurst2" == "PASSED" ]
	then
	TakePicFinBurst="***PASSED***"
	else
	TakePicFinBurst="!!!FAILED!!!"
fi
rm -f /var/log/Picture_Finite-Burst.log

#-----------------Take Picture in infinite Burst mode
rm -f /mnt/UserFS2/*.bmp
rm -f /mnt/UserFS2/f*
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Picture_Infinite-Burst.txt
TakePicInfBurst1=$(grep "Take_picture_primary_infinite_burst" /var/log/Picture_Infinite-Burst.log | cut -d"." -f 27)
sleep 3
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Pause_Burst.txt
PauseBurst=$(grep "Pause_burst" /var/log/Pause_Burst.log | cut -d"." -f 27)
ls /mnt/UserFS2
ls /mnt/UserFS2/ > /usr/bin/HATS_validation/Camera/DTHscripts/checklog.log
NBframe1=$(grep -c ^"f" /usr/bin/HATS_validation/Camera/DTHscripts/checklog.log)
sh /usr/bin/scripts/DTHScriptInterpreter.sh //usr/bin/HATS_validation/Camera/DTHscripts/Resume_Burst.txt
ResumeBurst=$(grep "Resume_burst" /var/log/Resume_Burst.log | cut -d"." -f 27)
sleep 5
sh /usr/bin/scripts/DTHScriptInterpreter.sh //usr/bin/HATS_validation/Camera/DTHscripts/Stop_Burst.txt
StopBurst=$(grep "Stop_burst" /var/log/Stop_Burst.log | cut -d"." -f 27)
ls /mnt/UserFS2/
ls /mnt/UserFS2/ > //usr/bin/HATS_validation/Camera/DTHscripts/checklog.log
NBframe2=$(grep -c ^"f" /usr/bin/HATS_validation/Camera/DTHscripts/checklog.log)
if [ $NBframe1 -lt $NBframe2 ]
	then
	TakePicInfBurst2="PASSED"
	else
	TakePicInfBurst2="FAILED"
fi
if [ "$TakePicInfBurst1" == "PASSED" ] && [ "$TakePicInfBurst2" == "PASSED" ] && [ "$PauseBurst" == "PASSED" ] && [ "$ResumeBurst" == "PASSED" ] && [ "$StopBurst" == "PASSED" ]
	then
	TakePicInfBurst="***PASSED***"
	else
	TakePicInfBurst="!!!FAILED!!!"
fi
rm -f /var/log/Picture_Infinite-Burst.log
rm -f /var/log/Pause_Burst.log
rm -f /var/log/Resume_Burst.log
rm -f /var/log/Stop_Burst.log
rm -f /mnt/UserFS2/*.bmp
rm -f /mnt/UserFS2/f*

#-----------------Stop Preview and stop Idle
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Stop_Preview-idle.txt

#-----------------Stop MMTE
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/StopMmte.txt



# Verdict result
echo " "
echo "Initialize Secondary Camera:.................................... $InitSecond" | tee -a $LOGFILE
echo "Set Preview Properties Secondary Camera:........................ $SetPreSecond" | tee -a $LOGFILE
echo "Set Picture properties Secondary Camera:........................ $SetPicSecond" | tee -a $LOGFILE
echo "Take Picture from Secondary sensor in single shot mode:......... $TakePicSing" | tee -a $LOGFILE
echo "Take Picture from Secondary sensor in finite burst shot mode:... $TakePicFinBurst" | tee -a $LOGFILE
echo "Take Picture from Secondary sensor in infinite burst shot mode:. $TakePicInfBurst" | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE > /dev/null
echo "*****************************************************************************" | tee -a $LOGFILE > /dev/null
echo " "
echo "Tests results have been saved in $LOGFILE"
echo " "

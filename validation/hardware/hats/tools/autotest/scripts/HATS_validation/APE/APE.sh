#!/bin/sh
# Author: Guillaume Camus
export LOGSAVE=/var/log/CScall.log
CONF_FILE=/mob_conf.txt
rm -f /usr/bin/HATS_validation/APE/CScall.log
if [ -f $LOGSAVE ]; then
  rm $LOGSAVE
fi

echo " "
echo "*********************************************************" | tee -a $LOGSAVE
echo "******************* Mini APE CS Call ********************" | tee -a $LOGSAVE
echo "*********************************************************" | tee -a $LOGSAVE
echo " "

mount /mnt/SDCard
if [ $? -ne 0 ]; then
  echo "| Mount SD :........................................ FAILED |" | tee -a $LOGSAVE
  exit
fi

if [ -f /mnt/SDCard/data_hats_auto/mobconfig$CONF_FILE ]; then
  cp /mnt/SDCard/data_hats_auto/mobconfig$CONF_FILE /
  umount /mnt/SDCard
else
  echo "Could not find $CONF_FILE !"
  echo "| Check $CONF_FILE presence :........................................ FAILED |" | tee -a $LOGSAVE
  umount /mnt/SDCard
  exit
fi

Numero=$(grep "^NumToCall=" /mob_conf.txt | cut -d"=" -f 2)

echo "$Numero"
if [ -f /usr/bin/HATS_validation/APE/CScall.sh ]; then
  rm /usr/bin/HATS_validation/APE/CScall.sh
fi
MAKE=/usr/bin/HATS_validation/APE/CScall.sh
cat /usr/bin/HATS_validation/APE/ScriptCScallPart1.txt | tee -a $MAKE; export MAKE
echo "NUM=$Numero" | tee -a $MAKE; export MAKE
cat /usr/bin/HATS_validation/APE/ScriptCScallPart3.txt | tee -a $MAKE; export MAKE
chmod u+x /usr/bin/HATS_validation/APE/CScall.sh

/usr/bin/HATS_validation/APE/CScall.sh | modem_ape.exe --auto_run &

sleep 65

Modem=$(grep ^GetModem_Mode /usr/bin/HATS_validation/APE/CScall.log | cut -d" " -f 2)
EnableRF=$(grep ^Enable_RF /usr/bin/HATS_validation/APE/CScall.log | cut -d" " -f 2)
Register=$(grep ^Register /usr/bin/HATS_validation/APE/CScall.log | cut -d" " -f 2)
RatGSM=$(grep ^rat_GSM /usr/bin/HATS_validation/APE/CScall.log | cut -d" " -f 2)
CS2G=$(grep ^CS2G /usr/bin/HATS_validation/APE/CScall.log | cut -d" " -f 2)
Release2G=$(grep ^ReleaseCS2G /usr/bin/HATS_validation/APE/CScall.log | cut -d" " -f 2)
RatWCDMA=$(grep ^rat_WCDMA /usr/bin/HATS_validation/APE/CScall.log | cut -d" " -f 2)
CS3G=$(grep ^CS3G /usr/bin/HATS_validation/APE/CScall.log | cut -d" " -f 2)
Release3G=$(grep ^ReleaseCS3G /usr/bin/HATS_validation/APE/CScall.log | cut -d" " -f 2)


if [ "$Modem" == "PASSED" ]
  then
  ModemMode="***PASSED***"
  else
  ModemMode="!!!FAILED!!!"
fi


if [ "$EnableRF" == "PASSED" ] && [ "$Register" == "PASSED" ]
  then
  RegisterState="***PASSED***"
  else
  RegisterState="!!!FAILED!!!"
fi


if [ "$RatGSM" == "PASSED" ] && [ "$CS2G" == "PASSED" ] && [ "$Release2G" == "PASSED" ]
  then
  CScall2G="***PASSED***"
  else
  CScall2G="!!!FAILED!!!"
fi


if [ "$RatWCDMA" == "PASSED" ] && [ "$CS3G" == "PASSED" ] && [ "$Release3G" == "PASSED" ]
  then
  CScall3G="***PASSED***"
  else
  CScall3G="!!!FAILED!!!"
fi

echo " "
echo "Mini APE Modem Mode:........................ $ModemMode" | tee -a $LOGSAVE
echo "Mini APE Register State:.................... $RegisterState" | tee -a $LOGSAVE
echo "Mini APE CS call 2G:........................ $CScall2G" | tee -a $LOGSAVE
echo "Mini APE CS call 3G:........................ $CScall3G" | tee -a $LOGSAVE
echo "*********************************************************" | tee -a $LOGSAVE
echo " "
echo "Logfile result has been saved in $LOGSAVE"

rm -f /usr/bin/HATS_validation/APE/CScall.log

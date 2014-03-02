#!/bin/sh
#Guillaume Camus

#echo "Warning: !!!This test deleted demesg log!!!"
export LOGFILE=/var/log/BB_registerDSI.log
if [ -f $LOGFILE ]; then
  rm $LOGFILE
fi
/usr/bin/HATS_validation/BaseBand_registers/Clocks_Enable.sh

echo " " | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE
echo "***************************** BB Registers DSI ******************************" | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE

#need to display a "video" to test DSI-0 : a preview is OK
#-----------------Start MMTE
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/StartMmte.txt
#-----------------Preview primary camera
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Preview_Primary_640-480.txt

INC=0
while [ "$INC" != 3 ]
 do
 let INC=INC+1
 case "$INC" in
  1 ) Reg=DSI-0; NA=0;;
  2 ) Reg=DSI-1; NA=0;;
  3 ) Reg=DSI-2; NA=0;;
 esac

 if [ "$NA" == 0 ]
 then
  NBline=$(grep -c ^"$Reg" /usr/bin/HATS_validation/BaseBand_registers/BB_register.txt)
  printf "\n\n----------------------------------\n$Reg: $NBline Registers\n----------------------------------\n"
  echo  "Reading registers:"
  readKO=0
  readOK=0
  writeReg=0
  regread=1
  while [ "$NBline" != 0 ]
   do
     echo -e "\b\b\b\c"
     echo -e "$regread\c"

   let NBline=NBline-1
   Name=$(grep ^$Reg.$NBline[^0-9] /usr/bin/HATS_validation/BaseBand_registers/BB_register.txt | cut -d" " -f 2)
   ADR=$(grep ^$Reg.$NBline[^0-9] /usr/bin/HATS_validation/BaseBand_registers/BB_register.txt | cut -d" " -f 3)
   READ=$(grep ^$Reg.$NBline[^0-9] /usr/bin/HATS_validation/BaseBand_registers/BB_register.txt | cut -d" " -f 4)
   dmesg -c > cheklog.txt
   if [ "$READ" == "READ" ] || [ "$READ" == "RW" ]
   then
     BBreading=$(sk.sh db r $ADR 2>&1 | cut -d":" -f 2)
     dmesg > cheklog.txt
     ERline=$(grep -c "" cheklog.txt)

     if [ "$BBreading" == "Bus error" ]||[ "$ERline" != 0 ]
     then
      let readKO=readKO+1;echo "$Name register reading KO Adr: $ADR"
     else
      let readOK=readOK+1
     fi
   else
    let writeReg=writeReg+1
   fi

   let regread=regread+1

   done

   echo -e "\n"
   echo "Reading OK: $readOK"
   echo "Reading KO: $readKO"
   echo "Writing (discarded): $writeReg"


   if [ "$readKO" != 0 ]
    then
     verdict="!!!FAILED!!!"
    else
     verdict="***PASSED***"
   fi
   echo -e "\n"
   echo " "

   case "$Reg" in
    DSI-0  )    echo "Read DSI-0 BB registers:.............. $verdict" | tee -a $LOGFILE;;
    DSI-1  )    echo "Read DSI-1 BB registers:.............. $verdict" | tee -a $LOGFILE;;
    DSI-2  )    echo "Read DSI-2 BB registers:.............. $verdict" | tee -a $LOGFILE;;
    * ) exit;;
  esac
  fi

done

echo " "
rm ./cheklog.txt
#-----------------Stop Preview and stop Idle
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/Stop_Preview-idle.txt
#-----------------Stop MMTE
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Camera/DTHscripts/StopMmte.txt

. /usr/bin/HATS_validation/BaseBand_registers/Clocks_Init.sh


echo " "
echo "******************************** Test End ***********************************" | tee -a $LOGFILE
echo " " | tee -a $LOGFILE
echo "******** Tests relsuts have been saved in $LOGFILE ******"

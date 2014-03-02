#!/bin/sh
#Guillaume Camus
export LOGFILE=/var/log/HDMI_registers.log
if [ -f $LOGFILE ]; then
  rm $LOGFILE
fi
echo "*****************************************************************************" | tee -a $LOGFILE
echo "******************************* HDMI Registers ******************************" | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE

INC=0
while [ "$INC" != 10 ]
	do
	let INC=INC+1
	case "$INC" in
		1 ) Name=STANDBY; ADR=0x00;;
		2 ) Name=HDMI_5_VOLT_TIME; ADR=0x1;;
		3 ) Name=STANDBY_INTERRUPT_MASK; ADR=0x2;;
		4 ) Name=STANDBY_PENDING_INTERRUPT; ADR=0x3;;
		5 ) Name=GENERAL_INTERRUPT_MASK; ADR=0x4;;
		6 ) Name=GENERAL_INTERRUPT; ADR=0x5;;
		7 ) Name=GENERAL_STATUS; ADR=0x6;;
		8 ) Name=GPIO_CONFIGURATION; ADR=0x7;;
		9 ) Name=GENERAL_CONTROL; ADR=0x8;;
		10 ) Name=FIRMWARE_DOWNLOAD_ENTRY; ADR=0xf;;
		11 ) exit;;
	esac


# Reading registers	
  dmesg -c > cheklog.txt
  HDMIread=$(sk.sh av r $ADR | cut -d":" -f 2)
  dmesg > cheklog.txt
  ERline=$(grep -c "" cheklog.txt)
  HDMIsave=$HDMIread
  if [ "$HDMIread" == "ERROR" ]||[ "$ERline" != 0 ]
    then
    verdictRead="FAILED"
    else
    verdictRead="PASSED"
  fi	
  echo -e "\nReading $Name @ $ADR $verdictRead\n"
  

# Writing registers
  if [ "$verdictRead" == "PASSED" ]
  then
   case "$Name" in
		STANDBY )                    DataToWrite=$(let HDMIread=HDMIread+1); sk.sh av w $ADR $DataToWrite;WriteOK=1;;
		HDMI_5_VOLT_TIME )           DataToWrite=$(let HDMIread=HDMIread+1); sk.sh av w $ADR $DataToWrite;WriteOK=1;;
		STANDBY_INTERRUPT_MASK )     DataToWrite=$(let HDMIread=HDMIread+1); sk.sh av w $ADR $DataToWrite;WriteOK=1;;
		STANDBY_PENDING_INTERRUPT )  echo "Cannot write in $Name this register is on read only";WriteOK=0;;
		GENERAL_INTERRUPT_MASK )     DataToWrite=$(let HDMIread=HDMIread+1); sk.sh av w $ADR $DataToWrite;WriteOK=1;;
		GENERAL_INTERRUPT )          echo "Cannot write in $Name this register is on read only";WriteOK=0;;
		GENERAL_STATUS )             DataToWrite=$(let HDMIread=HDMIread+1); sk.sh av w $ADR $DataToWrite;WriteOK=1;;
		GPIO_CONFIGURATION )         DataToWrite=$(let HDMIread=HDMIread+1); sk.sh av w $ADR $DataToWrite;WriteOK=1;;
		GENERAL_CONTROL )            echo "Cannot write in $Name this register is on read only";WriteOK=0;;
		FIRMWARE_DOWNLOAD_ENTRY )    DataToWrite=$(let HDMIread=HDMIread+1); sk.sh av w $ADR $DataToWrite;WriteOK=1;;
		* ) exit;;
	 esac
    if [ "$WriteOK" == 1 ]
     then
     dmesg -c > cheklog.txt
     HDMIread2=$(sk.sh av r $ADR | cut -d":" -f 2)
     dmesg > cheklog.txt
     ERline=$(grep -c "" cheklog.txt)
     if [ "$HDMIread2" != "$DataToWrite" ]||[ "$ERline" != 0 ]
       then
       verdictWrite="FAILED"
       else
       verdictWrite="PASSED"
     fi	
     echo "Writing $Name @ $ADR $verdictWrite"
   fi
  fi  


#Verdict Result
  if [ "$verdictRead" == "FAILED" ]||[ "$verdictWrite" == "FAILED" ]
    then
  	verdict="!!!FAILED!!!"
    else
  	verdict="***PASSED***"
  fi
   
  
  echo " "
  case "$Name" in
		STANDBY )                    echo -e "Read/Write STANDBY HDMI registers:......................$verdict\n" | tee -a $LOGFILE;;
		HDMI_5_VOLT_TIME )           echo -e "Read/Write HDMI_5_VOLT_TIME HDMI registers:.............$verdict\n" | tee -a $LOGFILE;;
		STANDBY_INTERRUPT_MASK )     echo -e "Read/Write STANDBY_INTERRUPT_MASK HDMI registers:.......$verdict\n" | tee -a $LOGFILE;;
		STANDBY_PENDING_INTERRUPT )  echo -e "Read STANDBY_PENDING_INTERRUPT HDMI registers:......... $verdict\n" | tee -a $LOGFILE;;
		GENERAL_INTERRUPT_MASK )     echo -e "Read/Write GENERAL_INTERRUPT_MASK HDMI registers:.......$verdict\n" | tee -a $LOGFILE;;
		GENERAL_INTERRUPT )          echo -e "Read GENERAL_INTERRUPT HDMI registers:..................$verdict\n" | tee -a $LOGFILE;;
		GENERAL_STATUS )             echo -e "Read/Write GENERAL_STATUS HDMI registers:...............$verdict\n" | tee -a $LOGFILE;;
		GPIO_CONFIGURATION )         echo -e "Read/Write GPIO_CONFIGURATION HDMI registers:...........$verdict\n" | tee -a $LOGFILE;;
		GENERAL_CONTROL )            echo -e "Read GENERAL_CONTROL HDMI registers:....................$verdict\n" | tee -a $LOGFILE;;
		FIRMWARE_DOWNLOAD_ENTRY )    echo -e "Read/Write FIRMWARE_DOWNLOAD_ENTRY HDMI registers:..... $verdict\n" | tee -a $LOGFILE;;
		* ) exit;;
	esac
done  
   
   
echo " "
echo "*****************************************************************************" | tee -a $LOGFILE
echo "******************************* HDMI Registers ******************************"
echo "***********************************TEST END**********************************" | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE
echo " "
echo "Tests relsuts have been saved in $LOGFILE"
echo " "

#!/bin/sh
#Guillaume Camus

echo "This test deleted demesg log you can find a copy in: /var/log/savedmesg.txt"

dmesg  > /var/log/savedmesg.txt

export LOGFILE=/var/log/PMU_registers.log

if [ -f $LOGFILE ]; then
	rm $LOGFILE
fi

echo " " | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE
echo "******************************** PMU Registers ******************************" | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE

INC=0
while [ "$INC" != 18 ]
	do
	let INC=INC+1
	case "$INC" in
		1 ) Reg=ACCESS;;
		2 ) Reg=ADC;;
		3 ) Reg=AUDIO;;
		4 ) Reg=CHARGER;;
		5 ) Reg=CTRL;;
		6 ) Reg=GAUGE;;
		7 ) Reg=GPIO;;
		8 ) Reg=INT;;
		9 ) Reg=LATCH;;
		10 ) Reg=M_INT;;
		11 ) Reg=MISC;;
		12 ) Reg=PWMO;;
		13 ) Reg=REGU2;;
		14 ) Reg=REGUL;;
		15 ) Reg=RTC;;
		16 ) Reg=SIM;sk.sh ab W 0x300 0x01  > /dev/null;;
		17 ) Reg=TVOUT;;
		18 ) Reg=USB;;
		* ) exit;;
	esac

	NBline=$(grep -c ^"$Reg" /usr/bin/HATS_validation/PMU_registers/PMU_register.txt)
	printf "\n\n----------------------------------\n$Reg: $NBline Registers\n----------------------------------\n"
	readKO=0
	while [ "$NBline" != 0 ]
		do
		let NBline=NBline-1
		Name=$(grep ^$Reg.$NBline[^0-9] /usr/bin/HATS_validation/PMU_registers/PMU_register.txt | cut -d" " -f 2)
		ADR=$(grep ^$Reg.$NBline[^0-9] /usr/bin/HATS_validation/PMU_registers/PMU_register.txt | cut -d" " -f 3)
		dmesg -c > //usr/bin/HATS_validation/PMU_registers/cheklog.txt
    PMUread=$(sk.sh ab R $ADR 2>&1 | cut -d" " -f 3)
    #echo "read value: $PMUread in $Name register"
    dmesg > //usr/bin/HATS_validation/PMU_registers/cheklog.txt
    ERline=$(grep -c "" /usr/bin/HATS_validation/PMU_registers/cheklog.txt)
    if [ "$ERline" != 0 ]
     then
     echo -e "\nReading  $ADR failed \n";let readKO=readKO+1
    fi
		#printf "$Reg $NBline %5s $Name %5s $ADR\n"
  done
  if [ "$readKO" != 0 ]
   then
   verdict="!!!FAILED!!!"
   else
   verdict="***PASSED***"
  fi

  echo " "
  case "$Reg" in
	  ACCESS )  echo "Read ACCESS PMU registers:.............. $verdict" | tee -a $LOGFILE;;
		ADC )     echo "Read ADC PMU registers:................. $verdict" | tee -a $LOGFILE;;
		AUDIO )   echo "Read AUDIO PMU registers:............... $verdict" | tee -a $LOGFILE;;
		CHARGER ) echo "Read CHARGER PMU registers:............. $verdict" | tee -a $LOGFILE;;
		CTRL )    echo "Read CTRL PMU registers:................ $verdict" | tee -a $LOGFILE;;
		GAUGE )   echo "Read GAUGE PMU registers:............... $verdict" | tee -a $LOGFILE;;
		GPIO )    echo "Read GPIO PMU registers:................ $verdict" | tee -a $LOGFILE;;
		INT )     echo "Read INT PMU registers:................. $verdict" | tee -a $LOGFILE;;
		LATCH )   echo "Read LATCH PMU registers:............... $verdict" | tee -a $LOGFILE;;
		M_INT )   echo "Read M_INT PMU registers:............... $verdict" | tee -a $LOGFILE;;
		MISC )    echo "Read MISC PMU registers:................ $verdict" | tee -a $LOGFILE;;
		PWMO )    echo "Read PWMO PMU registers:................ $verdict" | tee -a $LOGFILE;;
		REGU2 )   echo "Read REGU2 PMU registers:............... $verdict" | tee -a $LOGFILE;;
		REGUL )   echo "Read REGUL PMU registers:............... $verdict" | tee -a $LOGFILE;;
		RTC )     echo "Read RTC PMU registers:................. $verdict" | tee -a $LOGFILE;;
		SIM )     echo "Read SIM PMU registers:................. $verdict" | tee -a $LOGFILE;echo -e "\n";sk.sh ab W 0x300 0x02 > /dev/null;;
		TVOUT )   echo "Read TVOUT PMU registers:............... $verdict" | tee -a $LOGFILE;;
		USB )     echo "Read USB PMU registers:................. $verdict" | tee -a $LOGFILE;;
		* ) exit;;
	esac
done

rm /usr/bin/HATS_validation/PMU_registers/cheklog.txt

echo " "
echo "*****************************************************************************" | tee -a $LOGFILE
echo "******************************** PMU Registers ******************************"
echo "********************************** Test End *********************************"
echo "*****************************************************************************" | tee -a $LOGFILE

echo " "
echo "Tests relsuts have been saved in $LOGFILE"
echo " "

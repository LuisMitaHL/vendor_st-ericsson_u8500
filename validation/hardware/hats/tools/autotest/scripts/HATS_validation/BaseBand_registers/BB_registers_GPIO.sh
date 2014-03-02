#!/bin/sh
#Guillaume Camus

#echo "Warning: !!!This test deleted demesg log!!!"
export LOGFILE=/var/log/BB_registerGPIO.log
if [ -f $LOGFILE ]; then
  rm $LOGFILE
fi
/usr/bin/HATS_validation/BaseBand_registers/Clocks_Enable.sh

echo " " | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE
echo "***************************** BB Registers GPIO *****************************" | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE

INC=0
while [ "$INC" != 9 ]
 do
 let INC=INC+1
 case "$INC" in
  1 ) Reg=GPIO0; NA=0;;
  2 ) Reg=GPIO1; NA=0;;
  3 ) Reg=GPIO2; NA=0;;
  4 ) Reg=GPIO3; NA=0;;
  5 ) Reg=GPIO4; NA=0;;
  6 ) Reg=GPIO5; NA=0;;
  7 ) Reg=GPIO6; NA=0;;
  8 ) Reg=GPIO7; NA=0;;
  9 ) Reg=GPIO8; NA=0;;
 esac

 if [ "$NA" == 0 ]
 then
  NBline=$(grep -c ^"$Reg" /usr/bin/HATS_validation/BaseBand_registers/BB_register.txt)
  printf "\n\n----------------------------------\n$Reg: $NBline Registers\n----------------------------------\n"
  echo  "Reading registers:"
  readKO=0
  readOK=0
  regread=1
  while [ "$NBline" != 0 ]
   do
     echo -e "\b\b\b\c"
     echo -e "$regread\c"

   let NBline=NBline-1
   Name=$(grep ^$Reg.$NBline[^0-9] /usr/bin/HATS_validation/BaseBand_registers/BB_register.txt | cut -d" " -f 2)
   ADR=$(grep ^$Reg.$NBline[^0-9] /usr/bin/HATS_validation/BaseBand_registers/BB_register.txt | cut -d" " -f 3)
   dmesg -c > cheklog.txt
     BBreading=$(sk.sh db r $ADR 2>&1 | cut -d":" -f 2)
     dmesg > cheklog.txt
     ERline=$(grep -c "" cheklog.txt)

     if [ "$BBreading" == "Bus error" ]||[ "$ERline" != 0 ]
     then
      let readKO=readKO+1;echo "$Name regiter reading KO Adr: $ADR"
     else
      let readOK=readOK+1
     fi

   let regread=regread+1

   done

   echo -e "\n"
   echo "Reading OK: $readOK"
   echo "Reading KO: $readKO"

   if [ "$readKO" != 0 ]
    then
     verdict="!!!FAILED!!!"
    else
     verdict="***PASSED***"
   fi
   echo -e "\n"
   echo " "

   case "$Reg" in
    GPIO0  )    echo "Read GPIO0 BB registers:.............. $verdict" | tee -a $LOGFILE;;
    GPIO1  )    echo "Read GPIO1 BB registers:.............. $verdict" | tee -a $LOGFILE;;
    GPIO2  )    echo "Read GPIO2 BB registers:.............. $verdict" | tee -a $LOGFILE;;
    GPIO3  )    echo "Read GPIO3 BB registers:.............. $verdict" | tee -a $LOGFILE;;
    GPIO4  )    echo "Read GPIO4 BB registers:.............. $verdict" | tee -a $LOGFILE;;
    GPIO5  )    echo "Read GPIO5 BB registers:.............. $verdict" | tee -a $LOGFILE;;
    GPIO6  )    echo "Read GPIO6 BB registers:.............. $verdict" | tee -a $LOGFILE;;
    GPIO7  )    echo "Read GPIO7 BB registers:.............. $verdict" | tee -a $LOGFILE;;
    GPIO8  )    echo "Read GPIO8 BB registers:.............. $verdict" | tee -a $LOGFILE;;
    * ) exit;;
  esac
  fi

done

echo " "
rm ./cheklog.txt
/usr/bin/HATS_validation/BaseBand_registers/Clocks_Init.sh

echo " "
echo "******************************** Test End ************************************" | tee -a $LOGFILE
echo " " | tee -a $LOGFILE
echo "******* Tests relsuts have been saved in $LOGFILE *******"

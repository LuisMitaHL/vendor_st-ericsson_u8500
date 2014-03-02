#!/bin/sh
#Guillaume Camus

#echo "Warning: !!!This test deleted demesg log!!!"
export LOGFILE=/var/log/BB_registerHSI2C.log
if [ -f $LOGFILE ]; then
  rm $LOGFILE
fi
/usr/bin/HATS_validation/BaseBand_registers/Clocks_Enable.sh

echo " " | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE
echo "***************************** BB Registers HSI2C ****************************" | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE

INC=0
while [ "$INC" != 5 ]
 do
 let INC=INC+1
 case "$INC" in
  1 ) Reg=HSI2C-0; NA=0;;
  2 ) Reg=HSI2C-1; NA=0;;
  3 ) Reg=HSI2C-2; NA=0;;
  4 ) Reg=HSI2C-3; NA=0;;
  5 ) Reg=HSI2C-4; NA=0;;
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
    HSI2C-0  )   echo "Read HSI2C-0 BB registers:............ $verdict" | tee -a $LOGFILE;;
    HSI2C-1  )   echo "Read HSI2C-1 BB registers:............ $verdict" | tee -a $LOGFILE;;
    HSI2C-2  )   echo "Read HSI2C-2 BB registers:............ $verdict" | tee -a $LOGFILE;;
    HSI2C-3  )   echo "Read HSI2C-3 BB registers:............ $verdict" | tee -a $LOGFILE;;
    HSI2C-4  )   echo "Read HSI2C-4 BB registers:............ $verdict" | tee -a $LOGFILE;;
    * ) exit;;
  esac
  fi

done

echo " "
rm ./cheklog.txt
/usr/bin/HATS_validation/BaseBand_registers/Clocks_Init.sh

echo " "
echo "******************************** Test End ***********************************" | tee -a $LOGFILE
echo " " | tee -a $LOGFILE
echo "****** Tests relsuts have been saved in $LOGFILE ******"

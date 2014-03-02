#!/bin/sh
# Guillaume Camus
#
LOGFILE=/var/log/HATS_P0.log
if [ -f $LOGFILE ]; then
 rm $LOGFILE
fi

echo "*****************************************************************************" | tee -a $LOGFILE
echo '****************************** HATS HW/SW SI P0 *****************************' | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE
echo " " | tee -a $LOGFILE


# sauvegarde des info DMESG
dmesg > /var/log/saved_dmesg.log
CheakDMS=$(grep -c . /var/log/saved_dmesg.log)
if ("$CheakDMS" != 0)
	then
	cat /var/log/saved_dmesg.log > /var/log/dmesg.log
fi
rm -f /var/log/saved_dmesg.log


# récuperation des informations du soft
rm -f /var/log/datasoft.log
LME=$(grep ^"compiled" /etc/version | cut -d" " -f 5)
Month=$(grep ^"compilation" /etc/version | cut -d" " -f 4)
Day=$(grep ^"compilation" /etc/version | cut -d" " -f 5)
Year=$(grep ^"compilation" /etc/version | cut -d" " -f 8)
printf ":" | tee -a /var/log/datasoft.log > /dev/null
grep ^"compilation" /etc/version | cut -d" " -f 6  | tee -a /var/log/datasoft.log > /dev/null
heure=$(grep ^":" /var/log/datasoft.log | cut -d":" -f 2)
min=$(grep ^":" /var/log/datasoft.log | cut -d":" -f 3)
secd=$(grep ^":" /var/log/datasoft.log | cut -d":" -f 4)
rm -f /var/log/datasoft.log


# RF tests
rm -f /var/log/RF.log
/usr/bin/HATS_validation/Radio/rf_test.sh
sleep 2
grep . /var/log/RF.log | tee -a $LOGFILE > /dev/null


# HDMI Registers
#rm /var/log/HDMI_registers.log
#. /usr/bin/HATS_validation/HDMI_registers/HDMI_Registers.sh
#grep . /var/log/HDMI_registers.log | tee -a $LOGFILE > /dev/null


# PMU Registers
rm -f /var/log/PMU_registers.log
/usr/bin/HATS_validation/PMU_registers/PMU_Registers.sh
sleep 2
grep . /var/log/PMU_registers.log | tee -a $LOGFILE > /dev/null


# BaseBand Registers
rm /var/log/BB_register.log
/usr/bin/HATS_validation/BaseBand_registers/baseband_registers.sh
sleep 2
grep . /var/log/BB_register.log | tee -a $LOGFILE > /dev/null


# ADC
rm -f /var/log/ADC_Part1.log
rm -f /var/log/Gas_Gauge.log
echo " " | tee -a $LOGFILE > /dev/null
echo "*******************************************************************" | tee -a $LOGFILE
echo "******************************** ADC ******************************" | tee -a $LOGFILE
echo "*******************************************************************" | tee -a $LOGFILE
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/ADC/ADC_Part1.txt
sleep 2
grep . /var/log/ADC_Part1.log | tee -a $LOGFILE > /dev/null
echo "***************************** GAS Gauge ***************************" | tee -a $LOGFILE
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/ADC/Gas_Gauge.txt
sleep 2
grep . /var/log/Gas_Gauge.log | tee -a $LOGFILE > /dev/null
echo "*******************************************************************" | tee -a $LOGFILE > /dev/null


# CHARGE
rm -f /var/log/Charge.log
echo " " | tee -a $LOGFILE > /dev/null
echo "*******************************************************************" | tee -a $LOGFILE
echo "****************************** CHARGE *****************************" | tee -a $LOGFILE
echo "*******************************************************************" | tee -a $LOGFILE
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Charge/Charge.txt
sleep 2
grep . /var/log/Charge.log | tee -a $LOGFILE > /dev/null
echo "*******************************************************************" | tee -a $LOGFILE > /dev/null


# COEXISTENCE
rm -f /var/log/Agressor.log
rm -f /var/log/Victims.log
echo " " | tee -a $LOGFILE > /dev/null
echo "*******************************************************************" | tee -a $LOGFILE
echo "**************************** COEXISTENCE **************************" | tee -a $LOGFILE
echo "*******************************************************************" | tee -a $LOGFILE
echo "****************************** Agressor ***************************" | tee -a $LOGFILE
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/COEX/Agressor.txt
sleep 2
grep . /var/log/Agressor.log | tee -a $LOGFILE > /dev/null
echo "****************************** Victims ****************************" | tee -a $LOGFILE
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/COEX/Victims.txt
sleep 2
grep . /var/log/Victims.log | tee -a $LOGFILE > /dev/null
echo "*******************************************************************" | tee -a $LOGFILE > /dev/null


# POWER
rm -f /var/log/Power.log
echo " " | tee -a $LOGFILE > /dev/null
echo "*******************************************************************" | tee -a $LOGFILE
echo "******************************* POWER *****************************" | tee -a $LOGFILE
echo "*******************************************************************" | tee -a $LOGFILE
sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Power/Power.txt
sleep 2
grep . /var/log/Power.log | tee -a $LOGFILE > /dev/null
echo "*******************************************************************" | tee -a $LOGFILE > /dev/null



#formatage du nom du log et sauvegarde dans la Sdcard
cd /mnt
mount SDCard
cd /
find  /mnt/SDCard/data_hats_auto/logreturn/ > /Nblog.log
LogID=$( grep -c . /Nblog.log )
rm -f /Nblog.log

echo " " | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE
echo "****************************** Campain Test End *****************************" | tee -a $LOGFILE
echo "*****************************************************************************" | tee -a $LOGFILE


grep . $LOGFILE | tee -a /mnt/SDCard/data_hats_auto/logreturn/log_"$LogID"-HATS_P0-"$LME"-"$Month"-"$Day"-"$Year"-"$heure"h"$min"m"$secd"s.log > /dev/null


echo "P0 HATS tests relsuts have been saved in $LOGFILE"
echo "one copy of result have been saved in SDcard:"
echo "/mnt/SDCard/data_hats_auto/logreturn/log_"$LogID"-HATS_P0-"$LME"-"$Month"-"$Day"-"$Year"-"$heure"h"$min"m"$secd"s.log"

#!/bin/sh
#Guillaume Camus

sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/BaseBand_registers/DVFS_Disable.txt

CLOCKLOG=/usr/bin/HATS_validation/BaseBand_registers/ClockEnableList.log
if [ -f $CLOCKLOG ]; then
	rm /usr/bin/HATS_validation/BaseBand_registers/ClockEnableList.log
fi
path=$(pwd)
cd /sys/kernel/debug/clk/
find [0-9a-zA-Z]* -type d > /usr/bin/HATS_validation/BaseBand_registers/CLKlist.txt
cd $path
grep -n "" /usr/bin/HATS_validation/BaseBand_registers/CLKlist.txt > /usr/bin/HATS_validation/BaseBand_registers/CLKlist2.txt
rm /usr/bin/HATS_validation/BaseBand_registers/CLKlist.txt
NBclk=$(grep -c ^"" /usr/bin/HATS_validation/BaseBand_registers/CLKlist2.txt)

INC=0
while [ "$INC" != "$NBclk" ]
 do
 let INC=INC+1
 clock=$(grep ^$INC[^0-9] /usr/bin/HATS_validation/BaseBand_registers/CLKlist2.txt | cut -d":" -f 2)
 status=$(cat /sys/kernel/debug/clk/$clock/enable)
 if [ "$status" == 0 ]
  then
  echo 1 > /sys/kernel/debug/clk/$clock/enable
  echo "$INC Enable clock $clock" | tee -a $CLOCKLOG;export CLOCKLOG;
 fi
done

rm /usr/bin/HATS_validation/BaseBand_registers/CLKlist2.txt

echo -e "\n******************************* CLK ENABLE **********************************"

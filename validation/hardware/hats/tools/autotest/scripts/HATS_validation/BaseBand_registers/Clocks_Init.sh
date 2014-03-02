#!/bin/sh
#Guillaume Camus

CLK_EN_LOG_FILE=/usr/bin/HATS_validation/BaseBand_registers/ClockEnableList.log

if [ -f $CLK_EN_LOG_FILE ]; then
	NbClkEnable=$(grep -c ^"" $CLK_EN_LOG_FILE)
else
	echo "Cannot find ClockEnableList.log"
	exit 1
fi
if [ a$NbClkEnable == a ]; then
	echo "Cannot find clock"
	exit 2
else
	echo "Nb clock enable found: $NbClkEnable"
	while read line
	do
		clockName=$(echo $line | grep ^[0-9][0-9]* | cut -d" " -f 4)
		if [ a$clockName != a ]; then
			echo "Disable clock $clockName"
			echo 0 > /sys/kernel/debug/clk/$clockName/enable
		fi
	done < $CLK_EN_LOG_FILE 
	rm $CLK_EN_LOG_FILE
	echo -e "\n****************************** CLK  DISABLE *********************************"
fi

sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/BaseBand_registers/DVFS_Enable.txt


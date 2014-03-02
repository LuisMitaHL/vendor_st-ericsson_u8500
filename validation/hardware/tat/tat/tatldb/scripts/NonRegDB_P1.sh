#!/bin/sh

#Enable all clocks
/usr/bin/HATS_validation/BaseBand_registers/Clocks_Enable.sh

#Run non reg DB tests
/usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/scripts/hats_services/NonRegDB_P1.txt

#Disable all clocks
/usr/bin/HATS_validation/BaseBand_registers/Clocks_Init.sh

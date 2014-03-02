#!/bin/sh
# Author: Guillaume Camus


while [ 1 ]
do
echo " "
echo -e '*****************************************************************************************'
echo -e '*****************************************************************************************'
echo -e '***                                                                                   ***'
echo -e '***                             \033[1;34mHATS Validation Tests Menu\033[0m                            ***'
echo -e '***                            \033[1;34m----------------------------\033[0m                           ***'
echo -e '***                                                                                   ***'
echo -e '***                                                                                   ***'
echo -e '***     DISPLAY:            COEXISTENCE:                   SIM:                       ***'
echo -e '***     1) LCD              13) Agressors Part1            27) SMC                    ***'
echo -e '***                         14) Agressors Part2                                       ***'
echo -e '***     3) TVout AV8100     15) Victims Part1              MINI APE:                  ***'
echo -e '***     4) HDMI             16) Victims Part2              28) Cs call 2G/3G (*)      ***'
echo -e '***                                                                                   ***'
echo -e '***     REGISTERS:          POWER:                                                    ***'
echo -e '***     5) HDMI             17) Mode Management                                       ***'
echo -e '***     6) BaseBand         18) DVFS Management                                       ***'
echo -e '***     7) PMU              19) Reset AP                                              ***'
echo -e '***                         20) Reset Modem 1/2                                       ***'
echo -e '***     RADIO:              21) Reset Modem 2/2                                       ***'
echo -e '***     8) RF (*)                                                                     ***'
echo -e '***                         CAMERA:                                                   ***'
echo -e '***     ADC:                22) Picture Primary                                       ***'
echo -e '***     9) ADC Part1        23) Picture Secondary                                     ***'
echo -e '***     10) ADC Part2       24) Video Primary 320x240      COMMANDES:                 ***'
echo -e '***     11) Gas Gauge       25) Video Primary 640x480      97) (*) Information        ***'
echo -e '***                         26) Video Secondary 320x240    98) Last Test Result Log   ***'
echo -e '***     CHARGE:                                            99) Reboot Mobile          ***'
echo -e '***     12) Charge                                          0) Exit                   ***'
echo -e '***                                                                                   ***'
echo -e '***                                                                                   ***'
echo -e '*****************************************************************************************'
echo -e '*****************************************************************************************'
echo "Make your selection:"
read MenResp

	case "$MenResp" in
		1 ) /usr/bin/HATS_validation/Display/lcd;;
		3 ) /usr/bin/HATS_validation/Display/av8100tvout;;
		4 ) /usr/bin/HATS_validation/Display/hdmi;;

		5 ) /usr/bin/HATS_validation/HDMI_registers/HDMI_Registers.sh; PathLog=/var/log/HDMI_registers.log;;
		6 ) /usr/bin/HATS_validation/BaseBand_registers/baseband_registers.sh; PathLog=/var/log/BB_registers.log;;
		7 ) /usr/bin/HATS_validation/PMU_registers/PMU_Registers.sh; PathLog=/var/log/PMU_registers.log;;

		8 ) /usr/bin/HATS_validation/Radio/rf_test.sh; PathLog=/var/log/RF.log;;

		9 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/ADC/ADC_Part1.txt; PathLog=/var/log/ADC_Part1.log;;
		10 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/ADC/ADC_Part2.txt; PathLog=/var/log/ADC_Part2.log;;
		11 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/ADC/Gas_Gauge.txt; PathLog=/var/log/Gas_Gauge.log;;

		12 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/Charge/Charge.txt; PathLog=/var/log/Charge.log;;

		13 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/COEX/Agressor_P1-CG2900.txt; PathLog=/var/log/Agressor_P1-CG2900.log;;
		14 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/COEX/Agressor_P2-CW1200.txt; PathLog=/var/log/Agressor_P2-CW1200.log;;
		15 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/COEX/Victims_P1-CG2900.txt; PathLog=/var/log/Victims_P1-CG2900.log;;
		16 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/COEX/Victims_P2-CW1200.txt; PathLog=/var/log/Victims_P2-CW1200.log;;

		17 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/POWER/Power_ModeManagement.txt; PathLog=/var/log/Power_ModeManagement.log;;
		18 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/POWER/Power_DVFSManagement.txt; PathLog=/var/log/Power_DVFSManagement.log;;
		19 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/POWER/Power_ResetAP.txt; PathLog=/var/log/Power_ResetAP.log;;
		20 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/POWER/Power_ResetModem1-2.txt; PathLog=/var/log/Power_ResetModem1-2.log;;
		21 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/POWER/Power_ResetModem2-2.txt; PathLog=/var/log/Power_ResetModem2-2.log;;

	  22 ) /usr/bin/HATS_validation/Camera/PicturePrimaryCamera.sh; PathLog=/var/log/Primary_Camera.log;;
	  23 ) /usr/bin/HATS_validation/Camera/PictureSecondaryCamera.sh; PathLog=/var/log/Secondary_Camera.log;;
	  24 ) /usr/bin/HATS_validation/Camera/VideoPrimaryCamera320-240.sh; PathLog=/var/log/Video_320-240_Primary_Camera.log;;
	  25 ) /usr/bin/HATS_validation/Camera/VideoPrimaryCamera640-480.sh; PathLog=/var/log/Video_640-480_Primary_Camera.log;;
	  26 ) /usr/bin/HATS_validation/Camera/VideoSecondaryCamera320-240.sh; PathLog=/var/log/Video_320-240_Secondary_Camera.log;;

		27 ) sh /usr/bin/scripts/DTHScriptInterpreter.sh /usr/bin/HATS_validation/SIM/SIM.txt; PathLog=/var/log/SIM.log;;

		28 ) /usr/bin/HATS_validation/APE/APE.sh; PathLog=/var/log/CScall.log;;

		97 ) echo -e '\033[1;31m\nINFORMATION:\nRF and APE tests needs "mob_conf.txt" file\nThis file contain RF Board data and phone number for APE call\nThis file must be in SDcard like this: SDCard/data_hats_auto/mobconfig/mob_conf.txt\nTo get "mob_conf.txt" please contact System Test Team: systemtest.support@stericsson.com\n\033[0m';;
		98 ) cat $PathLog;;
		99 ) reboot;;
		0 ) exit;;
		* ) echo "your input is incorrect";;
	esac

done


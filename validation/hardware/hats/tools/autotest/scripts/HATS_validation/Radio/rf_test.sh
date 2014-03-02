#echo start modified
#!/bin/sh
CONF_FILE=/mob_conf.txt
LOGFILE=/var/log/RF.log
if [ -f $LOGFILE ]; then
  rm $LOGFILE
fi
echo "|*******************************************************************************|" | tee -a $LOGFILE
echo "|********************************* RF Auto Tests *******************************|" | tee -a $LOGFILE
echo "|************************************ Results **********************************|" | tee -a $LOGFILE
echo "|*******************************************************************************|" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE

mount /mnt/SDCard
if [ $? -ne 0 ]; then
  echo "| Mount SD :........................................ FAILED |" | tee -a $LOGFILE
  exit
fi

if [ -f /mnt/SDCard/data_hats_auto/mobconfig$CONF_FILE ]; then
  cp /mnt/SDCard/data_hats_auto/mobconfig$CONF_FILE /
  umount /mnt/SDCard
else
  echo "Could not find $CONF_FILE !"
  echo "| Check $CONF_FILE presence :........................................ FAILED |" | tee -a $LOGFILE
  umount /mnt/SDCard
  exit
fi
cd /


MobName=$(grep "^MobName=" $CONF_FILE| cut -d"=" -f 2)
GSM850int=$(grep "^GSM850int=" $CONF_FILE| cut -d"=" -f 2)
GSM850ext=$(grep "^GSM850ext=" $CONF_FILE| cut -d"=" -f 2)
GSM900int=$(grep "^GSM900int=" $CONF_FILE| cut -d"=" -f 2)
GSM900ext=$(grep "^GSM900ext=" $CONF_FILE| cut -d"=" -f 2)
GSM1800int=$(grep "^GSM1800int=" $CONF_FILE| cut -d"=" -f 2)
GSM1800ext=$(grep "^GSM1800ext=" $CONF_FILE| cut -d"=" -f 2)
GSM1900int=$(grep "^GSM1900int=" $CONF_FILE| cut -d"=" -f 2)
GSM1900ext=$(grep "^GSM1900ext=" $CONF_FILE| cut -d"=" -f 2)
WCDMAIint=$(grep "^WCDMAIint=" $CONF_FILE| cut -d"=" -f 2)
WCDMAIext=$(grep "^WCDMAIext=" $CONF_FILE| cut -d"=" -f 2)
WCDMAIIint=$(grep "^WCDMAIIint=" $CONF_FILE| cut -d"=" -f 2)
WCDMAIIext=$(grep "^WCDMAIIext=" $CONF_FILE| cut -d"=" -f 2)
WCDMAIVint=$(grep "^WCDMAIVint=" $CONF_FILE| cut -d"=" -f 2)
WCDMAIVext=$(grep "^WCDMAIVext=" $CONF_FILE| cut -d"=" -f 2)
WCDMAVint=$(grep "^WCDMAVint=" $CONF_FILE| cut -d"=" -f 2)
WCDMAVext=$(grep "^WCDMAVext=" $CONF_FILE| cut -d"=" -f 2)
WCDMAVIIIint=$(grep "^WCDMAVIIIint=" $CONF_FILE| cut -d"=" -f 2)
WCDMAVIIIext=$(grep "^WCDMAVIIIext=" $CONF_FILE| cut -d"=" -f 2)


echo ""
echo "******************* parameters ******************"
echo "****** Board $MobName *****"
echo ""

echo ""
echo "GSM Band 850    => Interne: $GSM850int"
echo "                => Externe: $GSM850ext"
echo "GSM Band 900    => Interne: $GSM900int"
echo "                => Externe: $GSM900ext"
echo "GSM Band 1800   => Interne: $GSM1800int"
echo "                => Externe: $GSM1800ext"
echo "GSM Band 1900   => Interne: $GSM1900int"
echo "                => Externe: $GSM1900ext"
echo "WCDMA band I    => Interne: $WCDMAIint"
echo "                => Externe: $WCDMAIext"
echo "WCDMA band II   => Interne: $WCDMAIIint"
echo "                => Externe: $WCDMAIIext"
echo "WCDMA band IV   => Interne: $WCDMAIVint"
echo "                => Externe: $WCDMAIVext"
echo "WCDMA band V    => Interne: $WCDMAVint"
echo "                => Externe: $WCDMAVext"
echo "WCDMA band VIII => Interne: $WCDMAVIIIint"
echo "                => Externe: $WCDMAVIIIext"
echo ""

echo ""
echo "--------------- Selftest RFBUS"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTest/RFBus/RFBus_Start
cat $ROOT_DTH/value
cat $ROOT_DTH/Out_status/value | dthfilter -r u8
echo ""
ERROR=$(cat $ROOT_DTH/Out_error_code/value | dthfilter -r u16)
if [ $ERROR == 0 ]
	then RFBUS="***PASSED***"
	else RFBUS="!!!FAILED!!!"
fi
echo $RFBUS

echo ""
echo "--------------- Selftest STROBE"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTest/Strobe/Strobe_Start
cat $ROOT_DTH/value
cat $ROOT_DTH/Out_status/value | dthfilter -r u8
echo ""
ERROR=$(cat $ROOT_DTH/Out_error_code/value | dthfilter -r u16)
if [ $ERROR == 0 ]
	then STROBE="***PASSED***"
	else STROBE="!!!FAILED!!!"
fi
echo $STROBE

echo ""
echo "--------------- Selftest RF Supply"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTest/RF_Supply/RF_Supply_Start
cat $ROOT_DTH/value
cat $ROOT_DTH/Out_status/value | dthfilter -r u8
echo ""
ERROR1=$(cat $ROOT_DTH/Out_STw3530_error_code/value | dthfilter -r u16)
echo ""
ERROR2=$(cat $ROOT_DTH/Out_STw3500_error_code/value | dthfilter -r u16)
echo ""
cat $ROOT_DTH/Out_regulators/value | dthfilter -r u16
if [ $ERROR1 == 0 ]
	then
	if [ $ERROR2 == 0 ]
		then Supply="***PASSED***"
		else Supply="!!!FAILED!!!"
	fi
	else Supply="!!!FAILED!!!"
fi
echo ""
echo $Supply

echo ""
echo "--------------- Selftest Digital RX TX"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTest/DigitalRXTX/DigitalRXTX_Start
cat $ROOT_DTH/value
cat $ROOT_DTH/Out_status/value | dthfilter -r u8
echo ""
ERROR=$(cat $ROOT_DTH/Out_error_code/value | dthfilter -r u16)
if [ $ERROR == 0 ]
	then DigitRXTX="***PASSED***"
	else DigitRXTX="!!!FAILED!!!"
fi
echo $DigitRXTX

echo ""
echo "--------------- Selftest PA id & Pin"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTest/PA_ID_PIN/PA_ID_PIN_Start
cat $ROOT_DTH/value
cat $ROOT_DTH/Out_status/value | dthfilter -r u8
echo ""
ERROR=$(cat $ROOT_DTH/Out_error_code/value | dthfilter -r u16)
echo ""
cat $ROOT_DTH/Out_voltage/value | dthfilter -r u16
if [ $ERROR == 0 ]
	then PAIDPIN="***PASSED***"
	else PAIDPIN="!!!FAILED!!!"
fi
echo ""
echo $PAIDPIN

echo ""
echo "--------------- Selftest WCDMA TX"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTest/WCDMA_TX/WCDMA_TX_Start
cat $ROOT_DTH/value
cat $ROOT_DTH/Out_status/value | dthfilter -r u8
echo ""
ERROR1=$(cat $ROOT_DTH/Out_low_band_error_code/value | dthfilter -r u16)
echo ""
cat $ROOT_DTH/Out_low_band_power_value/value | dthfilter -r u16
echo ""
ERROR2=$(cat $ROOT_DTH/Out_high_band_error_code/value | dthfilter -r u16)
echo ""
cat $ROOT_DTH/Out_high_band_power_value/value | dthfilter -r u16
if [ $ERROR1 == 0 ]
	then
	if [ $ERROR2 == 0 ]
		then WCDMATX="***PASSED***"
		else WCDMATX="!!!FAILED!!!"
	fi
	else WCDMATX="!!!FAILED!!!"
fi
echo ""
echo $WCDMATX


echo ""
echo "--------------- Selftest Antenna Low Band"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTest/Antenna/Antenna_Start
echo 0 | dthfilter -w u16 > $ROOT_DTH/In_Sys_Info/value
echo 5 | dthfilter -w u32 > $ROOT_DTH/In_Rf_Band/value
echo 4182 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo 20 | dthfilter -w float > $ROOT_DTH/In_Power_Level/value
cat $ROOT_DTH/value
cat $ROOT_DTH/Out_status/value | dthfilter -r u8
echo ""
ERROR=$(cat $ROOT_DTH/Out_Error_code/value | dthfilter -r u16)
echo ""
cat $ROOT_DTH/Out_VSWR/value | dthfilter -r float
if [ $ERROR == 0 ]
	then AntennaL="***PASSED***"
	else AntennaL="!!!FAILED!!!"
fi
echo ""
echo $AntennaL

echo ""
echo "--------------- Selftest Antenna High Band"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTest/Antenna/Antenna_Start
echo 0 | dthfilter -w u16 > $ROOT_DTH/In_Sys_Info/value
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_Rf_Band/value
echo 9750 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo 20 | dthfilter -w float > $ROOT_DTH/In_Power_Level/value
cat $ROOT_DTH/value
cat $ROOT_DTH/Out_status/value | dthfilter -r u8
echo ""
ERROR=$(cat $ROOT_DTH/Out_Error_code/value | dthfilter -r u16)
echo ""
cat $ROOT_DTH/Out_VSWR/value | dthfilter -r float
if [ $ERROR == 0 ]
	then AntennaH="***PASSED***"
	else AntennaH="!!!FAILED!!!"
fi
echo ""
echo $AntennaH


echo ""
echo "--------------- Tuning TX WCDMA BAND IV"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/TX/Start
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 4 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_AmountOfData/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_EmptySlotNumber/value
echo "1312;1362;1412;1412;1462;1513" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
echo "24;24;24;0;24;24" | dthfilter -w s16 > $ROOT_DTH/In_Levels/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Measures/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then TunTxBIV="***PASSED***"
	else TunTxBIV="!!!FAILED!!!"
fi
echo ""
echo $TunTxBIV

echo ""
echo "--------------- Tuning CAL WCDMA BAND IV"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/CAL/Start
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 4 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_AmountOfData/value
echo "1312;1362;1412;1412;1462;1513" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
if [ -f $CONF_FILE ]; then
  echo "$WCDMAIVint" | dthfilter -w s16 > $ROOT_DTH/In_IntMeasures/value #Values to get from CSV file
  echo "$WCDMAIVext" | dthfilter -w float > $ROOT_DTH/In_ExtMeasures/value #Values to get from CSV file
fi
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Slope/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffB/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffC/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespChannel/value | dthfilter -r u16
echo ""
cat $ROOT_DTH/Out_RespDetector/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespTx/value | dthfilter -r float
if [ $ERROR == 0 ]
	then CALBIV="***PASSED***"
	else CALBIV="!!!FAILED!!!"
fi
echo ""
echo $CALBIV

echo ""
echo "--------------- Tuning TX WCDMA BAND II"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/TX/Start
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 2 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_AmountOfData/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_EmptySlotNumber/value
echo "9262;9331;9400;9400;9469;9538" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
echo "24;24;24;0;24;24" | dthfilter -w s16 > $ROOT_DTH/In_Levels/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Measures/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then TunTxBII="***PASSED***"
	else TunTxBII="!!!FAILED!!!"
fi
echo ""
echo $TunTxBII

echo ""
echo "--------------- Tuning CAL WCDMA BAND II"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/CAL/Start
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 2 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_AmountOfData/value
echo "9262;9331;9400;9400;9469;9538" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
if [ -f $CONF_FILE ]; then
  echo "$WCDMAIIint" | dthfilter -w s16 > $ROOT_DTH/In_IntMeasures/value #Values to get from CSV file
  echo "$WCDMAIIext" | dthfilter -w float > $ROOT_DTH/In_ExtMeasures/value #Values to get from CSV file
fi
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Slope/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffB/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffC/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespChannel/value | dthfilter -r u16
echo ""
cat $ROOT_DTH/Out_RespDetector/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespTx/value | dthfilter -r float
if [ $ERROR == 0 ]
	then CALBII="***PASSED***"
	else CALBII="!!!FAILED!!!"
fi
echo ""
echo $CALBII

echo ""
echo "--------------- Tuning TX WCDMA BAND I"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/TX/Start
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_AmountOfData/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_EmptySlotNumber/value
echo "9612;9681;9750;9750;9819;9888" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
echo "24;24;24;0;24;24" | dthfilter -w s16 > $ROOT_DTH/In_Levels/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Measures/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then TunTxBI="***PASSED***"
	else TunTxBI="!!!FAILED!!!"
fi
echo ""
echo $TunTxBI

echo ""
echo "--------------- Tuning CAL WCDMA BAND I"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/CAL/Start
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_AmountOfData/value
echo "9612;9681;9750;9750;9819;9888" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
if [ -f $CONF_FILE ]; then
  echo "$WCDMAIint" | dthfilter -w s16 > $ROOT_DTH/In_IntMeasures/value #Values to get from CSV file
  echo "$WCDMAIext" | dthfilter -w float > $ROOT_DTH/In_ExtMeasures/value #Values to get from CSV file
fi
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Slope/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffB/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffC/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespChannel/value | dthfilter -r u16
echo ""
cat $ROOT_DTH/Out_RespDetector/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespTx/value | dthfilter -r float
if [ $ERROR == 0 ]
	then CALBI="***PASSED***"
	else CALBI="!!!FAILED!!!"
fi
echo ""
echo $CALBI


echo ""
echo "--------------- Tuning TX WCDMA BAND VIII"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/TX/Start
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 8 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_AmountOfData/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_EmptySlotNumber/value
echo "2712;2752;2787;2787;2825;2863" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
echo "24;24;24;0;24;24" | dthfilter -w s16 > $ROOT_DTH/In_Levels/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Measures/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then TunTxBVIII="***PASSED***"
	else TunTxBVIII="!!!FAILED!!!"
fi
echo ""
echo $TunTxBVIII

echo ""
echo "--------------- Tuning CAL WCDMA BAND VIII"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/CAL/Start
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 8 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 1 | dthfilter -w u8> $ROOT_DTH/In_AmountOfData/value
echo "2712;2752;2787;2787;2825;2863" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
if [ -f $CONF_FILE ]; then
  echo "$WCDMAVIIIint" | dthfilter -w s16 > $ROOT_DTH/In_IntMeasures/value #Values to get from CSV file
  echo "$WCDMAVIIIext" | dthfilter -w float > $ROOT_DTH/In_ExtMeasures/value #Values to get from CSV file
fi
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Slope/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffB/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffC/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespChannel/value | dthfilter -r u16
echo ""
cat $ROOT_DTH/Out_RespDetector/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespTx/value | dthfilter -r float
if [ $ERROR == 0 ]
	then CALBVIII="***PASSED***"
	else CALBVIII="!!!FAILED!!!"
fi
echo ""
echo $CALBVIII

echo ""
echo "--------------- Tuning TX WCDMA BAND V"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/TX/Start
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 5 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_AmountOfData/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_EmptySlotNumber/value
echo "4132;4157;4182;4182;4207;4233" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
echo "24;24;24;0;24;24" | dthfilter -w s16 > $ROOT_DTH/In_Levels/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Measures/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then TunTxBV="***PASSED***"
	else TunTxBV="!!!FAILED!!!"
fi
echo ""
echo $TunTxBV

echo ""
echo "--------------- Tuning CAL WCDMA BAND V"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/CAL/Start
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 5 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_AmountOfData/value
echo "4132;4157;4182;4182;4207;4233" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
if [ -f $CONF_FILE ]; then
  echo "$WCDMAVint" | dthfilter -w s16 > $ROOT_DTH/In_IntMeasures/value #Values to get from CSV file
  echo "$WCDMAVext" | dthfilter -w float > $ROOT_DTH/In_ExtMeasures/value #Values to get from CSV file
fi
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Slope/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffB/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffC/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespChannel/value | dthfilter -r u16
echo ""
cat $ROOT_DTH/Out_RespDetector/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespTx/value | dthfilter -r float
if [ $ERROR == 0 ]
	then CALBV="***PASSED***"
	else CALBV="!!!FAILED!!!"
fi
echo ""
echo $CALBV

echo ""
echo "--------------- SelfTuning WCDMA BAND IV"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/WCDMA/WCDMA_TX
echo  1412 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  4 | dthfilter -w u32 > $ROOT_DTH/In_Band/value
echo  24 | dthfilter -w float > $ROOT_DTH/In_Tx_Target_Pwr_Start/value
echo  -2 | dthfilter -w s16 > $ROOT_DTH/In_Pwr_Ctrl_Step/value
echo  32 | dthfilter -w u16 > $ROOT_DTH/In_Amount_Ctrl_Data/value
echo  2 | dthfilter -w u16 > $ROOT_DTH/In_Tuning_Options/value
echo  66 | dthfilter -w u16 > $ROOT_DTH/In_Step_Duration/value
echo  320 | dthfilter -w u16 > $ROOT_DTH/In_Pwr_Ctrl_Start_Value/value
echo "2634;2340;2370;2119;1868;1784;1492;1324;1157;1074;990;864;781;781;655;614;572;530;488;488;488;404;404;363;363;363;321;321;321;279;279;279" | dthfilter -w s16 > $ROOT_DTH/In_Pa_Vcc/value
echo "14;12;11;10;9;9;8;7;7;7;7;6;6;6;6;6;6;6;6;5;5;5;5;5;5;5;5;5;5;5;5;5" | dthfilter -w s16 > $ROOT_DTH/In_Pa_Bias/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value| dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Rfic_Tx_agc_index/value | dthfilter -r float
if [ $ERROR == 0 ]
	then SelfTIV="***PASSED***"
	else SelfTIV="!!!FAILED!!!"
fi
echo ""
echo $SelfTIV

echo ""
echo "--------------- SelfTuning WCDMA BAND II"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/WCDMA/WCDMA_TX
echo  9400 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  2 | dthfilter -w u32 > $ROOT_DTH/In_Band/value
echo  24 | dthfilter -w float > $ROOT_DTH/In_Tx_Target_Pwr_Start/value
echo  -2 | dthfilter -w s16 > $ROOT_DTH/In_Pwr_Ctrl_Step/value
echo  32 | dthfilter -w u16 > $ROOT_DTH/In_Amount_Ctrl_Data/value
echo  2 | dthfilter -w u16 > $ROOT_DTH/In_Tuning_Options/value
echo  66 | dthfilter -w u16 > $ROOT_DTH/In_Step_Duration/value 
echo  320 | dthfilter -w u16 > $ROOT_DTH/In_Pwr_Ctrl_Start_Value/value
echo "2942;2633;2373;2113;1940;1680;1593;1420;1247;1117;1030;900;814;814;684;684;597;554;511;511;467;424;424;381;337;337;337;294;294;294;294;294" | dthfilter -w s16 > $ROOT_DTH/In_Pa_Vcc/value
echo "14;12;11;10;9;8;7;7;6;6;6;6;6;6;6;6;6;6;6;6;5;5;5;5;5;5;5;5;5;5;5;5" | dthfilter -w s16 > $ROOT_DTH/In_Pa_Bias/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value| dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Rfic_Tx_agc_index/value | dthfilter -r float
if [ $ERROR == 0 ]
	then SelfTII="***PASSED***"
	else SelfTII="!!!FAILED!!!"
fi
echo ""
echo $SelfTII

echo ""
echo "--------------- SelfTuning WCDMA BAND I"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/WCDMA/WCDMA_TX
echo 9750 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  1 | dthfilter -w u32 > $ROOT_DTH/In_Band/value
echo  24 | dthfilter -w float > $ROOT_DTH/In_Tx_Target_Pwr_Start/value
echo  -2 | dthfilter -w s16 > $ROOT_DTH/In_Pwr_Ctrl_Step/value
echo  32 | dthfilter -w u16 > $ROOT_DTH/In_Amount_Ctrl_Data/value
echo  2 | dthfilter -w u16 > $ROOT_DTH/In_Tuning_Options/value
echo  66 | dthfilter -w u16 > $ROOT_DTH/In_Step_Duration/value
echo  320| dthfilter -w u16 > $ROOT_DTH/In_Pwr_Ctrl_Start_Value/value
echo "2942;2633;2373;2113;1940;1680;1593;1420;1247;1117;1030;900;814;814;684;684;597;554;511;511;467;424;424;381;337;337;337;294;294;294;294;294" | dthfilter -w s16 > $ROOT_DTH/In_Pa_Vcc/value
echo "14;12;11;10;9;8;7;7;6;6;6;6;6;6;6;6;6;6;6;6;5;5;5;5;5;5;5;5;5;5;5;5" | dthfilter -w s16 > $ROOT_DTH/In_Pa_Bias/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value| dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Rfic_Tx_agc_index/value | dthfilter -r float
if [ $ERROR == 0 ]
	then SelfTI="***PASSED***"
	else SelfTI="!!!FAILED!!!"
fi
echo ""
echo $SelfTI

echo ""
echo "--------------- SelfTuning WCDMA BAND VIII"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/WCDMA/WCDMA_TX
echo  2787 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  8 | dthfilter -w u32 > $ROOT_DTH/In_Band/value
echo  24 | dthfilter -w float > $ROOT_DTH/In_Tx_Target_Pwr_Start/value
echo  -2 | dthfilter -w s16 > $ROOT_DTH/In_Pwr_Ctrl_Step/value
echo  32 | dthfilter -w u16 > $ROOT_DTH/In_Amount_Ctrl_Data/value
echo  2 | dthfilter -w u16 > $ROOT_DTH/In_Tuning_Options/value
echo  66 | dthfilter -w u16 > $ROOT_DTH/In_Step_Duration/value
echo  320 | dthfilter -w u16 > $ROOT_DTH/In_Pwr_Ctrl_Start_Value/value
echo "2678;2210;1886;1805;1643;1481;1359;1197;1076;995;914;833;752;711;671;630;590;590;468;428;408;367;347;327;306;286;266;266;266;266;266;266" | dthfilter -w s16 > $ROOT_DTH/In_Pa_Vcc/value
echo "13;12;12;12;11;11;10;10;9;9;8;8;8;7;7;7;7;6;6;6;6;6;6;6;6;6;6;6;6;6;6;6" | dthfilter -w s16 > $ROOT_DTH/In_Pa_Bias/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value| dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Rfic_Tx_agc_index/value | dthfilter -r float
if [ $ERROR == 0 ]
	then SelfTVIII="***PASSED***"
	else SelfTVIII="!!!FAILED!!!"
fi
echo ""
echo $SelfTIII


echo ""
echo "--------------- SelfTuning WCDMA BAND V"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/WCDMA/WCDMA_TX
echo  4182 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  5 | dthfilter -w u32 > $ROOT_DTH/In_Band/value
echo  24 | dthfilter -w float > $ROOT_DTH/In_Tx_Target_Pwr_Start/value
echo  -2 | dthfilter -w s16 > $ROOT_DTH/In_Pwr_Ctrl_Step/value
echo  32 | dthfilter -w u16 > $ROOT_DTH/In_Amount_Ctrl_Data/value
echo  2 | dthfilter -w u16 > $ROOT_DTH/In_Tuning_Options/value
echo  66 | dthfilter -w u16 > $ROOT_DTH/In_Step_Duration/value
echo  320 | dthfilter -w u16 > $ROOT_DTH/In_Pwr_Ctrl_Start_Value/value
echo "2368;2056;1820;1741;1585;1428;1310;1154;1036;958;879;801;723;684;644;605;566;566;449;409;390;351;331;311;292;272;253;253;253;253;253;253" | dthfilter -w s16 > $ROOT_DTH/In_Pa_Vcc/value
echo "13;11;11;11;10;10;9;9;8;8;7;7;7;7;7;7;7;6;6;6;6;6;6;6;6;6;6;6;6;6;6;6" | dthfilter -w s16 > $ROOT_DTH/In_Pa_Bias/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value| dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Rfic_Tx_agc_index/value | dthfilter -r float
if [ $ERROR == 0 ]
	then SelfTV="***PASSED***"
	else SelfTV="!!!FAILED!!!"
fi
echo ""
echo $SelfTV

echo ""
echo "--------------- Tuning TX GMSK BAND 850"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/TX/Start
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_AmountOfData/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_EmptySlotNumber/value
echo "190;190;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
echo "6;19;0;0;0;0" | dthfilter -w s16 > $ROOT_DTH/In_Levels/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Measures/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then TunTx850="***PASSED***"
	else TunTx850="!!!FAILED!!!"
fi
echo ""
echo $TunTx850

echo ""
echo "--------------- Tuning CAL GMSK BAND 850"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/CAL/Start
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 0 | dthfilter -w u8 > $ROOT_DTH/In_AmountOfData/value
echo "190;190;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
if [ -f $CONF_FILE ]; then
  echo "$GSM850int" | dthfilter -w s16 > $ROOT_DTH/In_IntMeasures/value #Values to get from CSV file
  echo "$GSM850ext" | dthfilter -w float > $ROOT_DTH/In_ExtMeasures/value #Values to get from CSV file
fi
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Slope/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffB/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffC/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespChannel/value | dthfilter -r u16
echo ""
cat $ROOT_DTH/Out_RespDetector/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespTx/value | dthfilter -r float
if [ $ERROR == 0 ]
	then CAL850="***PASSED***"
	else CAL850="!!!FAILED!!!"
fi
echo ""
echo $CAL850

echo ""
echo "--------------- Tuning TX GMSK BAND 900"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/TX/Start
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 2 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_AmountOfData/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_EmptySlotNumber/value
echo "37;37;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
echo "6;19;0;0;0;0" | dthfilter -w s16 > $ROOT_DTH/In_Levels/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Measures/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then TunTx900="***PASSED***"
	else TunTx900="!!!FAILED!!!"
fi
echo ""
echo $TunTx900

echo ""
echo "--------------- Tuning CAL GMSK BAND 900"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/CAL/Start
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 2 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 0 | dthfilter -w u8 > $ROOT_DTH/In_AmountOfData/value
echo "37;37;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
if [ -f $CONF_FILE ]; then
  echo "$GSM900int" | dthfilter -w s16 > $ROOT_DTH/In_IntMeasures/value #Values to get from CSV file
  echo "$GSM900ext" | dthfilter -w float > $ROOT_DTH/In_ExtMeasures/value #Values to get from CSV file
fi
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Slope/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffB/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffC/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespChannel/value | dthfilter -r u16
echo ""
cat $ROOT_DTH/Out_RespDetector/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespTx/value | dthfilter -r float
if [ $ERROR == 0 ]
	then CAL900="***PASSED***"
	else CAL900="!!!FAILED!!!"
fi
echo ""
echo $CAL900

echo ""
echo "--------------- Tuning TX GMSK BAND 1800"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/TX/Start
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 3 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_AmountOfData/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_EmptySlotNumber/value
echo "698;698;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
echo "1;15;0;0;0;0" | dthfilter -w s16 > $ROOT_DTH/In_Levels/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Measures/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then TunTx1800="***PASSED***"
	else TunTx1800="!!!FAILED!!!"
fi
echo ""
echo $TunTx1800

echo ""
echo "--------------- Tuning CAL GMSK BAND 1800"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/CAL/Start
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 3 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 0 | dthfilter -w u8 > $ROOT_DTH/In_AmountOfData/value
echo "698;698;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
if [ -f $CONF_FILE ]; then
  echo "$GSM1800int" | dthfilter -w s16 > $ROOT_DTH/In_IntMeasures/value #Values to get from CSV file
  echo "$GSM1800ext" | dthfilter -w float > $ROOT_DTH/In_ExtMeasures/value #Values to get from CSV file
fi
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Slope/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffB/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffC/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespChannel/value | dthfilter -r u16
echo ""
cat $ROOT_DTH/Out_RespDetector/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespTx/value | dthfilter -r float
if [ $ERROR == 0 ]
	then CAL1800="***PASSED***"
	else CAL1800="!!!FAILED!!!"
fi
echo ""
echo $CAL1800

echo ""
echo "--------------- Tuning TX GMSK BAND 1900"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/TX/Start
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 4 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_AmountOfData/value
echo 1 | dthfilter -w u8 > $ROOT_DTH/In_EmptySlotNumber/value
echo "661;661;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
echo "1;15;0;0;0;0" | dthfilter -w s16 > $ROOT_DTH/In_Levels/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Measures/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then TunTx1900="***PASSED***"
	else TunTx1900="!!!FAILED!!!"
fi
echo ""
echo $TunTx1900

echo ""
echo "--------------- Tuning CAL GMSK BAND 1900"
ROOT_DTH=/mnt/9p/DTH/RF/Tuning/Power/CAL/Start
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
echo 4 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
echo 0 | dthfilter -w u8 > $ROOT_DTH/In_AmountOfData/value
echo "661;661;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
if [ -f $CONF_FILE ]; then
  echo "$GSM1900int" | dthfilter -w s16 > $ROOT_DTH/In_IntMeasures/value #Values to get from CSV file
  echo "$GSM1900ext" | dthfilter -w float > $ROOT_DTH/In_ExtMeasures/value #Values to get from CSV file
fi
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_ErrorCode/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Slope/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffB/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_CoeffC/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespChannel/value | dthfilter -r u16
echo ""
cat $ROOT_DTH/Out_RespDetector/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_RespTx/value | dthfilter -r float
if [ $ERROR == 0 ]
	then CAL1900="***PASSED***"
	else CAL1900="!!!FAILED!!!"
fi
echo ""
echo $CAL1900

echo ""
echo "--------------- SelfTuning GMSK BAND 850"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/GMSK/GMSK_TX
echo  190 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  1 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
echo  0 | dthfilter -w u16 > $ROOT_DTH/In_Pa_Data_Valid/value
echo "32.5;31;29;27;25;23;21;19;17;15;13;11;9;7;5;0" | dthfilter -w float > $ROOT_DTH/In_Target_Power/value
echo "0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Vcc/value
echo "0" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Bias/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Pa_Vcc/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then Self850="***PASSED***"
	else Self850="!!!FAILED!!!"
fi
echo ""
echo $Self850

echo ""
echo "--------------- SelfTuning GMSK BAND 900"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/GMSK/GMSK_TX
echo  37 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  2 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
echo  0 | dthfilter -w u16 > $ROOT_DTH/In_Pa_Data_Valid/value
echo "32.5;31;29;27;25;23;21;19;17;15;13;11;9;7;5;0" | dthfilter -w float > $ROOT_DTH/In_Target_Power/value
echo "0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Vcc/value
echo "0" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Bias/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Pa_Vcc/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then Self900="***PASSED***"
	else Self900="!!!FAILED!!!"
fi
echo ""
echo $Self900

echo ""
echo "--------------- SelfTuning GMSK BAND 1800"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/GMSK/GMSK_TX
echo  698 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  3 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
echo  0 | dthfilter -w u16 > $ROOT_DTH/In_Pa_Data_Valid/value
echo "30.5;28;26;24;22;20;18;16;14;12;10;8;6;4;2;0" | dthfilter -w float > $ROOT_DTH/In_Target_Power/value
echo "0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Vcc/value
echo "0" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Bias/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Pa_Vcc/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then Self1800="***PASSED***"
	else Self1800="!!!FAILED!!!"
fi
echo ""
echo $Self1800

echo ""
echo "--------------- SelfTuning GMSK BAND 1900"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/GMSK/GMSK_TX
echo  661 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  4 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
echo  0 | dthfilter -w u16 > $ROOT_DTH/In_Pa_Data_Valid/value
echo "30.5;28;26;24;22;20;18;16;14;12;10;8;6;4;2;0" | dthfilter -w float > $ROOT_DTH/In_Target_Power/value
echo "0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Vcc/value
echo "0" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Bias/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Pa_Vcc/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then Self1900="***PASSED***"
	else Self1900="!!!FAILED!!!"
fi
echo ""
echo $Self1900

echo ""
echo "--------------- SelfTuning IQ 8PSK BAND 850"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/GSM_TX/GSM_TX_IQ
echo 190 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo 1 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Modulation_type/value
echo 0 | dthfilter -w u16 > $ROOT_DTH/In_Operation_Mode/value
echo 8 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Power_Level/value
echo 0 | dthfilter -w s16 > $ROOT_DTH/In_I_Dc_Offset/value
echo 0 | dthfilter -w s16 > $ROOT_DTH/In_Q_Dc_Offset/value
echo 0 | dthfilter -w s16 > $ROOT_DTH/In_Amplitude_Offset/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Iq_Dc_Offset_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_I_Branch_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_I_Pwr_Diff/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Q_Branch_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Q_Pwr_Diff/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Phase_Error_Code/value | dthfilter -r u8
echo ""
cat $ROOT_DTH/Out_Iq_Phase_Tuning_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Phase_Best_Tuning_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Ampl_Error_Code/value | dthfilter -r u8
echo ""
cat $ROOT_DTH/Out_Iq_Ampl_Tuning_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Ampl_Best_Tuning_Value/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then SelfIQ850="***PASSED***"
	else SelfIQ850="!!!FAILED!!!"
fi
echo ""
echo $SelfIQ850

echo ""
echo "--------------- SelfTuning IQ 8PSK BAND 900"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/GSM_TX/GSM_TX_IQ
echo 37 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo 2 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Modulation_type/value
echo 0 | dthfilter -w u16 > $ROOT_DTH/In_Operation_Mode/value
echo 8 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Power_Level/value
echo 0 | dthfilter -w s16 > $ROOT_DTH/In_I_Dc_Offset/value
echo 0 | dthfilter -w s16 > $ROOT_DTH/In_Q_Dc_Offset/value
echo 0 | dthfilter -w s16 > $ROOT_DTH/In_Amplitude_Offset/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Iq_Dc_Offset_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_I_Branch_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_I_Pwr_Diff/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Q_Branch_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Q_Pwr_Diff/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Phase_Error_Code/value | dthfilter -r u8
echo ""
cat $ROOT_DTH/Out_Iq_Phase_Tuning_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Phase_Best_Tuning_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Ampl_Error_Code/value | dthfilter -r u8
echo ""
cat $ROOT_DTH/Out_Iq_Ampl_Tuning_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Ampl_Best_Tuning_Value/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then SelfIQ900="***PASSED***"
	else SelfIQ900="!!!FAILED!!!"
fi
echo ""
echo $SelfIQ900

echo ""
echo "--------------- SelfTuning IQ 8PSK BAND 1800"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/GSM_TX/GSM_TX_IQ
echo 698 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo 3 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Modulation_type/value
echo 0 | dthfilter -w u16 > $ROOT_DTH/In_Operation_Mode/value
echo 2 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Power_Level/value
echo 0 | dthfilter -w s16 > $ROOT_DTH/In_I_Dc_Offset/value
echo 0 | dthfilter -w s16 > $ROOT_DTH/In_Q_Dc_Offset/value
echo 0 | dthfilter -w s16 > $ROOT_DTH/In_Amplitude_Offset/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Iq_Dc_Offset_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_I_Branch_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_I_Pwr_Diff/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Q_Branch_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Q_Pwr_Diff/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Phase_Error_Code/value | dthfilter -r u8
echo ""
cat $ROOT_DTH/Out_Iq_Phase_Tuning_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Phase_Best_Tuning_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Ampl_Error_Code/value | dthfilter -r u8
echo ""
cat $ROOT_DTH/Out_Iq_Ampl_Tuning_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Ampl_Best_Tuning_Value/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then SelfIQ1800="***PASSED***"
	else SelfIQ1800="!!!FAILED!!!"
fi
echo ""
echo $SelfIQ1800

echo ""
echo "--------------- SelfTuning IQ 8PSK BAND 1900"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/GSM_TX/GSM_TX_IQ
echo 661 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo 4 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Modulation_type/value
echo 0 | dthfilter -w u16 > $ROOT_DTH/In_Operation_Mode/value
echo 2 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Power_Level/value
echo 0 | dthfilter -w s16 > $ROOT_DTH/In_I_Dc_Offset/value
echo 0 | dthfilter -w s16 > $ROOT_DTH/In_Q_Dc_Offset/value
echo 0 | dthfilter -w s16 > $ROOT_DTH/In_Amplitude_Offset/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Iq_Dc_Offset_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_I_Branch_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_I_Pwr_Diff/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Q_Branch_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Q_Pwr_Diff/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Phase_Error_Code/value | dthfilter -r u8
echo ""
cat $ROOT_DTH/Out_Iq_Phase_Tuning_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Phase_Best_Tuning_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Ampl_Error_Code/value | dthfilter -r u8
echo ""
cat $ROOT_DTH/Out_Iq_Ampl_Tuning_Value/value | dthfilter -r s16
echo ""
cat $ROOT_DTH/Out_Iq_Ampl_Best_Tuning_Value/value | dthfilter -r s16
if [ $ERROR == 0 ]
	then SelfIQ1900="***PASSED***"
	else SelfIQ1900="!!!FAILED!!!"
fi
echo ""
echo $SelfIQ1900

echo ""
echo "--------------- SelfTuning 8PSK BAND 850"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/8PSK/8PSK_TX
echo  190 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  1 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
echo  1 | dthfilter -w u16 > $ROOT_DTH/In_Pa_Data_Valid/value
echo "0;0;0;27;25;23;21;19;17;15;13;11;9;7;5;0" | dthfilter -w float > $ROOT_DTH/In_Target_Power/value
echo "0;0;0;1680;1488;1248;1088;928;832;736;688;640;592;576;544;0" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Vcc/value
echo "50" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Bias/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Rfic_Tx_agc_index/value | dthfilter -r float
if [ $ERROR == 0 ]
	then Self8psk850="***PASSED***"
	else Self8psk850="!!!FAILED!!!"
fi
echo ""
echo $Self8psk850

echo ""
echo "--------------- SelfTuning 8PSK BAND 900"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/8PSK/8PSK_TX
echo  37 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  2 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
echo  1 | dthfilter -w u16 > $ROOT_DTH/In_Pa_Data_Valid/value
echo "0;0;0;27;25;23;21;19;17;15;13;11;9;7;5;0" | dthfilter -w float > $ROOT_DTH/In_Target_Power/value
echo "0;0;0;1680;1488;1248;1088;928;832;736;688;640;592;576;544;0" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Vcc/value
echo "50" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Bias/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Rfic_Tx_agc_index/value | dthfilter -r float
if [ $ERROR == 0 ]
	then Self8psk900="***PASSED***"
	else Self8psk900="!!!FAILED!!!"
fi
echo ""
echo $Self8psk900

echo ""
echo "--------------- SelfTuning 8PSK BAND 1800"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/8PSK/8PSK_TX
echo  698 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  3 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
echo  1 | dthfilter -w u16 > $ROOT_DTH/In_Pa_Data_Valid/value
echo "0;0;26;24;22;20;18;16;14;12;10;8;6;4;2;0" | dthfilter -w float > $ROOT_DTH/In_Target_Power/value
echo "0;0;1680;1584;1376;1200;1024;864;768;688;640;592;576;544;528;512" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Vcc/value
echo "50" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Bias/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Rfic_Tx_agc_index/value | dthfilter -r float
if [ $ERROR == 0 ]
	then Self8psk1800="***PASSED***"
	else Self8psk1800="!!!FAILED!!!"
fi
echo ""
echo $Self8psk1800

echo ""
echo "--------------- SelfTuning 8PSK BAND 1900"
ROOT_DTH=/mnt/9p/DTH/RF/SelfTuning/8PSK/8PSK_TX
echo  661 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
echo  4 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
echo  1 | dthfilter -w u16 > $ROOT_DTH/In_Pa_Data_Valid/value
echo "0;0;26;24;22;20;18;16;14;12;10;8;6;4;2;0" | dthfilter -w float > $ROOT_DTH/In_Target_Power/value
echo "0;0;1680;1584;1376;1200;1024;864;768;688;640;592;576;544;528;512" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Vcc/value
echo "50" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Bias/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value
ERROR=$(cat $ROOT_DTH/Out_Error_Code/value | dthfilter -r u8)
echo ""
cat $ROOT_DTH/Out_Maximum_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Measured_Power/value | dthfilter -r float
echo ""
cat $ROOT_DTH/Out_Rfic_Tx_agc_index/value | dthfilter -r float
if [ $ERROR == 0 ]
	then Self8psk1900="***PASSED***"
	else Self8psk1900="!!!FAILED!!!"
fi
echo ""
echo $Self8psk1900


echo ""
echo "--------------- Start TX WCDMA BAND I"
ROOT_DTH=/mnt/9p/DTH/RF/Tx3G/StartTx3G
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_FreqBand/value
echo 9750 | dthfilter -w u16 > $ROOT_DTH/In_TxUarfcnChannel/value
echo 0 | dthfilter -w u8 > $ROOT_DTH/In_AfcTabSelect/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_AfcValue/value
echo 24 | dthfilter -w float > $ROOT_DTH/In_TxPowerLevel/value
echo 1 | dthfilter -w u32 > $ROOT_DTH/In_DPDCH/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_ACKNACK/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_CQI/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/In_Modulation/value
echo 0 | dthfilter -w u8 > $ROOT_DTH/In_E_DPDCH_number/value
echo 0 | dthfilter -w u8 > $ROOT_DTH/In_E_DPDCH_TTI/value
echo 120 | dthfilter -w u16 > $ROOT_DTH/In_BETA_C/value
echo 225 | dthfilter -w u16 > $ROOT_DTH/In_BETA_D/value
echo 5 | dthfilter -w u16 > $ROOT_DTH/In_BETA_HS/value
echo 5 | dthfilter -w u16 > $ROOT_DTH/In_BETA_EC/value
echo 0 | dthfilter -w u16 > $ROOT_DTH/In_BETA_ED1/value
echo 0 | dthfilter -w u16 > $ROOT_DTH/In_BETA_ED2/value
echo 8 | dthfilter -w u16 > $ROOT_DTH/In_BETA_ED3/value
echo 8 | dthfilter -w u16 > $ROOT_DTH/In_BETA_ED4/value
echo 5 | dthfilter -w u32 > $ROOT_DTH/In_scrambling_code/value
echo 256 | dthfilter -w u16 > $ROOT_DTH/In_DPDCH_spread_factor/value
echo 256 | dthfilter -w u16 > $ROOT_DTH/In_EDPDCH_spread_factor/value
echo 0 | dthfilter -w u8 > $ROOT_DTH/In_HSDPA_activation_delay/value
echo 0 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value

echo ""
echo "--------------- Stop TX WCDMA BAND I"
ROOT_DTH=/mnt/9p/DTH/RF/Tx2G5/StartTx2G5
echo 1 | dthfilter -w u32 > $ROOT_DTH/value
cat $ROOT_DTH/value

echo ""
echo "--------------- Tests END ---------------"
echo ""


# Result
echo " " | tee -a $LOGFILE
echo "|*********************************** Selftest **********************************|" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "| Selftest RFBUS:................................................. $RFBUS |" | tee -a $LOGFILE
echo "| Selftest STROBE:................................................ $STROBE |" | tee -a $LOGFILE
echo "| Selftest RF Supply:............................................. $Supply |" | tee -a $LOGFILE
echo "| Selftest Digital Rx Tx:......................................... $DigitRXTX |" | tee -a $LOGFILE
echo "| Selftest PA id & Pin:........................................... $PAIDPIN |" | tee -a $LOGFILE
echo "| Selftest WCDMA Tx:.............................................. $WCDMATX |" | tee -a $LOGFILE
echo "| Selftest Antenna Low Band:...................................... $AntennaL |" | tee -a $LOGFILE
echo "| Selftest Antenna Higt Band:..................................... $AntennaH |" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
#echo "|******************************* GSM DCXO Tuning *******************************|" | tee -a $LOGFILE
#echo "" | tee -a $LOGFILE
#echo "| GSM DCXO Tuning:................................................ $DCXO |" | tee -a $LOGFILE
#echo "" | tee -a $LOGFILE
echo "|**************************** Start Tx Tunning WCDMA ***************************|" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "| Tx Tunning WCDMA Band I:........................................ $TunTxBI |" | tee -a $LOGFILE
echo "| Tx Tunning WCDMA Band II:....................................... $TunTxBII |" | tee -a $LOGFILE
echo "| Tx Tunning WCDMA Band IV:....................................... $TunTxBIV |" | tee -a $LOGFILE
echo "| Tx Tunning WCDMA Band V:........................................ $TunTxBV |" | tee -a $LOGFILE
echo "| Tx Tunning WCDMA Band VIII:..................................... $TunTxBVIII |" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "|***************************** Start Tx Tunning GSM ****************************|" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "| Tx Tunning GSM Band 850:........................................ $TunTx850 |" | tee -a $LOGFILE
echo "| Tx Tunning GSM Band 900:........................................ $TunTx900 |" | tee -a $LOGFILE
echo "| Tx Tunning GSM Band 1800:....................................... $TunTx1800 |" | tee -a $LOGFILE
echo "| Tx Tunning GSM Band 1900:....................................... $TunTx1900 |" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "|********************************* CAL Tunning *********************************|" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "| CAL Tunning WCDMA Band I:....................................... $CALBI |" | tee -a $LOGFILE
echo "| CAL Tunning WCDMA Band II:...................................... $CALBII |" | tee -a $LOGFILE
echo "| CAL Tunning WCDMA Band IV:...................................... $CALBIV |" | tee -a $LOGFILE
echo "| CAL Tunning WCDMA Band V:....................................... $CALBV |" | tee -a $LOGFILE
echo "| CAL Tunning WCDMA Band VIII:.................................... $CALBVIII |" | tee -a $LOGFILE
echo "| CAL Tunning GSM Band 850:....................................... $CAL850 |" | tee -a $LOGFILE
echo "| CAL Tunning GSM Band 900:....................................... $CAL900 |" | tee -a $LOGFILE
echo "| CAL Tunning GSM Band 1800:...................................... $CAL1800 |" | tee -a $LOGFILE
echo "| CAL Tunning GSM Band 1900:...................................... $CAL1900 |" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "|****************************** Self Tunning WCDMA *****************************|" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "| Self Tunning WCDMA Band I:...................................... $SelfTI |" | tee -a $LOGFILE
echo "| Self Tunning WCDMA Band II:..................................... $SelfTII |" | tee -a $LOGFILE
echo "| Self Tunning WCDMA Band IV:..................................... $SelfTIV |" | tee -a $LOGFILE
echo "| Self Tunning WCDMA Band V:...................................... $SelfTV |" | tee -a $LOGFILE
echo "| Self Tunning WCDMA Band VIII:................................... $SelfTVIII |" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "|******************************* Self Tunning GMSK *****************************|" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "| Self Tunning GMSK Band 850:..................................... $Self850 |" | tee -a $LOGFILE
echo "| Self Tunning GMSK Band 900:..................................... $Self900 |" | tee -a $LOGFILE
echo "| Self Tunning GMSK Band 1800:.................................... $Self1800 |" | tee -a $LOGFILE
echo "| Self Tunning GMSK Band 1900:.................................... $Self1900 |" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "|***************************** Self Tunning IQ 8PSK ****************************|" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "| Self Tunning IQ 8PSK Band 850:.................................. $SelfIQ850 |" | tee -a $LOGFILE
echo "| Self Tunning IQ 8PSK Band 900:.................................. $SelfIQ900 |" | tee -a $LOGFILE
echo "| Self Tunning IQ 8PSK Band 1800:................................. $SelfIQ1800 |" | tee -a $LOGFILE
echo "| Self Tunning IQ 8PSK Band 1900:................................. $SelfIQ1900 |" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "|******************************* Self Tunning 8PSK *****************************|" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "| Self Tunning 8PSK Band 850:..................................... $Self8psk850 |" | tee -a $LOGFILE
echo "| Self Tunning 8PSK Band 900:..................................... $Self8psk900 |" | tee -a $LOGFILE
echo "| Self Tunning 8PSK Band 1800:.................................... $Self8psk1800 |" | tee -a $LOGFILE
echo "| Self Tunning 8PSK Band 1900:.................................... $Self8psk1900 |" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
#echo "|************************************ GSM RSSI *********************************|" | tee -a $LOGFILE
#echo "" | tee -a $LOGFILE
#echo "| GMSK RSSI Band 850:............................................. $RSSI850 |" | tee -a $LOGFILE
#echo "| GMSK RSSI Band 900:............................................. $RSSI900 |" | tee -a $LOGFILE
#echo "| GMSK RSSI Band 1800:............................................ $RSSI1800 |" | tee -a $LOGFILE
#echo "| GMSK RSSI Band 1900:............................................ $RSSI1900 |" | tee -a $LOGFILE
#echo "" | tee -a $LOGFILE
#echo "|*********************************** WCDMA RSSI ********************************|" | tee -a $LOGFILE
#echo "" | tee -a $LOGFILE
#echo "| GMSK RSSI Band I:............................................... $RSSIBI |" | tee -a $LOGFILE
#echo "| GMSK RSSI Band II:.............................................. $RSSIBII |" | tee -a $LOGFILE
#echo "| GMSK RSSI Band IV:.............................................. $RSSIBIV |" | tee -a $LOGFILE
#echo "| GMSK RSSI Band V:............................................... $RSSIBV |" | tee -a $LOGFILE
#echo "| GMSK RSSI Band VIII:............................................ $RSSIBVIII |" | tee -a $LOGFILE
#echo "" | tee -a $LOGFILE
#echo "|****************************** WCDMA RSSI Diversity ***************************|" | tee -a $LOGFILE
#echo "" | tee -a $LOGFILE
#echo "| GMSK RSSI Band I:............................................... $RSSIDIVBI |" | tee -a $LOGFILE
#echo "| GMSK RSSI Band II:.............................................. $RSSIDIVBII |" | tee -a $LOGFILE
#echo "| GMSK RSSI Band IV:.............................................. $RSSIDIVBIV |" | tee -a $LOGFILE
#echo "| GMSK RSSI Band V:............................................... $RSSIDIVBV |" | tee -a $LOGFILE
#echo "" | tee -a $LOGFILE
echo "|*******************************************************************************|" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
#echo "!WARNING! : If RSSI and DCXO tests are failed, it's due to CMU absence" | tee -a $LOGFILE
echo "Tests results have been saved in $LOGFILE"

if [ $# -le 70 ]
then
  echo "$0 runs GSM power TX tuning"
  echo "arg1 => channel"
  echo "arg2 => RF Band: WCDMA: 1=BAND I, 2=BAND II, 3=BAND III, 4=BAND IV,  5=BAND V, 6=BAND VI, 7=BAND VII, 8=BAND VIII, 9=BAND IX"
  echo "arg3 =>Max tuned power (dBm)"
  echo "arg4 => Index step size for 1dB"
  echo "arg5 => Amount of control data"
  echo "arg6 => Tuning Option 0=CW+NO_PMM, 1=WCDMA+NO_PMM, 2=CW+PMM, 3=WCDMA+PMM"
  echo "arg7 => Step duration (µs)"
  echo "arg8..39 => PA VCC"
  echo "arg40..71 => PA bias"
  exit 1
fi

ROOT_DTH=/mnt/DTH/RF/SelfTuning/WCDMA/WCDMA_TX
if [ $# -eq 71 ]; then
  echo $1 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
  echo $2 | dthfilter -w u32 > $ROOT_DTH/In_Band/value
  echo $3 | dthfilter -w float > $ROOT_DTH/In_Tx_Target_Pwr_Start/value
  echo $4 | dthfilter -w s16 > $ROOT_DTH/In_Pwr_Ctrl_Step/value
  echo $5 | dthfilter -w u16 > $ROOT_DTH/In_Amount_Ctrl_Data/value
  echo $6 | dthfilter -w u16 > $ROOT_DTH/In_Tuning_Options/value 
  echo $7 | dthfilter -w u16 > $ROOT_DTH/In_Step_Duration/value 
  echo "${8};${9};${10};${11};${12};${13};${14};${15};${16};${17};${18};${19};${20};${21};${22};${23};${24};${25};${26};${27};${28};${29};${30};${31};${32};${33};${34};${35};${36};${37};${38};${39}" | dthfilter -w s16 > $ROOT_DTH/In_Pa_Vcc/value
  echo "${40};${41};${42};${43};${44};${45};${46};${47};${48};${49};${50};${51};${52};${53};${54};${55};${56};${57};${58};${59};${60};${61};${62};${63};${64};${65};${66};${67};${68};${69};${70};${71}" | dthfilter -w s16 > $ROOT_DTH/In_Pa_Bias/value
  
  cat $ROOT_DTH/value
  echo "ALL PARAMETERS ARE SET."
 
else
    exit 2
fi



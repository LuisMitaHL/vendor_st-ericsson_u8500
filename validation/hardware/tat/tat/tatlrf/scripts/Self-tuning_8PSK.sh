if [ $# -le 18 ]
then
  echo "$0 runs GSM power TX tuning"
  echo "arg1 => channel"
  echo "arg2 => RF Band: GSM: 1=850MHz, 2=900MHz, 3=1800MHz, 4=1900MHz"
  echo "arg3 =>Pa ctrl data in use 0= internal, 1= external"
  echo "arg4..19 => Target power"
  exit 1
fi

ROOT_DTH=/mnt/DTH/RF/SelfTuning/8PSK/8PSK_TX
if [ $# -eq 19 ]; then
  echo $1 | dthfilter -w u16 > $ROOT_DTH/In_Tx_Channel/value
  echo $2 | dthfilter -w u16 > $ROOT_DTH/In_Band/value
  echo "0" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Data_Valid/value
  echo "${4};${5};${6};${7};${8};${9};${10};${11};${12};${13};${14};${15};${16};${17};${18};${19}" | dthfilter -w float > $ROOT_DTH/In_Target_Power/value
  echo "0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Pa_Vcc/value
  echo "0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0"  | dthfilter -w u16 > $ROOT_DTH/In_Pa_Bias/value 
  echo 0 | dthfilter -w u32 > $ROOT_DTH/value
  
  cat $ROOT_DTH/value
  echo "ALL PARAMETERS ARE SET."
 
else
    exit 2
fi



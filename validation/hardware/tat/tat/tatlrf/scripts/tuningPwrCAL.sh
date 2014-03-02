if [ $# -le 8 ]
then
  echo "$0 runs GSM power TX tuning"
  echo "arg1 => RF mode: 0=GSM, 1=WCDMA"
  echo "arg2 => RF Band:"
  echo "   GSM: 1=850MHz, 2=900MHz, 4=1800MHz, 8=1900MHz"
  echo "   WCDMA: 1=I, 2=II, 4=III, 8=IV, 16=V, 32=VI, 64=VII, 128=VIII, 256=IX"
  echo "arg3 => Amount of data: 0=2 datas, 1=6 datas"
  echo "arg4..n => channels"
  echo "argn+1..x => Internal power measurement"
  echo "argx+1..y => External power measurement"
  exit 1
fi

ROOT_DTH=/mnt/DTH/RF/Tuning/Power/CAL/Start 
if [ $3 -eq 0 -a $# -eq 9 ]; then
  echo $1 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
  echo $2 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
  echo $3 | dthfilter -w u8 > $ROOT_DTH/In_AmountOfData/value
  echo "${4};${5};0;0;0;0" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
  echo "${6};${7};0;0;0;0" | dthfilter -w float > $ROOT_DTH/In_IntMeasures/value
  echo "${8};${9};0;0;0;0" | dthfilter -w float > $ROOT_DTH/In_ExtMeasures/value

  cat $ROOT_DTH/value
  echo "ALL PARAMETERS ARE SET."
  
elif [ $3 -eq 1 -a $# -eq 21 ]; then
  echo $1 | dthfilter -w u32 > $ROOT_DTH/In_Mode/value
  echo $2 | dthfilter -w u32 > $ROOT_DTH/In_RFBand/value
  echo $3 | dthfilter -w u8 > $ROOT_DTH/In_AmountOfData/value
  echo "${4};${5};${6};${7};${8};${9}" | dthfilter -w u16 > $ROOT_DTH/In_Channels/value
  echo "${10};${11};${12};${13};${14};${15}" | dthfilter -w float > $ROOT_DTH/In_IntMeasures/value
  echo "${16};${17};${18};${19};${20};${21}" | dthfilter -w float > $ROOT_DTH/In_ExtMeasures/value

  cat $ROOT_DTH/value
  echo "ALL PARAMETERS ARE SET."
 
else
    exit 2
fi



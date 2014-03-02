#!/bin/sh

DRY_RUN=0

test_xti=1
test_dsp_on_arm=1

execute()
{
  if [ $DRY_RUN -ne 1 ] ; then
    bash -c "$1"
  else
    echo "$1"
  fi

  if [ $? -ne 0 ] ; then
    exit 1
  fi

  return $?

}

abspath=`readlink -f $0`
test_dir=`dirname $abspath`
test_res_dir=$test_dir/test_results

mkdir -p $test_res_dir

echo "Executing: $abspath $*"

echo "test_dir: $test_dir"

# Check if we are calling from Android or LBP environment
if [ -z $ANDROID_PRODUCT_OUT ] ; then
  # Seems that not executing from Android, then try to see if calling from LBP
  if [ -z $LBP_PRODUCT_OUT ] ; then
    echo "Error: LBP_PRODUCT_OUT not defined !!"
    exit 1;
  fi
  echo "Assuming LBP environment"
  OUT_DIR=$LBP_PRODUCT_OUT
  OS=linux
else
  echo "Assuming ANDROID environment"
  OUT_DIR=$ANDROID_PRODUCT_OUT
  OS=android
fi

TARGET=`basename $OUT_DIR`
if [ "$TARGET" = "u8500" ] ; then
  PLATFORM=${TARGET}_v2-${OS}
elif [ "$TARGET" = "u9540" ] ; then
  PLATFORM=${TARGET}_v1-${OS}
else
  echo "Unknown TARGET: $TARGET"
  exit 1
fi

# Check whether perl exists and is in the path
if [ -z "`which adb 2>/dev/null`" ]; then
  echo
  echo "*** ERROR: ADB is required !!***"
  exit 1;
fi

if [ "$test_xti" = "1" ] ; then
echo "Waiting for adb to answer...."
adb wait-for-device

# FLush logcat
execute "adb logcat -c"

# Launch fido server
echo "Launch Fido server"
fido -protocol=legacy -nogui &
sleep 2s
PID_FIDO=`ps | grep 'fido' | grep -v 'grep' | awk '{print $1}'`
echo "PID FIDO: $PID_FIDO"

# start capture
echo "Start capture"
$test_dir/FTip ost_trace1.bin -fp pn &
PID_FTip=$!
echo "PID FTip: $PID_FTip"

# Write script on the target
echo "Generate target test script"
cat << EOF > /tmp/ost_test.sh

cd /data/busybox ; . ./setup.sh ; cd -
export PATH=$PATH:/data/busybox/bin
cd /system/bin
logcat &
st_osttest1
EOF

chmod +x /tmp/ost_test.sh

# Load script on the target
execute "adb push /tmp/ost_test.sh /data"

# FLush logcat
execute "adb logcat -c"

# Execute script on the target
echo "Execute test"
execute "adb shell /data/ost_test.sh"

# Pause to let traces being flushed
sleep 2s

# Kill FTip process (CTR+C)
kill -9 $PID_FTip
mv ost_trace1.bin $test_res_dir

# Generate dico list file
echo "Generate dico list file"
perl $test_dir/../../osttools/ostscripts/scripts/create_dico_list.pl --dico_list=$test_res_dir/ostdico_list.txt --defines=debug=2 --dico_path=$OUT_DIR/obj/mmbuildout/dictionaries/$PLATFORM > /dev/null

# Decode
echo "Decode..."
perl -S $test_dir/../../osttools/ostdec/ostdec.pl --input=dico=$test_res_dir/ostdico_list.txt --input=file=$test_res_dir/ost_trace1.bin --output=$test_res_dir/ost_trace_test1.txt --defines=nostdout --defines=rmdisplay=pp_ts > /dev/null

# Check results
echo "Check results"
perl -S $test_dir/verify.pl --action=diff --defines=input1=$test_res_dir/ost_trace_test1.txt --defines=input2=$test_dir/ref/tu_ost_test1/ost_trace_ref.txt --defines=diff2=1 --defines=outputdir=$test_res_dir

if [ $? -eq 0 ] ; then \
  echo "******************" ; \
  echo "TEST1: SUCESSFULL :)" ; \
  echo "******************" ; \
  echo "TEST1: SUCESSFULL" > $test_res_dir/test_log.txt; \
else \
  echo "******************" ; \
  echo "TEST1: FAILED (-"; \
  echo "******************" ; \
  echo "TEST1: FAILED" > $test_res_dir/test_log.txt; \
fi

##########################
# Execute test2: OST trace capture over XTI
#########################

# start capture
echo "Start capture"
$test_dir/FTip ost_trace2.bin -fp pn &
PID_FTip=$!
echo "PID FTip: $PID_FTip"

# Write script on the target
echo "Generate target test script"
cat << EOF > /tmp/ost_test.sh

cd /data/busybox ; . ./setup.sh ; cd -
export PATH=$PATH:/data/busybox/bin
cd /system/bin
logcat &
st_osttest2 0
EOF

chmod +x /tmp/ost_test.sh

# Load script on the target
execute "adb push /tmp/ost_test.sh /data"

# FLush logcat
execute "adb logcat -c"

# Execute script on the target
echo "Execute test"
execute "adb shell /data/ost_test.sh"

# Pause to let traces being flushed
sleep 2s

# Kill FTip process (CTR+C)
kill -9 $PID_FTip
mv ost_trace2.bin $test_res_dir

# Generate dico list file
echo "Generate dico list file"
perl $test_dir/../../osttools/ostscripts/scripts/create_dico_list.pl --dico_list=$test_res_dir/ostdico_list.txt --defines=debug=2 --dico_path=$OUT_DIR/obj/mmbuildout/dictionaries/$PLATFORM > /dev/null

# Decode
echo "Decode..."
perl -S $test_dir/../../osttools/ostdec/ostdec.pl --input=dico=$test_res_dir/ostdico_list.txt --input=file=$test_res_dir/ost_trace2.bin --output=$test_res_dir/ost_trace_test2.txt --defines=nostdout --defines=rmdisplay=pp_ts > /dev/null

# Check results
echo "Check results"
perl -S $test_dir/verify.pl --action=diff --defines=input1=$test_res_dir/ost_trace_test2.txt --defines=input2=$test_dir/ref/tu_ost_test2/ost_trace_ref.txt --defines=diff2=1 --defines=outputdir=$test_res_dir

if [ $? -eq 0 ] ; then \
  echo "******************" ; \
  echo "TEST2: SUCESSFULL :)" ; \
  echo "******************" ; \
  echo "TEST2: SUCESSFULL" >> $test_res_dir/test_log.txt; \
else \
  echo "******************" ; \
  echo "TEST2: FAILED (-"; \
  echo "******************" ; \
  echo "TEST2: FAILED" >> $test_res_dir/test_log.txt; \
fi


# Kill fido processes
kill -9 $PID_FIDO
fi
if [ "$test_dsp_on_arm" = "1" ] ; then

#######################################################
# Execute test2: OST trace capture with ARM redirection
#######################################################
cat << EOF > /tmp/ost_test.sh

cd /data/busybox ; . ./setup.sh ; cd -
export PATH=$PATH:/data/busybox/bin

rm /sdcard/trace_sva.bin
rm /sdcard/trace_sia.bin
cat /dev/cm_sva_trace > /sdcard/trace_sva.bin &
#process_sva=\$!
process_sva=\`ps | grep 'cat /dev/cm_sva_trace' | grep -v 'grep' | awk '{print \$1}'\`
echo "cat process for cm_sva_trace: \$process_sva"

cat /dev/cm_sia_trace > /sdcard/trace_sia.bin &
#process_sia=\$!
process_sia=\`ps | grep 'cat /dev/cm_sia_trace' | grep -v 'grep' | awk '{print \$1}'\`
echo "cat process for cm_sia_trace: \$process_sia"

cd /system/bin
logcat &
st_osttest2 1
kill -9 \$process_sva
kill -9 \$process_sia
ps | grep cat
ls -l /sdcard/*trace*.bin
EOF

chmod +x /tmp/ost_test.sh

echo "Waiting for adb to answer...."
adb wait-for-device

# FLush logcat
execute "adb logcat -c"

# Load script on the target
execute "adb push /tmp/ost_test.sh /data"

# Execute script on the target
execute "adb shell /data/ost_test.sh"

# Upload trace files
execute "adb pull /sdcard/trace_sia.bin $test_res_dir"
execute "adb pull /sdcard/trace_sva.bin $test_res_dir"

# Generate dico list file
perl $test_dir/../../osttools/ostscripts/scripts/create_dico_list.pl --dico_list=$test_res_dir/ostdico_list.txt --defines=debug=2 --dico_path=$OUT_DIR/obj/mmbuildout/dictionaries/$PLATFORM > /dev/null

# Decode traces
perl -S $test_dir/../../osttools/ostdec/ostdec.pl --input=dico=$test_res_dir/ostdico_list.txt --input=file=$test_res_dir/trace_sva.bin --output=$test_res_dir/ost_trace_sva_test.txt --defines=nostdout --defines=rmdisplay=pp_ts > /dev/null

# compare results
perl -S $test_dir/verify.pl --action=diff --defines=input1=$test_res_dir/ost_trace_sva_test.txt --defines=input2=$test_dir/ref/tu_ost_test2/ost_trace_sva_ref.txt --defines=diff2=1 --defines=outputdir=$test_res_dir
if [ $? -eq 0 ] ; then \
  echo "******************" ; \
  echo "TEST3 SVA: SUCESSFULL :)" ; \
  echo "******************" ; \
  echo "TEST3 SVA: SUCESSFULL" >> $test_res_dir/test_log.txt; \
else \
  echo "******************" ; \
  echo "TEST3 SVA: FAILED (-"; \
  echo "******************" ; \
  echo "TEST3 SVA: FAILED" >> $test_res_dir/test_log.txt; \
fi


# Decode traces
 perl -S $test_dir/../../osttools/ostdec/ostdec.pl --input=dico=$test_res_dir/ostdico_list.txt --input=file=$test_res_dir/trace_sia.bin --output=$test_res_dir/ost_trace_sia_test.txt --defines=nostdout --defines=rmdisplay=pp_ts > /dev/null
perl -S $test_dir/verify.pl --action=diff --defines=input1=$test_res_dir/ost_trace_sia_test.txt --defines=input2=$test_dir/ref/tu_ost_test2/ost_trace_sia_ref.txt --defines=diff2=1 --defines=outputdir=$test_res_dir
if [ $? -eq 0 ] ; then \
  echo "******************" ; \
  echo "TEST3 SIA: SUCESSFULL :)" ; \
  echo "******************" ; \
  echo "TEST3 SIA: SUCESSFULL" >> $test_res_dir/test_log.txt; \
else \
  echo "******************" ; \
  echo "TEST3 SIA: FAILED (-"; \
  echo "******************" ; \
  echo "TEST3 SIA: FAILED" >> $test_res_dir/test_log.txt; \
fi
fi

cat $test_res_dir/test_log.txt

#!/bin/sh

DRY_RUN=0

execute()
{
  if [ $DRY_RUN -ne 1 ] ; then
    bash -c "$1"
  else
    echo "$1"
  fi

  if [ $? -ne 0 ] ; then
    echo "Error!!!!!!!!"
    return 1
  fi

  return $?

}

abspath=`readlink -f $0`
the_dir="$ANDROID_BUILD_TOP/vendor/st-ericsson/multimedia/shared/osttrace/test"
here=`pwd`

echo "Executing: $abspath $*"


check_env()
{
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
    ROOT_DIR=$LBP_BUILD_TOP
  else
    echo "Assuming ANDROID environment"
    OUT_DIR=$ANDROID_PRODUCT_OUT
    OS=android
    ROOT_DIR=$ANDROID_BUILD_TOP
  fi

  if [ -z $ROOT_DIR ] ; then
    echo "Error: $ROOT_DIR not defined !!"
    exit 1;
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

  # Check whether adb exists and is in the path
  if [ -z "`which adb 2>/dev/null`" ]; then
    echo
    echo "*** ERROR: ADB is required !!***"
    exit 1;
  fi

}

function decode_ost()
{
  inputfile=$1
  outputfile=`basename $inputfile`
  outputfile=`echo "$outputfile" | cut -d'.' -f1`

  if [ ! -f "$inputfile" ] ; then
    echo "Missing input file: $inputfile"
  fi

  echo "OST traces decoding into $here/$outputfile.txt"
  check_env
  #generate dico list file
  perl $ROOT_DIR/vendor/st-ericsson/multimedia/shared/osttools/ostscripts/scripts/create_dico_list.pl --dico_list=$here/ostdico_list.txt --defines=debug=2 --dico_path=$OUT_DIR/obj/mmbuildout/dictionaries/$PLATFORM > /dev/null

  #decode
  perl $ROOT_DIR/vendor/st-ericsson/multimedia/shared/osttools/ostdec/ostdec.pl --input=dico=$here/ostdico_list.txt --input=file=$here/$inputfile --output=$here/$outputfile.txt --defines=nostdout --defines=alert=fido > /dev/null
}


function dsp_log_init
{

  echo "Upload local.prop and STEtraceSpec.txt"
  check_env
  echo "Waiting for adb to answer...."
  adb wait-for-device

  echo "the_dir: $the_dir"

  # Load local.prop on the target
  execute "adb push $the_dir/local.prop /data"

  # Load STEtraceSpec.txt on the target
  execute "adb push $the_dir/STEtraceSpec.txt /data"

  # reboot to take into account
  execute "adb reboot"

  echo "Waiting for adb to answer...."
  adb wait-for-device

}

function dsp_log
{
  record="10s"

  if [ $1 == ""] ; then
    echo "Use default logging time: $record"
  else
    record=$1s
  fi

  echo "start DSP OST traces logging for: $record"

 check_env
cat << EOF > /tmp/dsp_log.sh
cd /data/busybox ; . ./setup.sh ; cd -
#/data/busybox/bin/busybox sh -c
export PATH=$PATH:/data/busybox/bin

rm /sdcard/trace_sva.bin
rm /sdcard/trace_sia.bin
cat /dev/cm_sva_trace > /sdcard/trace_sva.bin&
cat /dev/cm_sia_trace > /sdcard/trace_sia.bin&
ps | grep cat
ls -l /sdcard/*trace*.bin
echo "Waiting for $record ...."
sleep $record
ps | grep cat
ls -l /sdcard/*trace*.bin
EOF

chmod +x /tmp/dsp_log.sh

echo "Waiting for adb to answer...."
adb wait-for-device

# Load script on the target
execute "adb push /tmp/dsp_log.sh /data"

# Execute script on the target
execute "adb shell /data/dsp_log.sh"

# Upload trace files
execute "adb pull /sdcard/trace_sia.bin ."
execute "adb pull /sdcard/trace_sva.bin ."

decode_ost trace_sia.bin
decode_ost trace_sva.bin
}





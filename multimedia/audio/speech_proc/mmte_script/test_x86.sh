#! /bin/sh


cd $MMROOT/audio/speech_proc/mmte_script

echo "alias NOMADIK  $MMROOT/audio 1" > $0.ate
echo "do $MMROOT/audio/speech_proc/mmte_script/speech_proc_config.ate" >> $0.ate
echo "quit" >> $0.ate


if ( $MMROOT/shared/mmte/bin/x86/mmte.exe -f $0.ate ) then
    rm $0.ate
else
    echo "error"
fi






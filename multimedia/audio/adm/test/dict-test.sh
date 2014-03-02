#!/bin/sh

PATH=$PATH:../x86/

for a in `seq 80000`; do
  echo $a

  ste-adm-test play /bin/ls x-ste-adm-dict-play &
  pp=$!

  ste-adm-test record /dev/null x-ste-adm-dict-rec &
  rp=$!

  wait $pp
  wait $rp


  ste-adm-test play /bin/ls x-ste-adm-dict-play &
  pp=$!

  ste-adm-test record /dev/null x-ste-adm-dict-rec -bytes 200000&
  rp=$!



  wait $pp
  wait $rp



  ste-adm-test record /dev/null x-ste-adm-dict-rec &
  rp=$1;

  ste-adm-test play /bin/ls x-ste-adm-dict-play &
  pp=$!

  wait $pp
  wait $rp


  ste-adm-test record /dev/null x-ste-adm-dict-rec -bytes 200000 &
  rp=$!

  ste-adm-test play /bin/ls x-ste-adm-dict-play &
  pp=$!

  wait $pp
  wait $rp



  sleep 1
done




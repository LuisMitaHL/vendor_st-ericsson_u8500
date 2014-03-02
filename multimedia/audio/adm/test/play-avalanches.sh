PATH=$PATH:../x86/
for a in a a a a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a  a a a a a aa a a a a a a a a a a a a a a a a a a a a a a a a a a a a ; do
  ste-adm-test play /sbin/adbd HSetOut &
  p1=$!;

  ste-adm-test play /sbin/adbd HSetOut &
  p2=$!;

  ste-adm-test play /sbin/adbd HSetOut &
  p3=$!;

  ste-adm-test play /sbin/adbd HSetOut &
  p4=$!;

  ste-adm-test play /sbin/adbd HSetOut &
  p5=$!;

  ste-adm-test play /sbin/adbd Speaker &
  p6=$!;

  ste-adm-test play /sbin/adbd Speaker &
  p7=$!;

  ste-adm-test play /sbin/adbd Speaker &
  p8=$!;

  ste-adm-test play /sbin/adbd Speaker &
  p9=$!;

  ste-adm-test play /sbin/adbd Speaker &
  p10=$!;


  # wait returns 129 for successful cmd in android shell
  wait $p1  
  wait $p2 
  wait $p3 
  wait $p4
  wait $p5
  wait $p6
  wait $p7
  wait $p8
  wait $p9
  wait $p10 

  sleep 1
done




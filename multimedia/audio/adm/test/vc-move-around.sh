#
# Feeds ADM with different voicecall routings.
# Unfortunately there are no VC entries with
# the same output device for two
# pairs with different input devices. For
# the other way around, there is [Mic,Speaker] and [Mic,Earpiece]
#

PATH=$PATH:../x86/
delay=8
its=$1
: ${its:=5}


(for a in a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a aa a a a a a a a a a a a a  a a a a a a a a a a a a a a a a a a a a a a a a aa a a a a a a a a a a a a  a a a a a a a a a a a a a a a a a a a a a a a a aa a a a a a a a a a a a a  a a a a a a a a a a a a a a a a a a a a a a a a aa a a a a a a a a a a a a  a a a a a a a a a a a a a a a a a a a a a a a a aa a a a a a a a a a a a a  a a a a a a a a a a a a a a a a a a a a a a a a aa a a a a a a a a a a a a  a a a a a a a a a a a a a a a a a a a a a a a a aa a a a a a a a a a a a a  a a a a a a a a a a a a a a a a a a a a a a a a aa a a a a a a a a a a a a  a a a a a a a a a a a a a a a a a a a a a a a a aa a a a a a a a a a a a a  a a a a aa a a a a a a a a a a a a a; do
	echo >/proc/self/fd/2 ------------------------------
	echo >/proc/self/fd/2      Iteration $a of $its
        echo >/proc/self/fd/2 ------------------------------

	echo HSetIn HSetOut
	sleep $delay
	echo Mic Speaker
	sleep $delay
        echo Mic Earpiece
        sleep $delay
        echo BTIn BTOut
        sleep $delay
        echo HSetIn HSetOut
        sleep 2
        echo HSetIn HSetOut
        sleep $delay

        echo None
        sleep $delay
        echo HSetIn HSetOut
        sleep $delay
	echo None
	sleep $delay
        echo Mic Speaker
        sleep $delay
	echo None
	sleep $delay
        echo BTIn BTOut
        sleep $delay
done; echo exit) | ste-adm-test voicecall


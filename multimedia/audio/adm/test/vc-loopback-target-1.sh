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


ste-adm-test csloop 1 7

(for a in `seq $its`; do
	echo >/proc/self/fd/2 ------------------------------
	echo >/proc/self/fd/2 Iteration $a of $its
        echo >/proc/self/fd/2 ------------------------------

	sleep $delay
	echo none
	sleep $delay
        echo HSetIn HSetOut
done; echo exit) | ste-adm-test voicecall


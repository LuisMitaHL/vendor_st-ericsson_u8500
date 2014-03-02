#
# Voiceall with ring signal
#


PATH=$PATH:../x86/
delay=8
its=$1
: ${its:=5}
: ${playfile:=/bin/ls}

ste-adm-test csloop 1 7

for a in `seq $its`; do
        echo ------------------------------
        echo      Iteration $a of $its
        echo ------------------------------

	# Play ring signal asynchronously
	ste-adm-test play $playfile HSetOut -duration 1200 &
	pplay_pid=$!

	# Keep voicecall open 5 sec
	( sleep 5; echo exit) | ste-adm-test voicecall &
	vc_pid=$!

	sleep 4;

	# Play hangup signal
	ste-adm-test play $playfile -duration 1900 HSetOut &
	hup_pid=$!

	wait $pplay_pid
	wait $vc_pid
	wait $hup_pid

        sleep 2
done;





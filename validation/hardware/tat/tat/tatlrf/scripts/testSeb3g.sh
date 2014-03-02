#! /bin/sh
#	Copyright (C) ST-Ericsson 2011
#	HATS RF feature test for Single BER 3G.
#	This script is member of HATS unitary test tools.
#	Author:	alain.crouzet-nonst@stericsson.com

mode=$1
path="RF/BER/3G"

test_PASS()
{
	local name=$1
	echo -e "Test \033[32mPASS\033[0m: $name"
}

test_FAIL()
{
	local name=$1
	local text=$2
	
	echo -e "Test \033[31mFAIL\033[0m: $name"

	if [ "$text" != "" ]
	then
		echo "$text"
	fi
}

test_result()
{
	local name=$1
	local expected=$2
	local criticity=$3
	res=`./dthadv.sh $path/Command/Action/Out_CmdStatus g`

	if [ "$expected" = "$res" ]; then
		test_PASS "$name"
	else
		test_FAIL "$name" "Operation status: $res"
		if [ "$criticity" = "fatal" ]; then
			exit 1
		fi
	fi
}

test_dutStatus()
{
	local name=$1
	local status=$2
	res=`./seb3gStatus.sh`

	if [ "$status" = "$res" ]; then
		test_PASS "$name"
	else
		test_FAIL "$name" "DUT state: $res"
	fi
}

test_all()
{
	# sets ISI message format
	./dthadv.sh $path/Command/Action/In_Sequence s $1

    # reinit module for test, if necessary
    res=`./seb3gStatus.sh`
    if [ $res -ne "0" ]
    then
	    ./seb3g.sh reset
	    test_result "init" "0" "fatal"
    fi

    # Should ever be in OFF state before beginning the test sequence.
    res=`./seb3gStatus.sh`
    test_dutStatus "module in initial state" "0"

    # store the list twice. The first time, discards it with SEQUENCER_RESET_REQ.
    for i in 1 2
    do
	    # nominal scenario 1: sets two joined pages, store them, run the test, check
	    # status until it is "ready", stop the test and finaly reset the sequencer.
	    ./seb3gSync.sh 1 8 1417 7 1204 78 6 7 12.6

	    # BAND 1, ch 9740
	    ./seb3gSetPage.sh 1 1 9740 128 -25

	    # BAND 4, ch 1412
	    ./seb3gSetPage.sh 5 8 1412 16 7

	    # store
	    ./seb3g.sh store
	    test_result "store 2 bands, 2 subblocks single BER 3G" "0" "fatal"

	    # should have gone to READY
	    test_dutStatus "go READY state after store" "1"

	    # first time, reset
	    if [ $i -eq 1 ]
	    then
		    if [ "$mode" = "nominal" ]; then
			    break;
		    else
			    ./seb3g.sh reset
			    test_result "reset on READY" "0"
			    test_dutStatus "go back OFF on reset on READY" "0"
		    fi
	    fi
    done

    # GET_RESULTS_REQ is allowed in READY state (but will be C_TEST_FAIL here)
    ./seb3g.sh result
    test_result "ask results in READY state" "1"

    # Test RUN state
    ./seb3g.sh start
    test_result "start single BER 3G" "0" "fatal"
    test_dutStatus "gone RUN after start" "2"

    if [ "$mode" != "nominal" ]
    then
	    # stop right here
	    ./seb3g.sh stop

	    test_result "stop single BER 3G" "0"

	    if [ "0" = "$res" ]
	    then
		    test_dutStatus "go back READY on stop" "1"

		    # run again and wait READY this time
		    # if the start is done too hasty then the sequencer acts weird
		    sleep 1
		    ./seb3g.sh start
		    test_result "start again single BER 3G" "0" "fatal"
		    test_dutStatus "gone RUN after start" "2"
	    fi
    fi

    res=0
    guard=10 # about 10 seconds
    while [ "$res" -ne "1" -a $guard -gt 0 ]
    do
	    #echo "check if DUT still busy..."
	    res=`./seb3gStatus.sh`

	    guard=`expr $guard - 1`
	    #echo "$guard"

	    sleep 1
    done
    test_dutStatus "gone READY after measurements" "1"

    ./seb3g.sh result
    test_result "ask results after measurement" "0"

    # end of test: return to default state
    ./seb3g.sh reset
    res=`./dthadv.sh $path/Command/Action/Out_CmdStatus g`
    test_result "reset after Test" "0"
}


# STORE_COMMAND_LIST_REQ and GET_RESULTS_RESP using one subblock per frequency.
echo "[ multiple subblocks for STORE ]"
test_all $format_msg 0

# STORE_COMMAND_LIST_REQ and GET_RESULTS_RESP always using one subblock and only
# one.
echo "[ one unique subblock for STORE ]"
test_all $format_msg 1


#!/bin/bash

	simd="runAllModuleTests_simd.log"

	vgopts="-v --leak-check=full --track-origins=yes --show-reachable=yes --num-callers=20 --track-fds=yes --malloc-fill=0xCC --free-fill=0xCD"

	function runTests {

		# Start simd/valgrind in the backround
		{ valgrind $vgopts $TOP/vendor/st-ericsson/access_services/sim/sim/simd/simd -vvvv &>> $simd & }
		sleep 5

		#tuicc startup
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc startup

		#modem simulator running all tests
		echo all | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
		sleep 1

		#run tcat
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat
		sleep 1

		# Run tcat with client connected
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		echo pc_displaytext | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		echo pc_getinkey | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		echo pc_setupmenu | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		echo pc_getinput | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
		sleep 1

		 #run tapdu
		$TOP/vendor/st-ericsson/access_services/sim/sim/tapdu/tapdu
		sleep 1

		#run tuicc
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc
		sleep 1

		#tuicc shutdown
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc shutdown
	}

	function run_tcat {

		# Start simd/valgrind in the backround
		{ valgrind $vgopts $TOP/vendor/st-ericsson/access_services/sim/sim/simd/simd -vvvv &>> $simd & }
		sleep 5

		#run tcat startup
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat startup
		sleep 3

		#run tcat
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat
		sleep 10

		#run tcat shutdown
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat shutdown
		sleep 3

	}

	function run_tcat_client {

		# Start simd/valgrind in the backround
		{ valgrind $vgopts $TOP/vendor/st-ericsson/access_services/sim/sim/simd/simd -vvvv &>> $simd & }
		sleep 5

		#tuicc startup
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc startup

		# Run tcat with client connected
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupeventlist | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
		echo pc_displaytext | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
		echo pc_getinkey | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
		sleep 1
                $TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_01 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_02 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_03 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_04 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_hold |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_hold_01 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_hold_02 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_disconnect |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_disconnect_01 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_disconnect_02 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_disconnect_03 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_disconnect_04 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_disconnect_05 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_disconnect_06 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_disconnect_07 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_disconnect_08 |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupecall |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
                $TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
                sleep 1
		echo pc_setupmenu | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
		echo pc_getinput | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_selectitem |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_refresh |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_refresh_full_change |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_refresh_reset |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		#Send Ready Indication after Refresh Reset to trigger TP download
                echo send_cat_ready |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
		#tuicc shutdown
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc shutdown
                sleep 1

		# Start simd/valgrind in the backround
		{ $TOP/vendor/st-ericsson/access_services/sim/sim/simd/simd -vvvv &>> $simd & }
		sleep 5

		#tuicc startup
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc startup
                sleep 1
		# Run tcat with client connected
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
		sleep 1
                echo pc_setupcall_00 | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
		sleep 1
		#tuicc shutdown
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc shutdown
                sleep 1
		# Start simd/valgrind in the backround
		{ $TOP/vendor/st-ericsson/access_services/sim/sim/simd/simd -vvvv &>> $simd & }
		sleep 5

		#run tuicc startup
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc startup

		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc poweroff
                sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc poweron
                sleep 1
                #Send Cat Ready to trigger TP download after power On
                echo send_cat_ready |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
                echo send_uicc_ready |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 5
                echo card_fallback |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1

                #modem reset will trigger silent reset
                echo modem_reset_ind |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 2

	}

	function run_tuicc {
		# Start simd/valgrind in the backround
		{ valgrind $vgopts $TOP/vendor/st-ericsson/access_services/sim/sim/simd/simd -vvvv &>> $simd & }
		sleep 5

		#run tuicc startup
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc startup

		#run tuicc
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc
		sleep 1

		#run tuicc shutdown
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc shutdown
	}

	function run_tapdu {

		# Start simd/valgrind in the backround
		{ valgrind $vgopts $TOP/vendor/st-ericsson/access_services/sim/sim/simd/simd -vvvv &>> $simd & }
		sleep 5

		#tuicc startup
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc startup

		#run tapdu
		$TOP/vendor/st-ericsson/access_services/sim/sim/tapdu/apdu_test
		sleep 1

		#run tuicc shutdown
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc shutdown
	}

	function run_tcat_all_pc {

		# Start simd/valgrind in the backround
		{ valgrind $vgopts $TOP/vendor/st-ericsson/access_services/sim/sim/simd/simd -vvvv &>> $simd & }
		sleep 5

		#tuicc startup
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc startup

		#modem simulator running all tests
		echo all | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
		sleep 1

		#tuicc shutdown
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc shutdown
	}

	function run_tuicc_sim {

		# Start simd/valgrind in the backround
		{ valgrind $vgopts $TOP/vendor/st-ericsson/access_services/sim/sim/simd/simd -vvvv &>> $simd & }
		sleep 5

		#tuicc startup
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc -sim startup

		#run tuicc relevant cases for SIM card
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc -sim condis
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc -sim select_relative
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc -sim getServiceAvailability
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc -sim getServiceTable
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc -sim updateServiceTable
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc -sim ir
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc -sim ecc
		sleep 1
                $TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc -sim disablePIN
                sleep 1

		#tuicc shutdown
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc -sim shutdown

                sleep 5
	}

        function run_tuicc_fail_tests {

		# Start simd/valgrind in the backround
		{ valgrind $vgopts $TOP/vendor/st-ericsson/access_services/sim/sim/simd/simd -vvvv &>> $simd & }
		sleep 5
		#tuicc startup
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc -sim startup
                sleep 1
                echo set_uicc_fail_tests |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1

		#run tuicc relevant cases
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc sendPIN
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc changePIN
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc disablePIN
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc disablePIN
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc enablePIN
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc PINinfo
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc sendPUK
		sleep 1
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc simChannelCloseFail01
		sleep 1
                $TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc simChannelOpenFail01
		sleep 1

                echo reset_uicc_fail_tests |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1

		#tuicc shutdown
		$TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc -sim shutdown
	}

        function run_bip {

                # Start simd/valgrind in the backround
                { valgrind $vgopts $TOP/vendor/st-ericsson/access_services/sim/sim/simd/simd -vvvv &>> $simd & }
                sleep 5

                #tuicc startup
                $TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc startup

		#Run bip with client connected
                #modem simulator running bip tests
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
                echo pc_open_channel | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1

		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
                echo pc_send_data | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1

                echo pc_displaytext | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1

		#Send Event DownLoad Envelope
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat ec_bip &

		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
                echo pc_receive_data | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1

		#Re-run BIP cases by sending delayed response to display_text command
		#This will ensure that Caching of Event Data Download shall behave as expected
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
                echo pc_send_data_01 | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1

		#Send Event DownLoad Envelope
		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat ec_bip &

		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
                echo pc_receive_data | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1

		$TOP/vendor/st-ericsson/access_services/sim/sim/tcat/tcat pc &
                echo pc_close_channel | $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1

                #tuicc shutdown
                $TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc shutdown
        }

        function run_recovery {

                # Start simd/valgrind in the backround
                { valgrind $vgopts $TOP/vendor/st-ericsson/access_services/sim/sim/simd/simd -vvvv &>> $simd & }
                sleep 5

                #tuicc startup
                $TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc startup

                #Send Cat Ready to trigger Recovery
                echo send_cat_ready |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
                echo send_uicc_ready |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 5

                #Send Card Removed to trigger Recovery followed by Cat Ready for TP download
                echo send_card_removed |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
                echo send_cat_ready |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
                echo send_uicc_ready |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 5

                echo send_cat_not_ready |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
                echo send_cat_not_supported |  $TOP/vendor/st-ericsson/access_services/sim/sim_msl/modem_simulator/modem_simulator
                sleep 1
                #tuicc shutdown
                $TOP/vendor/st-ericsson/access_services/sim/sim/tuicc/tuicc shutdown
        }

#
# Main start here...
#

if [ "$1" == "all" ]; then
	runTests
fi

if [ "$1" == "tcat" ]; then
	run_tcat
	#Kill simd processes to secure that no other simd is running
	killall -v memcheck-x86-li

	sleep 5
	run_recovery
	#Kill simd processes to secure that no other simd is running
	killall -v memcheck-x86-li

	sleep 5
fi

if [ "$1" == "tcat_all_pc" ]; then
	run_tcat_all_pc
	#Kill simd processes to secure that no other simd is running
	killall -v memcheck-x86-li

	sleep 5

	#run BIP test cases
	run_bip
	#Kill simd processes to secure that no other simd is running
	killall -v memcheck-x86-li

	sleep 5
fi

if [ "$1" == "tuicc" ]; then
	run_tuicc
	#Kill simd processes to secure that no other simd is running
	killall -v memcheck-x86-li
	sleep 5

	run_tuicc_sim
	#Kill simd processes to secure that no other simd is running
	killall -v memcheck-x86-li
	sleep 5

	run_tuicc_fail_tests
	#Kill simd processes to secure that no other simd is running
	killall -v memcheck-x86-li
	sleep 5
fi

if [ "$1" == "tuicc_sim" ]; then
	run_tuicc_sim
	#Kill simd processes to secure that no other simd is running
	killall -v memcheck-x86-li
	sleep 5
fi

if [ "$1" == "tapdu" ]; then
	run_tapdu
	#Kill simd processes to secure that no other simd is running
	killall -v memcheck-x86-li

	sleep 5
fi

if [ "$1" == "tcat_client" ]; then
	run_tcat_client
	#Kill simd processes to secure that no other simd is running
	killall -v memcheck-x86-li

	sleep 5
fi
#!/bin/sh
( cd `dirname $0` && ../bin/ostdecoder -d COMBI/TEST_OST_2_host_arm.xml -d COMBI/TEST_OST_2_nmf_arm.xml -d COMBI/TEST_OST_2_nmf_dsp.xml -d COMBI/TEST_OST_2.xml -t test_ost_2_log_combiprobe_decoded.txt    COMBI/test_ost_2_log_combiprobe.bin )

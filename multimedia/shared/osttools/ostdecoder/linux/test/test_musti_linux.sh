#!/bin/sh
( cd `dirname $0` && ../bin/ostdecoder -d MUSTI/osttest_2_0x1_Dictionary.xml -d MUSTI/TEST_OST_2_host_arm.xml  -d MUSTI/TEST_OST_2_nmf_arm.xml   -d MUSTI/TEST_OST_2_nmf_dsp.xml   -d MUSTI/TEST_OST_2.xml  -t test_ost_2_musti_W1016_decoded.txt    MUSTI/test_ost_2_musti_W1016.bin )

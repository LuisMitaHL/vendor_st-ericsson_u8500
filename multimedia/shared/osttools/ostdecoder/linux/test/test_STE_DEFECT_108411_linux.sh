#!/bin/sh
( cd `dirname $0` && ../bin/ostdecoder -d STE_DEFECT_108411/ST_OSTTEST_COMMON_host_arm.xml -d STE_DEFECT_108411/ST_OSTTEST_COMMON_nmf_arm.xml -d STE_DEFECT_108411/ST_OSTTEST_COMMON_nmf_dsp.xml -d STE_DEFECT_108411/ST_OSTTEST_host_arm.xml -t ost_STE_DEFECT_108411_decoded.txt    STE_DEFECT_108411/ost_trace.bin )

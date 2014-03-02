#!/bin/sh
( cd `dirname $0` && ../bin/ostdecoder -h -a -D STE_DEFECT_114364/dicoList.txt STE_DEFECT_114364/cscall_capture_carbide.bin )

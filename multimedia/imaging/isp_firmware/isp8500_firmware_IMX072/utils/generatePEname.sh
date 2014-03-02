#!/bin/bash
tmpfile=$( mktemp )

cat ${4}/baseline.h | grep "Byte0\|//  firmware version:" >${tmpfile}
cat ${4}/extension.h | grep Byte0 >>${tmpfile}

../utils/PrintPEname.pl ${tmpfile} ${4} ${1}
../utils/PrintPEname.pl ${tmpfile} ${4} ${2}
rm -f ${tmpfile}

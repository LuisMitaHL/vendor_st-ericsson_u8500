#!/bin/bash

function cleanup {
rm -f ${tmpfile}
}

function help {
echo -e "$0: generate a C file and header containing data of a binary file

\tUsage: $0 file outfilename {primary|secondary}

\tWill produce files outfilename.c and outfile.h in the directory of the original 
\tfile. 'outfilename' is also used to internally name the variables.
\tThe .c contains the raw data and is to be compiled by your compiler, 
\tthe .h is a header to include in your program that refers to the data
\tfrom the C file"
}

if [ $# -lt 2 ]
then
  help
  exit
fi

trap "cleanup; exit" SIGINT

#TODO: have fun with tee and pipes (from mkfifo). I don't know how to join
#the output easily however...

tmpfile=$( mktemp )
outfileC="${3}/${2}.c"
outfileH="${4}/${2}.h"

fw_type=primary
fw_version=$( gawk '/\/\/  firmware version/ {print $4}' ${4}/baseline.h )
fw_capsname=$( echo ${2} | awk '{print toupper($0)}')

od -tx1 -An -w1 -v $1 > ${tmpfile}
size=$( cat ${tmpfile} | wc -w )

echo "
/*****************************************************************************/
/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/
/*****************************************************************************/

char ${2}_type[]=\"${fw_type}\";
char ${2}_version[]=\"${fw_version}\";
#ifdef __arm__arm__
unsigned __align(8) char EXPORT_C ${2}[] = 
#else
unsigned __attribute__ ((aligned (8))) char  ${2}[] = 
#endif
{" > ${outfileC}

cat ${tmpfile} | expand - -t 1 | tr -s ' ' | awk '{ printf "    0x%s,\n", $1 }' >> ${outfileC}
truncate -s-2 ${outfileC}
echo -e "\n};\n" >> ${outfileC}


echo "
/*****************************************************************************/
/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/
/*****************************************************************************/

#ifndef __${fw_capsname}_H
#define __${fw_capsname}_H

#ifdef __cplusplus
    extern \"C\" {
#endif

#define ${fw_capsname}_SIZE ${size}

extern char ${2}_type[];
extern char ${2}_version[];

extern unsigned char IMPORT_C ${2}[${fw_capsname}_SIZE];

#ifdef __cplusplus
    } /* extern C */
#endif

#endif /* __${fw_capsname}_H */
" > ${outfileH}

cleanup

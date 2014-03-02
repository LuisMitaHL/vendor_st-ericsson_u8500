#!/bin/sh -f

#internal audio tools
export MMPERLLIB=${MMBUILDOUT}/tools/audio/perllibs
export PERLLIB=${MMPERLLIB}:${PERLLIB}
export PATH=${MMBUILDOUT}/tools/audio/script:${PATH}
export PATH=${MMBUILDOUT}/tools/audio/bin/Linux:${PATH}
export PATH=${MMBUILDOUT}/tools/audio/afm/bin/Linux:${PATH}

if [ -e ${MMBUILDOUT}/tools/audio/ARM/sim/bin/sourceme.sh ] ; then
   source ${MMBUILDOUT}/tools/audio/ARM/sim/bin/sourceme.sh
 fi

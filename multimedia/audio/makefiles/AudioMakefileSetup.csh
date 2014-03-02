#internal audio tools
setenv MMPERLLIB ${MMBUILDOUT}/tools/audio/perllibs
setenv PERLLIB ${MMPERLLIB}:${PERLLIB}
setenv PATH ${MMBUILDOUT}/tools/audio/script:${PATH}
setenv PATH ${MMBUILDOUT}/tools/audio/bin/Linux:${PATH}
setenv PATH ${MMBUILDOUT}/tools/audio/afm/bin/Linux:${PATH}

if ( -e ${MMBUILDOUT}/tools/audio/ARM/sim/bin/sourceme ) then
   source ${MMBUILDOUT}/tools/audio/ARM/sim/bin/sourceme
endif

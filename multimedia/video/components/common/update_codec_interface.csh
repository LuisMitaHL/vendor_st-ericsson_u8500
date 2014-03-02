#!/bin/csh -f
echo $*

if (${MAKEFILES_BACKWARD_COMPATIBLE} == yes) then
  set dst="${MMROOT}/video/components/common/inc/${SVA_SUBSYSTEM}/t1xhv_host_interface.h"
  set src="${MMROOT}/video/components/$1/mpc/share/${SVA_SUBSYSTEM}/host_interface_$1.h"
  if ( ! -e ${src} ) then
	set src="${MMROOT}/video/components/$1/mpc/share/host_interface_$1.h"
  endif
  perl ${MMROOT}/video/components/common/update_interface.pl ${src} ${dst}
else
  set dst="${MMBUILDIN}/include/${PLATFORM}/video/${SVA_SUBSYSTEM}/t1xhv_host_interface.h"
  if ( ! -e ${dst} ) then
    echo
    echo "${dst} NOT present! You should install common component"
    echo
  endif

  set src="`pwd | sed -e 's/\/$1\/.*/\/$1/g'`"
  set src="${src}/mpc/share/${SVA_SUBSYSTEM}/host_interface_$1.h"
  if ( ! -e ${src} ) then
  #	set src="${MMROOT}/video/components/$1/mpc/share/host_interface_$1.h"
	set src="`echo ${src} | sed -e 's/${SVA_SUBSYSTEM}\///g'`"
        echo "Using src: ${src}"
  endif
  perl ${MMBUILDIN}/tools/video_common/update_interface.pl ${src} ${dst}
endif

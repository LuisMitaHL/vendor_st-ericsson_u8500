
#ifndef _cil_aac_info_h_
#define _cil_aac_info_h_

#include "audiolibs_types.h"

/* aac dec */
typedef struct {
    t_uint16      bitrate;  // Not available yet
    t_uint16      tns;  // Always set to 1
    t_uint16      objectType; /* LC/LTP */
    t_uint16      bsac_on;    /* don't use simultaneously sbr=1 and bsac_on=1 */
    t_uint16      sbr;
} t_dec_aac_info;


#endif


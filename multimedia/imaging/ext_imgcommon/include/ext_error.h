/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_ERROR_H
#define _EXT_ERROR_H

#include <cm/inc/cm_type.h>
#include "ext_hsmsig.h"
#include "host/ext_ispctl_types.idt.h"
#include "ext_resource_sharer_manager.h"

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CError);
#endif
class CError
{
    public:
        CError();
        ~CError();
		static const char * stringIspctlError(e_ispctlError index);
		static const char * stringIspctlInfo(e_ispctlInfo index);
		static const char * stringSMSig(e_scf_signal index);
		static const char * stringRSRequesterId(t_requesterID index);
};
#endif /*_EXT_ERROR_H*/


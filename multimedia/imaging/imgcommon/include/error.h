/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ERROR_H
#define _ERROR_H
#include "osi_trace.h"
#include <cm/inc/cm_type.h>
#include "hsmsig.h" //rename_me scf_signals.h
#include "host/ispctl_types.idt.h"
#include "resource_sharer_manager.h"

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
		static const char * stringISPStateid(t_isp_state index);
		static const char * stringSW3AStatusid(t_sw3a_stop_req_status index);
		static const char * stringGrabStatusid(t_grab_abort_status index);
		static const char * stringIspctlCtrlType(t_controling_isp_ctrl_type index);
		static const char * stringOMXstate(OMX_STATETYPE index);
};
#endif // _COMI2C_H



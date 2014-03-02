/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "hsm.h"



/*
* SetFeature
 */
SCF_STATE COM_SM::SetFeature(s_scf_event const *e)
{
    switch(e->sig)
    {
        case SCF_STATE_ENTRY_SIG : return 0;
        case SCF_STATE_EXIT_SIG : return 0;
        default: break;
    }
    return SCF_STATE_PTR(&COM_SM::Streaming);
}

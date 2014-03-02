/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <ee/scheduler.nmf>

t_ee_debug_level ATTR(printLevel) = EE_DEBUG_LEVEL_WARNING;

EXPORT_SHARED void NmfPrint0(t_ee_debug_level level, char* str) {
    if((int)ATTR(printLevel) >= (int)level)
    {
        NMF_LOG(str);
    }
}

EXPORT_SHARED void NmfPrint1(t_ee_debug_level level, char* str, t_uword value1) {
    if((int)ATTR(printLevel) >= (int)level)
    {
        NMF_LOG(str, value1);
    }
}

EXPORT_SHARED void NmfPrint2(t_ee_debug_level level, char* str, t_uword value1, t_uword value2) {
    if((int)ATTR(printLevel) >= (int)level)
    {
        NMF_LOG(str, value1, value2);
    }
}

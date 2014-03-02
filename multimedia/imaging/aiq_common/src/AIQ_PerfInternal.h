/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/

#pragma once

#include "AIQ_CommonTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

AIQ_Error AIQINT_InitPerfTrace         (char *fileName);
AIQ_Error AIQINT_StartPerfCounter      (AIQ_U8 id);
AIQ_Error AIQINT_StopPerfCounter       (AIQ_U8 id, char* description);
void      AIQINT_FlushPerfTrace        (void);
void      AIQINT_DeInitPerfTrace       (void);

#ifdef __cplusplus
}
#endif

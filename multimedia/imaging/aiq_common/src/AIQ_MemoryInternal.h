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

AIQ_Error AIQINT_InitMemTrace          (char *fileName);
void*     AIQINT_malloc                (AIQ_U32 size,AIQ_U32 identifier,char* description);
void*     AIQINT_calloc                (AIQ_U32 nobj,AIQ_U32 size,AIQ_U32 identifier,char* description);
void      AIQINT_free                  (void* address);
void      AIQINT_MemTraceInsertComment (char* comment);
void      AIQINT_MemTraceStatus        (char* comment);
void      AIQINT_DeInitMemTrace        (void);

#ifdef __cplusplus
}
#endif

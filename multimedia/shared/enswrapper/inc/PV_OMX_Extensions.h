/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* ------------------------------------------------------------------
 * Copyright (C) 1998-2009 PacketVideo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */

/** This file is just a copy of the PV OMX extension to be able to print them in the wrapper log  */

#ifndef _PV_OMX_EXTENSIONS_H_
#define _PV_OMX_EXTENSIONS_H_

#include <OMX_Types.h>

typedef enum PV_OMX_INDEXTYPE {
    OMX_IndexParamComponentCapabilityFlags = 0xFF7A347, /**< reference: OMX_PVComponentCapabilityFlagsType */
} PV_OMX_INDEXTYPE;

/* Changes structure name to avoid conflict with the ENS one not correctly defining the index */
typedef struct OMX_PVComponentCapabilityFlagsType
{
    OMX_BOOL iIsOMXComponentMultiThreaded;
    OMX_BOOL iOMXComponentSupportsExternalOutputBufferAlloc;
    OMX_BOOL iOMXComponentSupportsExternalInputBufferAlloc;
    OMX_BOOL iOMXComponentSupportsMovableInputBuffers;
    OMX_BOOL iOMXComponentSupportsPartialFrames;
    OMX_BOOL iOMXComponentUsesNALStartCodes;
    OMX_BOOL iOMXComponentCanHandleIncompleteFrames;
    OMX_BOOL iOMXComponentUsesFullAVCFrames;

} OMX_PVComponentCapabilityFlagsType;

#endif /* _PV_OMX_EXTENSIONS_H_ */



/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AudioEffectMpcUtils.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "AudioEffectMpcUtils.h"

t_cm_memory_handle AudioEffectMpcUtils::allocDSPMemoryForSetConfig(OMX_U32 domainId, OMX_U32 size, OMX_U32 *configStructArmAddress, OMX_U32 *configStructDspAddress)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    t_cm_memory_handle memoryHandle = 0;
    t_cm_system_address systemAddress;
    OMX_U32 dspAddress = 0;
    t_cm_error cmErr;

    //Important to allocate this structure in SDRAM24.
    //All structures passed from ARM to DSP for setConfig are written on ARM side in 24bits mode
    //to be easily read by MPC.
    error = ENS::allocMpcMemory(domainId, CM_MM_MPC_SDRAM24, size/4, CM_MM_ALIGN_2WORDS, &memoryHandle);
    if(error != OMX_ErrorNone)  return 0;

    cmErr = CM_GetMpcMemorySystemAddress(memoryHandle, &systemAddress);
    if(cmErr != CM_OK)
    {
        ENS::freeMpcMemory(memoryHandle);
        return 0;
    }

    cmErr = CM_GetMpcMemoryMpcAddress(memoryHandle, &dspAddress);
    if(cmErr != CM_OK)
    {
        ENS::freeMpcMemory(memoryHandle);
        return 0;
    }

    *configStructArmAddress = systemAddress.logical;
    *configStructDspAddress = dspAddress;

    return memoryHandle;
}


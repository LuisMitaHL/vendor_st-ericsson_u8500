/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_nmf.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_Nmf.h"
#include "AFM_nmf.h"
#include "ENS_String.h"
#include "cm/inc/cm_macros.h"

AFM_API_EXPORT OMX_ERRORTYPE
AFM::connectPorts(
        t_cm_instance_handle hNmfOut, 
        const char * outSuffix,
        t_cm_instance_handle hNmfIn,
        const char * inSuffix,
        bool pcm) 
{
    OMX_ERRORTYPE error;

    DBC_ASSERT(hNmfOut && hNmfIn);
    DBC_ASSERT(outSuffix && inSuffix);

    ENS_String<20> outputport("outputport");
    ENS_String<20> fillthisbuffer("fillthisbuffer");
    ENS_String<20> outputsettings("outputsettings");

    outputport.concat(outSuffix);
    fillthisbuffer.concat(outSuffix);
    outputsettings.concat(outSuffix);

    ENS_String<20> emptythisbuffer("emptythisbuffer");
    ENS_String<20> inputport("inputport");
    ENS_String<20> pcmsettings("pcmsettings");

    emptythisbuffer.concat(inSuffix);
    inputport.concat(inSuffix);
    pcmsettings.concat(inSuffix);


    // Get component description
    t_cm_error errorBis;
    t_nmf_core_id coreIdIn;
    char templateNameIn[256];
    char localNameIn[256];
    t_nmf_ee_priority priorityIn;
    
    t_nmf_core_id coreIdOut;
    char templateNameOut[256];
    char localNameOut[256];
    t_nmf_ee_priority priorityOut;

    errorBis = CM_GetComponentDescription(hNmfOut, templateNameOut, 256, &coreIdOut, localNameOut, 256, &priorityOut);
    if(errorBis != CM_OK){
        return OMX_ErrorUndefined;
    }
    
    errorBis = CM_GetComponentDescription(hNmfIn, templateNameIn, 256, &coreIdIn, localNameIn, 256, &priorityIn);
    if(errorBis != CM_OK){
        return OMX_ErrorUndefined;
    }

    error = ENS::bindComponent(
        hNmfOut, outputport, hNmfIn, emptythisbuffer);
    if (error != OMX_ErrorNone) return error;
    
    error = ENS::bindComponent(
        hNmfIn, inputport, hNmfOut, fillthisbuffer);
    if (error != OMX_ErrorNone) return error;

    if (pcm) {
        error = ENS::bindComponentAsynchronousEx(
                hNmfOut, outputsettings, hNmfIn, pcmsettings, 1);
        if (error != OMX_ErrorNone) return error;
    }

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE
AFM::disconnectPorts(
        t_cm_instance_handle hNmfOut, 
        const char * outSuffix,
        t_cm_instance_handle hNmfIn,
        const char * inSuffix,
        bool pcm) 
{
    OMX_ERRORTYPE error;

    DBC_ASSERT(hNmfOut && hNmfIn);
    //coverity[forward_null]
    DBC_ASSERT(outSuffix && inSuffix);

    ENS_String<20> outputport("outputport");
    ENS_String<20> fillthisbuffer("fillthisbuffer");
    ENS_String<20> outputsettings("outputsettings");

    //coverity[forward_null]
    outputport.concat(outSuffix);
    fillthisbuffer.concat(outSuffix);
    outputsettings.concat(outSuffix);

    ENS_String<20> emptythisbuffer("emptythisbuffer");
    ENS_String<20> inputport("inputport");
    ENS_String<20> pcmsettings("pcmsettings");

    //coverity[forward_null]
    emptythisbuffer.concat(inSuffix);
    inputport.concat(inSuffix);
    pcmsettings.concat(inSuffix);

     // Get component description
    t_cm_error errorBis;
    t_nmf_core_id coreIdIn;
    char templateNameIn[256];
    char localNameIn[256];
    t_nmf_ee_priority priorityIn;
    
    t_nmf_core_id coreIdOut;
    char templateNameOut[256];
    char localNameOut[256];
    t_nmf_ee_priority priorityOut;

    errorBis = CM_GetComponentDescription(hNmfOut, templateNameOut, 256, &coreIdOut, localNameOut, 256, &priorityOut);
    if(errorBis != CM_OK){
        return OMX_ErrorUndefined;
    }
    
    errorBis = CM_GetComponentDescription(hNmfIn, templateNameIn, 256, &coreIdIn, localNameIn, 256, &priorityIn);
    if(errorBis != CM_OK){
        return OMX_ErrorUndefined;
    }

    error = ENS::unbindComponent(
        hNmfOut, outputport);
    if (error != OMX_ErrorNone) return error;
      
    error = ENS::unbindComponent(
        hNmfIn, inputport);
    if (error != OMX_ErrorNone) return error;

    if (pcm) {
        error = ENS::unbindComponentAsynchronous(
                hNmfOut, outputsettings);
        if (error != OMX_ErrorNone) return error;
    }

    return OMX_ErrorNone;
}

#ifndef HOST_ONLY
AFM_API_EXPORT t_cm_mpc_memory_type
AFM::memoryBank(t_memory_bank bank)
{
    switch(bank)
    {
        case MEM_XTCM:
            return CM_MM_MPC_TCM24_X;
        case MEM_YTCM:
            return CM_MM_MPC_TCM24_Y;
        case MEM_DDR24:
            return CM_MM_MPC_SDRAM24 ;
        case MEM_DDR16:
            return CM_MM_MPC_SDRAM16;
        case MEM_ESR24:
            return CM_MM_MPC_ESRAM24;
        case MEM_ESR16:
            return CM_MM_MPC_ESRAM16;
        default:
            DBC_ASSERT(0);
            return CM_MM_MPC_TCM24;
    }
}
#endif


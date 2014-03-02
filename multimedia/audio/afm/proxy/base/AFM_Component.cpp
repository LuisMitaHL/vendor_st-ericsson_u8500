/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_Component.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_String.h"
#include "ENS_Component_Fsm.h"
#include "AFM_Component.h"
#include "AFM_Index.h"
#include "AFM_Core.h"
#include "audio_chipset_api_index.h"
#include "linux_utils.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_base_AFM_ComponentTraces.h"
#endif

AFM_Component_RDB::AFM_Component_RDB(OMX_U32 nbOfDomains)
    : ENS_ResourcesDB(nbOfDomains)
{
    for (unsigned int i=0;i<nbOfDomains;i++) {
        setDefaultNMFDomainType(RM_NMFD_PROCSIA,i);
    }

}


AFM_API_EXPORT OMX_ERRORTYPE
AFM_Component::createResourcesDB() {
    mRMP = new AFM_Component_RDB(getPortCount());
    if (mRMP == 0) {
        return OMX_ErrorInsufficientResources;
    } else {
        return OMX_ErrorNone;
    }
}

AFM_API_EXPORT OMX_ERRORTYPE AFM_Component::construct(OMX_U32 nPortCount, OMX_U32 nRoleCount, bool ArmOnly)
{

    mProbeConfig = new AFM_AUDIO_CONFIG_PCM_PROBE[nPortCount];
    memset(mProbeConfig, 0, sizeof(AFM_AUDIO_CONFIG_PCM_PROBE) * nPortCount);
    return ENS_Component::construct(nPortCount, nRoleCount, ArmOnly);
}

AFM_API_EXPORT AFM_Component::~AFM_Component() {

    delete[] mProbeConfig;
}

AFM_API_EXPORT OMX_ERRORTYPE AFM_Component::getExtensionIndex(
        OMX_STRING cParameterName,
        OMX_INDEXTYPE* pIndexType) const
{
    if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
            == "OMX.ST.AFM.proprietary_communication") {
        *pIndexType = (OMX_INDEXTYPE)AFM_IndexParamProprietaryCommunication;
        return OMX_ErrorNone;
    }

    return ENS_Component::getExtensionIndex(cParameterName, pIndexType);
}


#ifndef BC_BREAK
AFM_API_EXPORT OMX_ERRORTYPE AFM_Component::createPcmPort(
        OMX_U32 nIndex,
        OMX_DIRTYPE eDir,
        OMX_U32 nBufferSizeMin,
        const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings)
{
    ENS_Port * port = new AFM_PcmPort(
                nIndex, eDir, nBufferSizeMin, defaultPcmSettings, *this);
    if (port == 0) {
        return OMX_ErrorInsufficientResources;
    }

    addPort(port);

    return OMX_ErrorNone;
}
#endif


AFM_API_EXPORT OMX_BOOL AFM_Component::isPortSpecificParameter(
        OMX_INDEXTYPE nParamIndex) const
{
    switch(nParamIndex){
        case AFM_IndexParamHostMpcSync:
        case AFM_IndexParamForceStandardTunneling:
        case AFM_IndexParamProprietaryCommunication:
        case AFM_IndexParamBindingMemoryType:
        case OMX_IndexParamAudioPortFormat:
        case OMX_IndexParamAudioPcm:
        case OMX_IndexParamAudioAac:
        case OMX_IndexParamAudioRa:
        case OMX_IndexParamAudioMp3:
        case OMX_IndexParamAudioAdpcm:
        case OMX_IndexParamAudioG723:
        case OMX_IndexParamAudioG729:
        case OMX_IndexParamAudioAmr:
        case OMX_IndexParamAudioWma:
        case OMX_IndexParamAudioSbc:
        case OMX_IndexParamAudioMidi:
        case OMX_IndexParamAudioGsm_FR:
        case OMX_IndexParamAudioMidiLoadUserSound:
        case OMX_IndexParamAudioG726:
        case OMX_IndexParamAudioGsm_EFR:
        case OMX_IndexParamAudioGsm_HR:
        case OMX_IndexParamAudioPdc_FR:
        case OMX_IndexParamAudioPdc_EFR:
        case OMX_IndexParamAudioPdc_HR:
        case OMX_IndexParamAudioTdma_FR:
        case OMX_IndexParamAudioTdma_EFR:
        case OMX_IndexParamAudioQcelp8:
        case OMX_IndexParamAudioQcelp13:
        case OMX_IndexParamAudioEvrc:
        case OMX_IndexParamAudioSmv:
        case OMX_IndexParamAudioVorbis:
        case OMX_IndexParamAudioSynchronized:
        case OMX_IndexParamAudioG711:
        case OMX_IndexParamAudio3DDopplerMode:
        case OMX_IndexParamAudioDDplus:
        case OMX_IndexParamAudioDTS:
        case OMX_IndexParamAudioAC3:
        case OMX_IndexParamAudioFLAC:
        case OMX_IndexParamAudioWMAPro:
           return OMX_TRUE;
        default:
            return ENS_Component::isPortSpecificParameter(nParamIndex);
    }
}

AFM_API_EXPORT OMX_ERRORTYPE AFM_Component::setParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure)
{
    switch (nParamIndex) {
        case OMX_IndexParamLinePriority:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_LINEPRIORITYTYPE);
                OMX_PARAM_LINEPRIORITYTYPE* linePriorityType = (OMX_PARAM_LINEPRIORITYTYPE*)pComponentParameterStructure;

                if ((linePriorityType->ePortPriority < OMX_PriorityBackground) || (linePriorityType->ePortPriority > OMX_PriorityTelephonyUL_2))
                {
                    return OMX_ErrorBadParameter;
                }

                if (prioritiesSetForAllPorts() && (linePriorityType->nPortIndex != OMX_ALL)) {
                    return OMX_ErrorBadParameter;
                }

                if (linePriorityType->nPortIndex == OMX_ALL) {
                    for (unsigned int i=0; i<getPortCount(); i++) {
                        (static_cast<AFM_Port *>(getPort(i)))->setPortPriorityLevel(linePriorityType->ePortPriority);
                    }
                } else {
                    if (linePriorityType->nPortIndex < getPortCount()) {
                        (static_cast<AFM_Port *>(getPort(linePriorityType->nPortIndex)))->setPortPriorityLevel(linePriorityType->ePortPriority);
                    }
                    else {
                        OstTraceFiltInst1(TRACE_ERROR, "ERROR : bad port index (0x%x) while setting parameter through index OMX_IndexParamLinePriority",linePriorityType->nPortIndex);
                        return OMX_ErrorBadParameter;
                    }
                }
                return OMX_ErrorNone;
            }
        default:
            return ENS_Component::setParameter(nParamIndex, pComponentParameterStructure);
    }
}

AFM_API_EXPORT OMX_ERRORTYPE AFM_Component::getParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure) const
{
    switch (nParamIndex) {
        case OMX_IndexParamLinePriority:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_LINEPRIORITYTYPE);
                OMX_PARAM_LINEPRIORITYTYPE* linePriorityType = (OMX_PARAM_LINEPRIORITYTYPE*)pComponentParameterStructure;

                if (!prioritiesSetForAllPorts() && (linePriorityType->nPortIndex == OMX_ALL)) {
                    return OMX_ErrorBadParameter;
                }

                if (linePriorityType->nPortIndex == OMX_ALL) {
                    linePriorityType->ePortPriority = (static_cast<const AFM_Port *>(getPort(0)))->getPortPriorityLevel();
                } else {
                    if (linePriorityType->nPortIndex < getPortCount()) {
                        linePriorityType->ePortPriority = (static_cast<const AFM_Port *>(getPort(linePriorityType->nPortIndex)))->getPortPriorityLevel();
                    }
                    else {
                        OstTraceFiltInst1(TRACE_ERROR, "ERROR : bad port index (0x%x) while getting parameter through index OMX_IndexParamLinePriority",linePriorityType->nPortIndex);
                        return OMX_ErrorBadParameter;
                    }
                }
                return OMX_ErrorNone;
            }
        default:
            return ENS_Component::getParameter(nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AFM_Component::setConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
    if ((nConfigIndex == (OMX_INDEXTYPE) AFM_IndexConfigPcmProbe))
    {
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,
                                      AFM_AUDIO_CONFIG_PCM_PROBE);

        return setProbeConfig(static_cast<AFM_AUDIO_CONFIG_PCM_PROBE *>(pComponentConfigStructure));
    }
    else
    {
        return ENS_Component::setConfig(nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE AFM_Component::getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const
{
    if ((nConfigIndex == (OMX_INDEXTYPE) AFM_IndexConfigPcmProbe))
    {
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,
                                      AFM_AUDIO_CONFIG_PCM_PROBE);

        AFM_AUDIO_CONFIG_PCM_PROBE * pConfig = static_cast<AFM_AUDIO_CONFIG_PCM_PROBE *>(pComponentConfigStructure);
        AFM_AUDIO_CONFIG_PCM_PROBE * probe = getProbeConfig(pConfig->nPortIndex);

        if (probe == NULL) return OMX_ErrorBadParameter;

        pConfig->bEnable = probe->bEnable;
        pConfig->nProbeIdx = probe->nProbeIdx;
        return OMX_ErrorNone;
    }
    else
    {
        return ENS_Component::getConfig(nConfigIndex, pComponentConfigStructure);
    }
}


OMX_ERRORTYPE AFM_Component::setProbeConfig(AFM_AUDIO_CONFIG_PCM_PROBE *pConfig)
{
    OMX_U32 portIdx = pConfig->nPortIndex;

    AFM_AUDIO_CONFIG_PCM_PROBE * probe = getProbeConfig(portIdx);

    if (probe == NULL) return OMX_ErrorBadParameter;

    *probe = *pConfig;

    return OMX_ErrorNone;
}


AFM_AUDIO_CONFIG_PCM_PROBE * AFM_Component::getProbeConfig(OMX_U32 portIdx) const
{
    if (portIdx >= getPortCount())
    {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_Component::getProbeConfig; port index is out of bounds: (%d vs %d)\n", (unsigned int)portIdx, (unsigned int)getPortCount());
        return NULL;
    }
    else
    {
        return &mProbeConfig[portIdx];
    }
}

ENS_API_EXPORT void AFM_Component::NmfPanicCallback(
        void *contextHandler,
        t_nmf_service_type serviceType,
        t_nmf_service_data *serviceData)
{

    if(serviceType==NMF_SERVICE_SHUTDOWN)
    {
        ALOGV("NMF_SERVICE_SHUTDOWN CORE ID: %d\n", serviceData->shutdown.coreid);
    }
    else
    {
        const char* reason = NULL;
        char unknown_reason[80];

        switch ((t_panic_reasonDescription)serviceData->panic.panicReason)
        {
            case INTERNAL_PANIC:           reason = "INTERNAL_PANIC";        break;
            case MPC_NOT_RESPONDING_PANIC: reason = "MPC_NOT_RESPONDING_PANIC"; break;
            case USER_STACK_OVERFLOW:      reason = "USER_STACK_OVERFLOW";   break;
            case SYSTEM_STACK_OVERFLOW:    reason = "SYSTEM_STACK_OVERFLOW"; break;
            case UNALIGNED_LONG_ACCESS:    reason = "UNALIGNED_LONG_ACCESS"; break;
            case EVENT_FIFO_OVERFLOW:      reason = "EVENT_FIFO_OVERFLOW";   break;
            case PARAM_FIFO_OVERFLOW:      reason = "PARAM_FIFO_OVERFLOW";   break;
            case INTERFACE_NOT_BINDED:     reason = "INTERFACE_NOT_BINDED";  break;
            case USER_PANIC:               reason = "USER_PANIC";            break;
            case UNBIND_INTERRUPT:         reason = "UNBIND_INTERRUPT";      break;
            case EVENT_FIFO_IN_USE:        reason = "EVENT_FIFO_IN_USE";     break;
            default:
            {
                sprintf(unknown_reason, "<unknown val 0x%X>", serviceData->panic.panicReason);
                reason = unknown_reason;
                break;
            }
        }

        if (serviceData->panic.panicSource == (t_panic_source) MPC_EE)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_Component::NmfPanicCallback DSP Panic! Reason=%d, CoreID=%d",serviceData->panic.panicReason,serviceData->panic.info.mpc.coreid);
            OstTraceFiltInst3(TRACE_ERROR, "AFM_Component::NmfPanicCallback DSP Panic! Handle=%X, Info1=%X, Info2=%X",(unsigned int)serviceData->panic.info.mpc.faultingComponent,(unsigned int)serviceData->panic.info.mpc.panicInfo1,(unsigned int)serviceData->panic.info.mpc.panicInfo2);


            ALOGE("DSP Panic! Reason=%s, CoreID=%d, Handle=%X, Info1=%X, Info2=%X\n",
                 reason,
                 serviceData->panic.info.mpc.coreid,
                 (unsigned int)serviceData->panic.info.mpc.faultingComponent,
                 (unsigned int)serviceData->panic.info.mpc.panicInfo1,
                 (unsigned int)serviceData->panic.info.mpc.panicInfo2);

#ifndef AFM_X86

            char templatename[256] = {0};
            char localname[256] = {0};

            t_nmf_core_id coreid;
            t_nmf_ee_priority prio;
#ifdef STE_PLATFORM_U5500
            t_cm_error res = CM_OK;
#else
            // Get name of faulting component from CM
            t_cm_error res = CM_GetComponentDescription(
                                    serviceData->panic.info.mpc.faultingComponent,
                                    templatename,
                                    sizeof(templatename),
                                    &coreid,
                                    localname,
                                    sizeof(localname),
                                    &prio);

#endif
            if (res == CM_OK) {
                ALOGE("DSP Panic: localName = '%s', templateName = '%s' prio = %d\n", localname, templatename, (int)prio);
            }

            if (serviceData->panic.info.mpc.coreid == SIA_CORE_ID) {
                eventHandlerCB(OMX_EventError, AFM_ErrorDspPanic, 0, (OMX_PTR) serviceData);
            }

#endif

        }
        else
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_Component::NmfPanicCallback NMF EE Panic! Reason=%d, CoreID=%d",serviceData->panic.panicReason,serviceData->panic.info.mpc.coreid);
            OstTraceFiltInst3(TRACE_ERROR, "AFM_Component::NmfPanicCallback NMF EE Panic! Handle=%X, Info1=%X, Info2=%X",(unsigned int)serviceData->panic.info.mpc.faultingComponent,(unsigned int)serviceData->panic.info.mpc.panicInfo1,(unsigned int)serviceData->panic.info.mpc.panicInfo2);

            ALOGE("NMF EE Panic! Reason=%s, Handle=%X, Info1=%X, Info2=%X\n",
                 reason,
                 (unsigned int)serviceData->panic.info.host.faultingComponent,
                 (unsigned int)serviceData->panic.info.host.panicInfo1,
                 (unsigned int)serviceData->panic.info.host.panicInfo2);
        }
    }
}

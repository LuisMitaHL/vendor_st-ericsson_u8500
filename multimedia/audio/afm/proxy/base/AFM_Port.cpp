/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_Port.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_Component.h"
#include "ENS_Port_Fsm.h"
#include "AFM_Port.h"
#include "AFM_Types.h"
#include "AFM_Index.h"

AFM_API_EXPORT AFM_Port::AFM_Port(
        OMX_U32 nIndex,
        OMX_DIRTYPE eDir,
        OMX_U32 nBufferSizeMin,
        const OMX_AUDIO_CODINGTYPE eEncoding,
        ENS_Component &enscomp) 	 
    : ENS_Port(
            EnsCommonPortData(
                    nIndex, 
                    eDir, 
                    2, 
                    nBufferSizeMin, 
                    OMX_PortDomainAudio, 
                    eDir == OMX_DirInput ? 
                                OMX_BufferSupplyInput : OMX_BufferSupplyOutput
            ), 
            enscomp
        )
{ 	 
    mParamPortDefinition.format.audio.cMIMEType = 0;
    mParamPortDefinition.format.audio.pNativeRender = 0;
    mParamPortDefinition.format.audio.eEncoding = eEncoding; 	
    mParamPortDefinition.format.audio.bFlagErrorConcealment = OMX_FALSE;

    mNbFormatSupported = 1;
    mEncodingTable = new OMX_AUDIO_CODINGTYPE[1];
    mEncodingTable[0] = eEncoding;
    mProprietaryCommInUse     = AFM_NO_PROPRIETARY_COMMUNICATION;
    mSupportedProprietaryComm = AFM_MPC_PROPRIETARY_COMMUNICATION;
    mPortPriorityLevel = OMX_PriorityNormal;
    mHostMpcSyncEnabled = OMX_FALSE;
    mBindingMemoryType = AFM_MEMORY_TYPE_SDRAM;
    setMpc(false);
}

AFM_API_EXPORT AFM_Port::AFM_Port(
        OMX_U32 nIndex,
        OMX_DIRTYPE eDir,
        OMX_U32 nBufferSizeMin,
        OMX_U32 nbFormatSupported,
        const OMX_AUDIO_CODINGTYPE *pEncodingTable,
        ENS_Component &enscomp)	 
    : ENS_Port(
            EnsCommonPortData(
                    nIndex, 
                    eDir, 
                    2, 
                    nBufferSizeMin, 
                    OMX_PortDomainAudio, 
                    eDir == OMX_DirInput ? 
                                OMX_BufferSupplyInput : OMX_BufferSupplyOutput
            ), 
            enscomp
        )
{ 	 
    mParamPortDefinition.format.audio.eEncoding = pEncodingTable[0]; 	
    mParamPortDefinition.format.audio.bFlagErrorConcealment = OMX_FALSE;

    DBC_ASSERT(nbFormatSupported != 0);
    mNbFormatSupported = nbFormatSupported;
    DBC_ASSERT(pEncodingTable != 0);
    mEncodingTable = new OMX_AUDIO_CODINGTYPE[nbFormatSupported];
    for(OMX_U32 i=0; i<mNbFormatSupported; i++) {
        mEncodingTable[i] = pEncodingTable[i];
    }

    mProprietaryCommInUse = AFM_NO_PROPRIETARY_COMMUNICATION;
    mSupportedProprietaryComm = AFM_MPC_PROPRIETARY_COMMUNICATION;
    mPortPriorityLevel = OMX_PriorityNormal;
    mHostMpcSyncEnabled = OMX_FALSE;
    mBindingMemoryType = AFM_MEMORY_TYPE_SDRAM;
    setMpc(false);
}

AFM_API_EXPORT AFM_Port::~AFM_Port(void) {
    delete [] mEncodingTable;
}

AFM_API_EXPORT OMX_ERRORTYPE AFM_Port::setFormatInPortDefinition(
            const OMX_PARAM_PORTDEFINITIONTYPE& portDef)
{
    mParamPortDefinition.format.audio.bFlagErrorConcealment 
        = portDef.format.audio.bFlagErrorConcealment;

    mParamPortDefinition.nBufferSize = portDef.nBufferSize;

    if(portDef.format.audio.eEncoding 
            != mParamPortDefinition.format.audio.eEncoding){
        return OMX_ErrorBadParameter;
    }

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE AFM_Port::proprietaryCommunicationRequest(
        OMX_HANDLETYPE hTunneledComp,
        OMX_U32 nTunneledPort) 
{ 
    OMX_ERRORTYPE err;
    OMX_INDEXTYPE index;
    
    // check that tunneled component supports AFM proprietary communication
    err = OMX_GetExtensionIndex(
             hTunneledComp, (char*) "OMX.ST.AFM.proprietary_communication",  &index);
    if (err != OMX_ErrorNone) { return OMX_ErrorNotImplemented; }
    
    // retrieve which type of AFM proprietary communication is supported
    AFM_PARAM_PROPRIETARY_COMMUNICATION config;
    config.nSize = sizeof(AFM_PARAM_PROPRIETARY_COMMUNICATION);
    getOmxIlSpecVersion(&config.nVersion);
    config.nPortIndex = nTunneledPort;

    err = OMX_GetParameter(hTunneledComp, index , &config);
    if (err != OMX_ErrorNone) { return OMX_ErrorNotImplemented; }

    if (config.eCommType == AFM_NO_PROPRIETARY_COMMUNICATION) return OMX_ErrorNotImplemented;
    
    // If both port support same proprietary comm (MPC or HOST),
    // tell tunneled component whether it is Hybrid or not
    if (((config.eCommType & AFM_MPC_PROPRIETARY_COMMUNICATION) &&
	 (mSupportedProprietaryComm & AFM_MPC_PROPRIETARY_COMMUNICATION))
	||
	((config.eCommType & AFM_HOST_PROPRIETARY_COMMUNICATION) &&
	 (mSupportedProprietaryComm & AFM_HOST_PROPRIETARY_COMMUNICATION)))
    {
      mProprietaryCommInUse = config.eCommType;
      config.eCommType = mSupportedProprietaryComm;
      err = OMX_SetParameter(hTunneledComp, index, &config);
      if (err != OMX_ErrorNone) { return OMX_ErrorNotImplemented; }
    }
    else
    {
      return OMX_ErrorNotImplemented;
    }
    
    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE 
AFM_Port::checkCompatibility(
        OMX_HANDLETYPE hTunneledComp, 
        OMX_U32 nTunneledPort) const
{

    OMX_ERRORTYPE err;
    OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef, myPortDef;

    tunneledPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    tunneledPortDef.nPortIndex = nTunneledPort;
	getOmxIlSpecVersion(&tunneledPortDef.nVersion);

    err = OMX_GetParameter(
            hTunneledComp, OMX_IndexParamPortDefinition, &tunneledPortDef);
    if (err != OMX_ErrorNone)  return err;

    myPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&myPortDef.nVersion);
    err = getParameter(OMX_IndexParamPortDefinition, &myPortDef);
    if (err != OMX_ErrorNone)  return err;

    if (myPortDef.eDir == tunneledPortDef.eDir) {
        return OMX_ErrorPortsNotCompatible;
    }

    if (myPortDef.eDomain != tunneledPortDef.eDomain) {
        return OMX_ErrorPortsNotCompatible;
    }

    if (myPortDef.eDomain == OMX_PortDomainAudio) {
        if (myPortDef.format.audio.eEncoding 
                != tunneledPortDef.format.audio.eEncoding) {
            return OMX_ErrorPortsNotCompatible;
        }
    } else {
        DBC_ASSERT(0);
    }


    return checkFormatCompatibility(hTunneledComp, nTunneledPort);
}

AFM_API_EXPORT OMX_PORT_PRIORITY AFM_Port::getPortPriorityLevel() const {
    //FIXME: Maybe the priorities set are not correct, and
    if (isMpc()) {
        switch (mPortPriorityLevel) {
            case OMX_PriorityBackground: return (OMX_PORT_PRIORITY)NMF_SCHED_BACKGROUND;
            case OMX_PriorityNormal: return (OMX_PORT_PRIORITY)NMF_SCHED_NORMAL;
            case OMX_PriorityUrgent: return (OMX_PORT_PRIORITY)NMF_SCHED_URGENT;
            case OMX_PriorityTelephonyDL: return (OMX_PORT_PRIORITY)NMF_SCHED_URGENT;
            case OMX_PriorityTelephonyUL: return (OMX_PORT_PRIORITY)NMF_SCHED_URGENT;
            case OMX_PriorityTelephonyUL_2: return (OMX_PORT_PRIORITY)NMF_SCHED_URGENT;
			default: return (OMX_PORT_PRIORITY)NMF_SCHED_NORMAL;
        }
    }
    return (OMX_PORT_PRIORITY)(mPortPriorityLevel);
}

AFM_API_EXPORT OMX_BOOL AFM_Port::getFlagErrorConcealment(void)
{
    return mParamPortDefinition.format.audio.bFlagErrorConcealment;    
}

AFM_API_EXPORT OMX_ERRORTYPE AFM_Port::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) 
{
    switch (nParamIndex) {
        case OMX_IndexParamAudioPortFormat:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_PORTFORMATTYPE);
                OMX_AUDIO_PARAM_PORTFORMATTYPE *portformat = 
                    (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;

                for( OMX_U32 i = 0; i < mNbFormatSupported; i++ ){
                    if(portformat->eEncoding == mEncodingTable[i]){
                        mParamPortDefinition.format.audio.eEncoding = portformat->eEncoding;
                        return OMX_ErrorNone;
                    }
                }

                return OMX_ErrorBadParameter;
            }
        case AFM_IndexParamProprietaryCommunication:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,AFM_PARAM_PROPRIETARY_COMMUNICATION);
                AFM_PARAM_PROPRIETARY_COMMUNICATION *proprietaryCom = (AFM_PARAM_PROPRIETARY_COMMUNICATION *)pComponentParameterStructure;

                mProprietaryCommInUse = proprietaryCom->eCommType;
                return OMX_ErrorNone;
            }

        case AFM_IndexParamHostMpcSync:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,AFM_PARAM_HOST_MPC_SYNC_TYPE);
                AFM_PARAM_HOST_MPC_SYNC_TYPE* hostSync = (AFM_PARAM_HOST_MPC_SYNC_TYPE*)pComponentParameterStructure;
                mHostMpcSyncEnabled = hostSync->enabled;
                return OMX_ErrorNone;
            }

        case AFM_IndexParamBindingMemoryType:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,AFM_PARAM_BINDING_MEMORY_TYPE);
                AFM_PARAM_BINDING_MEMORY_TYPE* param = (AFM_PARAM_BINDING_MEMORY_TYPE*)pComponentParameterStructure;

                switch (param->memoryType) 
                {
                    case AFM_MEMORY_TYPE_SDRAM:
                    case AFM_MEMORY_TYPE_ESRAM:
                    case AFM_MEMORY_TYPE_TCMX:
                    case AFM_MEMORY_TYPE_TCMY:
                        break;

                    default:
                        return OMX_ErrorBadParameter;
                }

                mBindingMemoryType = param->memoryType;
                return OMX_ErrorNone;
            }

        default:
            return ENS_Port::setParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

AFM_API_EXPORT OMX_ERRORTYPE AFM_Port::getParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) const 
{
    switch (nParamIndex) {
        case OMX_IndexParamAudioPortFormat:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_PORTFORMATTYPE);
                OMX_AUDIO_PARAM_PORTFORMATTYPE *portformat = 
                    (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;

                if(portformat->nIndex > mNbFormatSupported - 1) return OMX_ErrorNoMore;

                portformat->eEncoding = mEncodingTable[portformat->nIndex];

                return OMX_ErrorNone;
            }
        case AFM_IndexParamProprietaryCommunication:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,AFM_PARAM_PROPRIETARY_COMMUNICATION);
                AFM_PARAM_PROPRIETARY_COMMUNICATION *proprietaryCom = (AFM_PARAM_PROPRIETARY_COMMUNICATION *)pComponentParameterStructure;

                proprietaryCom->eCommType = mSupportedProprietaryComm;
                return OMX_ErrorNone;
            }

        case AFM_IndexParamHostMpcSync:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,AFM_PARAM_HOST_MPC_SYNC_TYPE);
                AFM_PARAM_HOST_MPC_SYNC_TYPE* hostSync = (AFM_PARAM_HOST_MPC_SYNC_TYPE*)pComponentParameterStructure;
                hostSync->enabled = mHostMpcSyncEnabled;
                return OMX_ErrorNone;
            }

        case AFM_IndexParamBindingMemoryType:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,AFM_PARAM_BINDING_MEMORY_TYPE);
                AFM_PARAM_BINDING_MEMORY_TYPE* param = (AFM_PARAM_BINDING_MEMORY_TYPE*)pComponentParameterStructure;
                param->memoryType = mBindingMemoryType;
                return OMX_ErrorNone;
            }

        default:
            return ENS_Port::getParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

AFM_API_EXPORT t_cm_mpc_memory_type AFM_Port::getBindingMpcMemoryType24(void) const
{
    switch (mBindingMemoryType) {
        case AFM_MEMORY_TYPE_SDRAM: return CM_MM_MPC_SDRAM24;
        case AFM_MEMORY_TYPE_ESRAM: return CM_MM_MPC_ESRAM24;
        case AFM_MEMORY_TYPE_TCMX:  return CM_MM_MPC_TCM24_X;
        case AFM_MEMORY_TYPE_TCMY:  return CM_MM_MPC_TCM24_Y;
        default:     DBC_ASSERT(0); return CM_MM_MPC_SDRAM24;
    }
}

/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Port.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_Client.h"
#include "ENS_Port.h"
#include "ENS_Port_Fsm.h"
#include "ENS_Component.h"
#include "ProcessingComponent.h"
#define OMXCOMPONENT "ENS_PORT"
#define INSTANTIATED
#include "osi_trace.h"
#include "ENS_Index.h"
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ens_proxy_ENS_Component_ENS_PortTraces.h"
#endif //OST_TRACE_COMPILER_IN_USE

ENS_API_EXPORT EnsCommonPortData::EnsCommonPortData(
    OMX_U32 nPortIndex,
    OMX_DIRTYPE eDir,
    OMX_U32 nBufferCountMin,
    OMX_U32 nBufferSizeMin,
    OMX_PORTDOMAINTYPE eDomain,
    OMX_BUFFERSUPPLIERTYPE eSupplierPref)
    :
    mPortIndex(nPortIndex),
    mDirection(eDir),
    mBufferCountMin(nBufferCountMin),
    mBufferSizeMin(nBufferSizeMin),
    mPortDomain(eDomain),
    mBufferSupplierPref(eSupplierPref)
{
}

ENS_API_EXPORT ENS_Port::ENS_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp)
    : mEnsComponent(enscomp)
{
    mPortFsm = new ENS_Port_Fsm(*this);

    mTunneledComponent      = 0;
    mTunneledPort           = 0;
    mTunnelFlags            = 0;
    mSupplierPreference     = commonPortData.mBufferSupplierPref;
    mBufferSupplier         = OMX_BufferSupplyUnspecified;
    mBufferCountCurrent     = 0;
    mUseStandardTunneling   = OMX_FALSE;

    // Fill in OMX_PARAM_PORTDEFINITIONTYPE
    mParamPortDefinition.nSize              = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
    mParamPortDefinition.nPortIndex         = commonPortData.mPortIndex;
    mParamPortDefinition.eDir               = commonPortData.mDirection;
    mParamPortDefinition.nBufferCountActual = commonPortData.mBufferCountMin;
    mParamPortDefinition.nBufferCountMin    = commonPortData.mBufferCountMin;
    mParamPortDefinition.nBufferSize        = commonPortData.mBufferSizeMin;
    mParamPortDefinition.bEnabled           = OMX_TRUE;
    mParamPortDefinition.bPopulated         = commonPortData.mBufferCountMin == 0? OMX_TRUE:OMX_FALSE;
    mParamPortDefinition.eDomain            = commonPortData.mPortDomain;
    // NOTE: mParamPortDefinition.format must be finished up by concrete ports

    mBufHdrTablePtr         = 0;

	// init STOST trace stuff
	setTraceInfo(enscomp.getSharedTraceInfoPtr(), commonPortData.mPortIndex);
	mPortFsm->setTraceInfo(enscomp.getSharedTraceInfoPtr(), commonPortData.mPortIndex);
    
    mSharedChunk = 0;
    // Buffer sharing data initialization
    mBufSharingPortList.mBufSharingPorts = 0;
    mBufSharingPortList.numPorts = 0;
    mIsAllocator = OMX_FALSE;

    #ifndef BACKWARD_COMPATIBILTY_MODE
        BUFFER_SHARING_ENABLED = true;
    #else
        BUFFER_SHARING_ENABLED = false;
    #endif

}

ENS_API_EXPORT ENS_Port::~ENS_Port(void) {
    if (mSharedChunk) {
        OMX_ERRORTYPE error = MMHwBuffer::Destroy(mSharedChunk);
        if (error != OMX_ErrorNone) {
            OstTraceInt1(TRACE_ERROR, "ENS_PROXY: ENS_Port::~ENS_Port error 0x%x of call to MMHwBuffer::Destroy\n", error);
        }
    }
    delete mPortFsm;
    if(mBufSharingPortList.mBufSharingPorts) {
        delete [] mBufSharingPortList.mBufSharingPorts;
    }
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Port::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) 
{
    switch (nParamIndex) {
        case OMX_IndexParamCompBufferSupplier:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_BUFFERSUPPLIERTYPE);
            OMX_PARAM_BUFFERSUPPLIERTYPE *bufSupply =
                static_cast<OMX_PARAM_BUFFERSUPPLIERTYPE *>
                    (pComponentParameterStructure);

            if(!getTunneledComponent()) {
                // ignored if port not yet tunneled
                // should be changed to appropriate error later
                return OMX_ErrorNone;
            }

            if (bufSupply->eBufferSupplier != OMX_BufferSupplyUnspecified) {
                mBufferSupplier = bufSupply->eBufferSupplier;
            } else {
                return OMX_ErrorBadParameter;
            }

            if (getDirection() == OMX_DirInput && getTunneledComponent()) {
                // if an input port gets this SetParameter, it means the IL
                // client is overriding the tunnel negotiation
                // the port is then responsible to signal the tunneled component
                bufSupply->nPortIndex = getTunneledPort();
                OMX_ERRORTYPE error = OMX_SetParameter(
                        getTunneledComponent(),
                        OMX_IndexParamCompBufferSupplier, 
                        bufSupply);
                if (error != OMX_ErrorNone) return error;
            }

            return OMX_ErrorNone;
        }

        case OMX_IndexParamPortDefinition:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_PORTDEFINITIONTYPE);
            OMX_PARAM_PORTDEFINITIONTYPE *portdef = 
                static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>
                    (pComponentParameterStructure);
            
            if (portdef->nBufferCountActual < mParamPortDefinition.nBufferCountMin){
                return OMX_ErrorBadParameter;
            }           

            mParamPortDefinition.nBufferCountActual = portdef->nBufferCountActual;
if(!BUFFER_SHARING_ENABLED){
            return setFormatInPortDefinition(*portdef);
}
else{
            // Port Format must be set by the concrete port
            OMX_ERRORTYPE error =  setFormatInPortDefinition(*portdef);
            if(error!=OMX_ErrorNone) return error;

/*
            if(mPortFsm->getState()==OMX_StateLoadedToIdleHOST ||
               mPortFsm->getState()==OMX_StateLoadedToIdleDSP ||
               mPortFsm->getState()==OMX_StateCheckRscAvailability)
                //|| mPortFsm->getState()==OmxStateTransientToEnableHOST ||
               //mPortFsm->getState()==OmxStateTransientToEnableDSP)
            {   
                error = sendBufferReqs(OMX_IndexParamPortDefinition,pComponentParameterStructure);
                if(error!=OMX_ErrorNone) return error; 
            }
*/
            return OMX_ErrorNone;
            
}
        }

        default:
            return OMX_ErrorUnsupportedIndex;
    }
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Port::updateSettings(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure)
{
    ENS_CmdUpdateSettingsEvt evt(nParamIndex, pComponentParameterStructure);
    ENS_Port_Fsm *portfsm = getPortFsm();
    if (!portfsm) {
        return OMX_ErrorBadPortIndex;
    }
    return portfsm->dispatch(&evt);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Port::getParameter(
        OMX_INDEXTYPE nParamIndex,  
        OMX_PTR pComponentParameterStructure) const 
{
    switch (nParamIndex) {
        
        case OMX_IndexParamCompBufferSupplier: 
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_BUFFERSUPPLIERTYPE);
            OMX_PARAM_BUFFERSUPPLIERTYPE *bufSupply =
                static_cast<OMX_PARAM_BUFFERSUPPLIERTYPE *>
                    (pComponentParameterStructure);

            bufSupply->eBufferSupplier = mBufferSupplier;
            
            return OMX_ErrorNone;
        }

        case OMX_IndexParamPortDefinition:
        {   
if(!BUFFER_SHARING_ENABLED){
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_PORTDEFINITIONTYPE);

            OMX_PARAM_PORTDEFINITIONTYPE *portdef = 
                static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>
                    (pComponentParameterStructure);
            
 			*portdef = mParamPortDefinition;

	         return OMX_ErrorNone;
}else{
            //OMX_ERRORTYPE error;

            //OMX_U32 maxBufferSize = 0, maxBufferCountActual = 0;
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_PORTDEFINITIONTYPE);
            OMX_PARAM_PORTDEFINITIONTYPE *portdef = 
                    static_cast<OMX_PARAM_PORTDEFINITIONTYPE *> (pComponentParameterStructure);

   /*         if(mPortFsm->getState()==OMX_StateLoadedToIdleHOST ||
               mPortFsm->getState()==OMX_StateLoadedToIdleDSP ||
               mPortFsm->getState()==OMX_StateCheckRscAvailability)
                //|| mPortFsm->getState()==OmxStateTransientToEnableHOST ||
               //mPortFsm->getState()==OmxStateTransientToEnableDSP)
            {
                error = calculateBufferReqs(&maxBufferSize,&maxBufferCountActual);
                if(error!=OMX_ErrorNone) return error;
            }

            // Calculate self requirements
            maxBufferSize = MAX(mParamPortDefinition.nBufferSize, maxBufferSize);
            maxBufferCountActual = MAX(mParamPortDefinition.nBufferCountActual,maxBufferCountActual);
*/
            *portdef = mParamPortDefinition;
    /*        portdef->nBufferSize = maxBufferSize;
            portdef->nBufferCountActual = maxBufferCountActual;*/

            return OMX_ErrorNone;
}
        }

        default:
            return OMX_ErrorUnsupportedIndex;
    }
}

OMX_ERRORTYPE ENS_Port::tunnelRequest(
        OMX_HANDLETYPE hTunneledComp,  
        OMX_U32 nTunneledPort,
        OMX_TUNNELSETUPTYPE* pTunnelSetup) {

    OMX_ERRORTYPE err;

    if (hTunneledComp == 0) {
        setTunneled(0, 0, getTunnelFlags());
        return OMX_ErrorNone;
    }

    if (mParamPortDefinition.nBufferCountMin == 0) {
        OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
        tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
        tunneledPortDef.nPortIndex  = nTunneledPort;
        getOmxIlSpecVersion(&tunneledPortDef.nVersion);
        err = OMX_GetParameter(hTunneledComp, 
                OMX_IndexParamPortDefinition, &tunneledPortDef);
        if(err != OMX_ErrorNone) return err;
        if (tunneledPortDef.nBufferCountMin != 0) {
	        OstTraceFiltInst0(TRACE_DEBUG, "ENS_PROXY: ENS_Port:tunnelRequest return error because this port needs 0 buffer and tunneled port requires at least one");
            return OMX_ErrorPortsNotCompatible;
        }
    }

    if (mUseStandardTunneling == OMX_FALSE) {
        // check if proprietary communication is possible
        err = proprietaryCommunicationRequest(
                hTunneledComp, nTunneledPort);

        if (err != OMX_ErrorNone) { 
            mUseStandardTunneling = OMX_TRUE;
        } 
    }

    if (getDirection() == OMX_DirOutput) {
        // Output Port
        pTunnelSetup->nTunnelFlags  = getTunnelFlags();
        pTunnelSetup->eSupplier     = getSupplierPreference();
        setTunneled(hTunneledComp, nTunneledPort, pTunnelSetup->nTunnelFlags);

        return OMX_ErrorNone;
    } 
    else {
        // Input Port
        OMX_BUFFERSUPPLIERTYPE bufferSupplierChoice;

        err = checkCompatibility(hTunneledComp, nTunneledPort);
        if (err != OMX_ErrorNone) return err;

        setTunneled(hTunneledComp, nTunneledPort, pTunnelSetup->nTunnelFlags);

        // make a best guess on who should supply the buffer
        if (getSupplierPreference() == OMX_BufferSupplyOutput
                || pTunnelSetup->eSupplier == OMX_BufferSupplyOutput) {
            // at least one port prefers output so select output
            bufferSupplierChoice = OMX_BufferSupplyOutput;
        }
        else if (getSupplierPreference() == OMX_BufferSupplyUnspecified
                && pTunnelSetup->eSupplier == OMX_BufferSupplyUnspecified) {
            // both ports don't care so select output
            bufferSupplierChoice = OMX_BufferSupplyOutput;
        }
        else {
            bufferSupplierChoice = OMX_BufferSupplyInput;
        }
        
        // Inform output port of which port supplies the buffer
        OMX_PARAM_BUFFERSUPPLIERTYPE bufSupply;
        bufSupply.nSize         = sizeof(bufSupply);
        getOmxIlSpecVersion(&bufSupply.nVersion);
        bufSupply.nPortIndex    = nTunneledPort;
        bufSupply.eBufferSupplier   = bufferSupplierChoice;

        err = OMX_SetParameter(
                hTunneledComp, OMX_IndexParamCompBufferSupplier, &bufSupply);
        if (err != OMX_ErrorNone) return err; 

        setBufferSupplier(bufferSupplierChoice);
    }

    return OMX_ErrorNone;

}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Port::standardTunnelingInit(void) 
{
    OMX_BUFFERHEADERTYPE *  pBufferHdr;
    OMX_U8 *                pBuffer;
    OMX_ERRORTYPE           error;

    OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
    OMX_U32 bufferSize = 0, bufferCountActual = 0;
if(!BUFFER_SHARING_ENABLED){
    tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    tunneledPortDef.nPortIndex  = getTunneledPort();
    getOmxIlSpecVersion(&tunneledPortDef.nVersion);
    error = OMX_GetParameter(getTunneledComponent(), 
            OMX_IndexParamPortDefinition, &tunneledPortDef);
    if(error != OMX_ErrorNone) return error;
    
    bufferSize = 
        MAX(mParamPortDefinition.nBufferSize, tunneledPortDef.nBufferSize);
    bufferCountActual = MAX(mParamPortDefinition.nBufferCountActual, 
                                    tunneledPortDef.nBufferCountActual);

    if (bufferCountActual != tunneledPortDef.nBufferCountActual) {
        tunneledPortDef.nBufferCountActual = bufferCountActual;
        error = OMX_SetParameter(getTunneledComponent(), 
                OMX_IndexParamPortDefinition, &tunneledPortDef);
        if (error != OMX_ErrorNone) return error;
    }
    
    mParamPortDefinition.nBufferCountActual = bufferCountActual;
}else{
    //OMX_U32 bufferSize = 0, bufferCountActual = 0;
    // Now get all requirements from all buffer users: sharing ports, tunneled ports, self
    error = calculateBufferReqs(&bufferSize, &bufferCountActual);
    if(error != OMX_ErrorNone) return error;
    
    // Now consider self requirements
    bufferSize= MAX(mParamPortDefinition.nBufferSize, bufferSize);
    bufferCountActual = MAX(mParamPortDefinition.nBufferCountActual, bufferCountActual);

    mParamPortDefinition.nBufferCountActual = bufferCountActual;
    tunneledPortDef = mParamPortDefinition;
    error = sendBufferReqs(OMX_IndexParamPortDefinition, &tunneledPortDef);
    if(error != OMX_ErrorNone) return error;

                                    
}
    if(bufferCountActual != 0) {
        mBufHdrTablePtr = new ENS_BUFFERHEADER_TABLETYPE [bufferCountActual];
        if (!mBufHdrTablePtr)  return OMX_ErrorInsufficientResources;
    
        for (unsigned int i = 0; i < bufferCountActual; i++) {
            void *bufferAllocInfo, *portPrivateInfo;
		
            mBufHdrTablePtr[i].mBufHdrPtr = 0;
            mBufHdrTablePtr[i].mBufHdrStatus = 0;
            mBufHdrTablePtr[i].mAtSharingPorts = 0;
            
            error = getProcessingComponent().allocateBuffer(mParamPortDefinition.nPortIndex,
                        i, bufferSize, &pBuffer, &bufferAllocInfo, &portPrivateInfo);
            if(error != OMX_ErrorNone) return error;
if(!BUFFER_SHARING_ENABLED){	    
            if (i==0 && mSharedChunk) {
                // Send the config for shared chunk
                error = mSharedChunk->SetConfigExtension(getTunneledComponent(), getTunneledPort());
            }
            error = OMX_UseBuffer(getTunneledComponent(), &pBufferHdr,
                        mTunneledPort, 0, bufferSize, pBuffer);
            if(error != OMX_ErrorNone) return error;
}else{
            error =  forwardBuffer(&pBufferHdr,0,bufferSize, 
                                   pBuffer,bufferAllocInfo,portPrivateInfo);
            if(error != OMX_ErrorNone) return error;
}        
            pBufferHdr->pPlatformPrivate = bufferAllocInfo;
            if (mParamPortDefinition.eDir == OMX_DirInput) {
                pBufferHdr->pInputPortPrivate   = portPrivateInfo;
                pBufferHdr->nInputPortIndex     = mParamPortDefinition.nPortIndex;
            } else {
                pBufferHdr->pOutputPortPrivate  = portPrivateInfo;
                pBufferHdr->nOutputPortIndex    = mParamPortDefinition.nPortIndex;
            }

            error = getProcessingComponent().useBufferHeader(mParamPortDefinition.eDir, pBufferHdr);
            if(error != OMX_ErrorNone) return error;

            mBufHdrTablePtr[i].mBufHdrPtr = pBufferHdr;
		    mBufHdrTablePtr[i].mBufHdrStatus = ENS_PORT_BUFFER_ALLOCATED | ENS_PORT_BUFFER_ATCOMPONENT;
            mBufHdrTablePtr[i].mAtSharingPorts = 0;

            mBufferCountCurrent++;
        }
    }

    mParamPortDefinition.bPopulated = OMX_TRUE;

    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Port::standardTunnelingDeInit(void) 
{
    OMX_ERRORTYPE           error;
    void *                  portPrivateInfo;

    for (unsigned int i = 0; i < mParamPortDefinition.nBufferCountActual; i++) {
        OMX_BUFFERHEADERTYPE * pBufferHdr = mBufHdrTablePtr[i].mBufHdrPtr;

		DBC_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ALLOCATED) 
				   && (mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ATCOMPONENT));
		
        mBufferCountCurrent--;

        if (mParamPortDefinition.eDir == OMX_DirInput) {
            portPrivateInfo = pBufferHdr->pInputPortPrivate;
        } else {
            portPrivateInfo = pBufferHdr->pOutputPortPrivate;
        }

        error = getProcessingComponent().freeBuffer(mParamPortDefinition.nPortIndex,
                    mBufferCountCurrent, OMX_TRUE, pBufferHdr->pPlatformPrivate, portPrivateInfo);
        if(error != OMX_ErrorNone) return error;

        error = OMX_FreeBuffer(getTunneledComponent(), mTunneledPort, pBufferHdr);
        if(error != OMX_ErrorNone) return error;
	

		mBufHdrTablePtr[i].mBufHdrPtr = 0;
		mBufHdrTablePtr[i].mBufHdrStatus = 0;
		
    }

    mSharedChunk = 0;
    mParamPortDefinition.bPopulated = OMX_FALSE;

    delete [] mBufHdrTablePtr;
    mBufHdrTablePtr = 0;
    
    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Port::standardTunnelingTransfertInit(void) 
{
    OMX_ERRORTYPE error;

    if (!isBufferSupplier())    return OMX_ErrorNone;
if(BUFFER_SHARING_ENABLED){
    if (!isAllocatorPort())     return OMX_ErrorNone;
}

    for (unsigned int i = 0; i < mParamPortDefinition.nBufferCountActual; i++) {
        OMX_BUFFERHEADERTYPE * pBufferHdr = mBufHdrTablePtr[i].mBufHdrPtr;

        if (!mBufHdrTablePtr[i].mBufHdrStatus) continue;
        if (mBufHdrTablePtr[i].mBufHdrStatus != (ENS_PORT_BUFFER_ALLOCATED | ENS_PORT_BUFFER_ATCOMPONENT)) continue;

        if ( mParamPortDefinition.eDir == OMX_DirOutput ) {
if(!BUFFER_SHARING_ENABLED){
            bufferSentToProcessingComponent(pBufferHdr);
            error = getProcessingComponent().fillThisBuffer(pBufferHdr);
            if (error != OMX_ErrorNone) {
                bufferReturnedFromProcessingComp(pBufferHdr);
                return error;
            }
}else{
            // Buffer-supplier output ports must send empty buffers to Processing Component
            if(getNumberInterConnectedPorts()) {
                error = forwardOutputBuffer(pBufferHdr);
                if(error!=OMX_ErrorNone) return error;

                bufferSentToSharingPort(pBufferHdr);
            }
            else{
                bufferSentToProcessingComponent(pBufferHdr);
                error = getProcessingComponent().fillThisBuffer(pBufferHdr);
                if (error != OMX_ErrorNone) {
                    bufferReturnedFromProcessingComp(pBufferHdr);
                    return error;
                }
            }
}
        } else {
            // Buffer-supplier input ports must send empty buffers to tunneled port
            bufferSentToNeighbor(pBufferHdr);
            error = OMX_FillThisBuffer(mTunneledComponent, pBufferHdr);
            if (error != OMX_ErrorNone) {
                bufferReturnedFromNeighbor(pBufferHdr);
                return error;
            }
        }
	} 

    return OMX_ErrorNone;
}


OMX_BUFFERHEADERTYPE * ENS_Port::allocateBufferHeader(
        OMX_PTR pAppPrivate,
        OMX_U32 nSizeBytes)
{
    OMX_BUFFERHEADERTYPE *  pBufferHdr = 0;

    pBufferHdr = new OMX_BUFFERHEADERTYPE;
    if (!pBufferHdr) return 0;

    //Store pointer to BufferHeader in Port
    if (mBufferCountCurrent == 0) {
        mBufHdrTablePtr 
            = new ENS_BUFFERHEADER_TABLETYPE [mParamPortDefinition.nBufferCountActual];
        if (!mBufHdrTablePtr)  return 0;
    }

    mBufHdrTablePtr[mBufferCountCurrent].mBufHdrPtr = pBufferHdr;
    mBufHdrTablePtr[mBufferCountCurrent].mBufHdrStatus = ENS_PORT_BUFFER_ALLOCATED | ENS_PORT_BUFFER_ATNEIGHBOR;
    mBufHdrTablePtr[mBufferCountCurrent].mAtSharingPorts = 0;
   
    
    pBufferHdr->nSize = sizeof(OMX_BUFFERHEADERTYPE);
    getOmxIlSpecVersion(&pBufferHdr->nVersion);

    pBufferHdr->nAllocLen               = nSizeBytes;
    pBufferHdr->nFilledLen              = 0;
    pBufferHdr->nOffset                 = 0;
    pBufferHdr->pAppPrivate             = pAppPrivate;
    pBufferHdr->pPlatformPrivate        = pAppPrivate;
    pBufferHdr->hMarkTargetComponent    = 0;
    pBufferHdr->pMarkData               = 0;
    pBufferHdr->nTickCount              = 0;
    pBufferHdr->nTimeStamp              = 0;
    pBufferHdr->nFlags                  = 0;

    if (mParamPortDefinition.eDir == OMX_DirInput) {
        pBufferHdr->pOutputPortPrivate  = 0;
        pBufferHdr->nInputPortIndex     = mParamPortDefinition.nPortIndex;
        pBufferHdr->nOutputPortIndex    = getTunneledPort();
    } 
    else {
        pBufferHdr->pInputPortPrivate   = 0;
        pBufferHdr->nInputPortIndex     = getTunneledPort();
        pBufferHdr->nOutputPortIndex    = mParamPortDefinition.nPortIndex;
    }

    return pBufferHdr;
}

void ENS_Port::freeBufferHeader(OMX_BUFFERHEADERTYPE *pBufferHdr) {
    delete pBufferHdr;

    if (mBufferCountCurrent == 0) {
        delete [] mBufHdrTablePtr;
        mBufHdrTablePtr = 0;
    }
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Port::useBuffer(
        OMX_BUFFERHEADERTYPE** ppBufferHdr,
        OMX_PTR pAppPrivate,
        OMX_U32 nSizeBytes,
        OMX_U8* pBuffer)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pBufferHdr;
    void *portPrivateInfo;

    if (nSizeBytes < mParamPortDefinition.nBufferSize) {
        return OMX_ErrorBadParameter;
    }
    
    pBufferHdr = allocateBufferHeader(pAppPrivate, nSizeBytes);
    if(!pBufferHdr) return OMX_ErrorInsufficientResources;

    pBufferHdr->pBuffer = pBuffer;
    
    *ppBufferHdr = pBufferHdr;
    
    mIsBufferAllocator = false;

    error = getProcessingComponent().useBuffer(mParamPortDefinition.nPortIndex, 
            mBufferCountCurrent, pBufferHdr, &portPrivateInfo);
    if(error != OMX_ErrorNone) return error;

    if (mParamPortDefinition.eDir == OMX_DirInput) {
        pBufferHdr->pInputPortPrivate = portPrivateInfo;
    } else {
        pBufferHdr->pOutputPortPrivate = portPrivateInfo;
    }

    mBufferCountCurrent++;

    if (mBufferCountCurrent == mParamPortDefinition.nBufferCountActual) {
        mParamPortDefinition.bPopulated = OMX_TRUE;
    }
if(BUFFER_SHARING_ENABLED){
    error =  forwardBuffer(0,pAppPrivate,nSizeBytes,pBuffer,0,0);
    if(error != OMX_ErrorNone) return error;
}
    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Port::allocateBuffer(
        OMX_BUFFERHEADERTYPE** ppBufferHdr,
        OMX_PTR pAppPrivate,
        OMX_U32 nSizeBytes)
{
    OMX_BUFFERHEADERTYPE *  pBufferHdr;
    OMX_U8 *                pBuffer;
    OMX_ERRORTYPE           error;
    void *                  bufferAllocInfo;
    void *                  portPrivateInfo;    

    if (nSizeBytes<mParamPortDefinition.nBufferSize) {
        return OMX_ErrorBadParameter;
    }

    if (getTunneledComponent() != 0) {
        return OMX_ErrorIncorrectStateOperation;
    }

    pBufferHdr = allocateBufferHeader(pAppPrivate, nSizeBytes);
    if(!pBufferHdr) return OMX_ErrorInsufficientResources;

    error = getProcessingComponent().allocateBuffer(mParamPortDefinition.nPortIndex,
            mBufferCountCurrent, nSizeBytes, &pBuffer, &bufferAllocInfo, &portPrivateInfo);
    if(error != OMX_ErrorNone) return error;

    pBufferHdr->pBuffer             = pBuffer;
    pBufferHdr->pPlatformPrivate    = bufferAllocInfo;

    *ppBufferHdr = pBufferHdr;

    if (mParamPortDefinition.eDir == OMX_DirInput) {
        pBufferHdr->pInputPortPrivate = portPrivateInfo;
    } else {
        pBufferHdr->pOutputPortPrivate = portPrivateInfo;
    }

    error = getProcessingComponent().useBufferHeader(mParamPortDefinition.eDir, pBufferHdr);
    if(error != OMX_ErrorNone) return error;

    mBufferCountCurrent++;
    mIsBufferAllocator = true;

    if (mBufferCountCurrent == mParamPortDefinition.nBufferCountActual) {
        mParamPortDefinition.bPopulated = OMX_TRUE;
    }

    return OMX_ErrorNone;
}


ENS_API_EXPORT OMX_ERRORTYPE ENS_Port::freeBuffer(
        OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_ERRORTYPE   error;
    OMX_PTR         portPrivateInfo;
    OMX_BOOL        bufferAllocated;
    
    for (unsigned int i = 0; i < mParamPortDefinition.nBufferCountActual; i++) {
        OMX_BUFFERHEADERTYPE *pBufferHdr = mBufHdrTablePtr[i].mBufHdrPtr;

        if (pBufferHdr == pBuffer) {

			DBC_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ALLOCATED)
						&& (mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ATNEIGHBOR));

            mBufferCountCurrent--;

            mParamPortDefinition.bPopulated = OMX_FALSE;

            if (mParamPortDefinition.eDir == OMX_DirInput) {
                portPrivateInfo = pBufferHdr->pInputPortPrivate;
            } else {
                portPrivateInfo = pBufferHdr->pOutputPortPrivate;
            }

            if (getTunneledComponent() == 0 && mIsBufferAllocator) {
                bufferAllocated = OMX_TRUE;
            } else {
                bufferAllocated = OMX_FALSE;
            }

            error = getProcessingComponent().freeBuffer(mParamPortDefinition.nPortIndex,
                    mBufferCountCurrent, bufferAllocated, pBufferHdr->pPlatformPrivate, portPrivateInfo);
            if (error != OMX_ErrorNone) return error;

			mBufHdrTablePtr[i].mBufHdrPtr = 0;
			mBufHdrTablePtr[i].mBufHdrStatus = 0;

            freeBufferHeader(pBufferHdr);

			
		    if (mSharedChunk) {
		        if (mBufferCountCurrent==0) {       // ER342234
		            if (!bufferAllocated) {
		                error = MMHwBuffer::Close(mSharedChunk);
		                DBC_ASSERT(error == OMX_ErrorNone);
		            }
		            setSharedChunk(0);
		        }
		    }			
			
			
			
            return OMX_ErrorNone;
        }
    }
    
    mSharedChunk = 0;
    return OMX_ErrorBadParameter;
}


OMX_ERRORTYPE ENS_Port::getBufferHdrTableIndex(OMX_BUFFERHEADERTYPE* pBuffer, OMX_U32& Index)
{
    OMX_U32 i;

    for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) {
        OMX_BUFFERHEADERTYPE *pBufferHdr = mBufHdrTablePtr[i].mBufHdrPtr;

        if (pBuffer == pBufferHdr) {  
			Index = i;
            return OMX_ErrorNone;
        }
    }
	return OMX_ErrorUndefined;
}

OMX_ERRORTYPE ENS_Port::mapSharedBufferHeader(OMX_BUFFERHEADERTYPE* pBuffer,
                                                      OMX_BUFFERHEADERTYPE** ppLocalBuffer)
{
    OMX_U32 i;

    for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) {
        OMX_BUFFERHEADERTYPE *pBufferHdr = mBufHdrTablePtr[i].mBufHdrPtr;

        if (pBuffer->pBuffer == pBufferHdr->pBuffer) {  
			*ppLocalBuffer = pBufferHdr;
            return OMX_ErrorNone;
        }
    }
	return OMX_ErrorUndefined;
}


ENS_API_EXPORT OMX_BOOL ENS_Port::isAllBuffersAtOwner()
{
    OMX_U32 i;

    if(useProprietaryCommunication()) return OMX_TRUE;

	if(useStandardTunneling() && isBufferSupplier())
	{
if(!BUFFER_SHARING_ENABLED){
        for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) {
			if (!mBufHdrTablePtr[i].mBufHdrStatus) continue;
			if (mBufHdrTablePtr[i].mBufHdrStatus != (ENS_PORT_BUFFER_ALLOCATED | ENS_PORT_BUFFER_ATCOMPONENT)){ 
				return OMX_FALSE;
			}
		}
}else{
        if(isAllocatorPort()) {
            for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) {
    			if (!mBufHdrTablePtr[i].mBufHdrStatus) continue;
    			if (mBufHdrTablePtr[i].mBufHdrStatus != (ENS_PORT_BUFFER_ALLOCATED | ENS_PORT_BUFFER_ATCOMPONENT)){ 
    				return OMX_FALSE;
    			}
            }
        }
        else{
            for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) {
                if (!mBufHdrTablePtr[i].mBufHdrStatus) continue;
                if (mBufHdrTablePtr[i].mBufHdrStatus != (ENS_PORT_BUFFER_ALLOCATED | ENS_PORT_BUFFER_ATSHARINGPORT)){ 
                    return OMX_FALSE;
                }
            }
        }
}
	} else {
		for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) {
			if (!mBufHdrTablePtr[i].mBufHdrStatus) continue;
			if (mBufHdrTablePtr[i].mBufHdrStatus != (ENS_PORT_BUFFER_ALLOCATED | ENS_PORT_BUFFER_ATNEIGHBOR)){  
				return OMX_FALSE;
			}
		}
	}
	return OMX_TRUE;
}

ENS_API_EXPORT OMX_BOOL ENS_Port::isAllBuffersReturnedFromNeighbor()
{
    OMX_U32 i;

    if(!useStandardTunneling()) return OMX_TRUE;

	for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) {
		if (!mBufHdrTablePtr[i].mBufHdrStatus) continue;
		if (mBufHdrTablePtr[i].mBufHdrStatus == (ENS_PORT_BUFFER_ALLOCATED | ENS_PORT_BUFFER_ATNEIGHBOR)){
			return OMX_FALSE;
		}
	}
	
	return OMX_TRUE;
}


ENS_API_EXPORT OMX_BOOL ENS_Port::isAllBuffersReturnedFromSharingPorts()
{
    OMX_U32 i;

    if(!useStandardTunneling()) return OMX_TRUE;

	for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) {
		if (!mBufHdrTablePtr[i].mBufHdrStatus) continue;
		if (mBufHdrTablePtr[i].mBufHdrStatus == (ENS_PORT_BUFFER_ALLOCATED | ENS_PORT_BUFFER_ATSHARINGPORT)){
			return OMX_FALSE;
		}
	}
	
	return OMX_TRUE;
}


ENS_API_EXPORT void ENS_Port::bufferSentToProcessingComponent(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_U32 i = 0;
	OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port:bufferSentToProcessingComp  0x%x", (unsigned int)pBuffer);
	DBC_ASSERT(OMX_ErrorNone == getBufferHdrTableIndex(pBuffer,i));
	DBC_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ALLOCATED)
			  && (mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ATCOMPONENT));
	mBufHdrTablePtr[i].mBufHdrStatus |= ENS_PORT_BUFFER_ATPROCESSINGCOMP;
	mBufHdrTablePtr[i].mBufHdrStatus &= ~ENS_PORT_BUFFER_ATCOMPONENT;
}

ENS_API_EXPORT void ENS_Port::bufferReturnedFromProcessingComp(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_U32 i = 0;
	OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port:bufferReturnedFromProcessingComp  0x%x", (unsigned int)pBuffer);
	DBC_ASSERT(OMX_ErrorNone == getBufferHdrTableIndex(pBuffer,i));
	DBC_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ALLOCATED)
			   && (mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ATPROCESSINGCOMP));
	mBufHdrTablePtr[i].mBufHdrStatus &= ~ENS_PORT_BUFFER_ATPROCESSINGCOMP;
	mBufHdrTablePtr[i].mBufHdrStatus |= ENS_PORT_BUFFER_ATCOMPONENT;
}

ENS_API_EXPORT void ENS_Port::bufferSentToNeighbor(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_U32 i = 0;
	DBC_ASSERT(OMX_ErrorNone == getBufferHdrTableIndex(pBuffer,i));
	DBC_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ALLOCATED)
			   && (mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ATCOMPONENT));
	mBufHdrTablePtr[i].mBufHdrStatus |= ENS_PORT_BUFFER_ATNEIGHBOR;
	mBufHdrTablePtr[i].mBufHdrStatus &= ~ENS_PORT_BUFFER_ATCOMPONENT;
}

ENS_API_EXPORT void ENS_Port::bufferReturnedFromNeighbor(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_U32 i = 0;
	DBC_ASSERT(OMX_ErrorNone == getBufferHdrTableIndex(pBuffer,i));
	DBC_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ALLOCATED)
			   && (mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ATNEIGHBOR));
	mBufHdrTablePtr[i].mBufHdrStatus &= ~ENS_PORT_BUFFER_ATNEIGHBOR;
	mBufHdrTablePtr[i].mBufHdrStatus |= ENS_PORT_BUFFER_ATCOMPONENT;
}



OMX_ERRORTYPE ENS_Port::calculateBufferReqs(OMX_U32 *pMaxBufferSize, OMX_U32 *pMaxBufferCount) const {
    // Calculate requirement components
    OMX_ERRORTYPE error;
    OMX_U32 maxBufferSize=0;
    OMX_U32 maxBufferCount = 0;

    OMX_BOOL isSharingPortContributing = OMX_FALSE;
    OMX_BOOL isTunneledPortContributing=  OMX_FALSE;

        switch(getPortRole()) {
            case ENS_SHARING_PORT:
                isSharingPortContributing = OMX_FALSE;
                isTunneledPortContributing = OMX_TRUE;
                break;
            case ENS_NON_TUNNELED_PORT:
            case ENS_NON_SUPPLIER_PORT:
                isSharingPortContributing = OMX_TRUE;
                isTunneledPortContributing = OMX_FALSE;
                break;
            case ENS_ALLOCATOR_PORT:
                isSharingPortContributing = OMX_TRUE;
                isTunneledPortContributing = OMX_TRUE;
                break;
        }

    if(isSharingPortContributing) {
        OMX_U32 bufferSize=0;
        OMX_U32 bufferCount = 0;

        //Are there any sharing ports
        if(getNumberInterConnectedPorts()) {
            error = getSharingReqs(&bufferSize,&bufferCount);
            if(error!=OMX_ErrorNone) return error;
        }
        maxBufferSize = MAX(bufferSize, maxBufferSize);
        maxBufferCount = MAX(bufferCount,maxBufferCount);
    }

    if(isTunneledPortContributing) {
        OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
        tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
        tunneledPortDef.nPortIndex  = getTunneledPort();
        getOmxIlSpecVersion(&tunneledPortDef.nVersion);
        error = OMX_GetParameter(getTunneledComponent(), 
                OMX_IndexParamPortDefinition, &tunneledPortDef);
        if(error != OMX_ErrorNone) return error;

        maxBufferSize = MAX(maxBufferSize, tunneledPortDef.nBufferSize);
        maxBufferCount = MAX(maxBufferCount,tunneledPortDef.nBufferCountActual);
    }

    *pMaxBufferSize = maxBufferSize;
    *pMaxBufferCount = maxBufferCount;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_Port::getSharingReqs(OMX_U32 *pMaxBufferSize, OMX_U32 *pMaxBufferCount) const{
    OMX_ERRORTYPE error;
    OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
    ENS_Port* pPort;
    ENS_Port_Fsm * portFsm;
    OMX_U32 maxBufferSize=0;
    OMX_U32 maxBufferCount =0;
    ENS_CmdGetParamEvt evt(OMX_IndexParamPortDefinition, &tunneledPortDef);
    
    for(OMX_U32 i=0;i<getNumberInterConnectedPorts();i++) {
        pPort = *(mBufSharingPortList.mBufSharingPorts + i);
        tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
        tunneledPortDef.nPortIndex  = pPort->getPortIndex();
        getOmxIlSpecVersion(&tunneledPortDef.nVersion);
        portFsm = pPort->getPortFsm();

        error = portFsm->dispatch(&evt);
        if(error!=OMX_ErrorNone) return error;

        maxBufferSize = MAX(maxBufferSize, tunneledPortDef.nBufferSize);
        maxBufferCount = MAX(maxBufferCount,tunneledPortDef.nBufferCountActual);
    }
    
    *pMaxBufferSize = maxBufferSize;
    *pMaxBufferCount = maxBufferCount;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_Port::setSharingReqs(OMX_U32 bufferCountActual){
    // This call forwards only buffercountactual field along the chain
    OMX_ERRORTYPE error;
    OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
    ENS_Port* pPort;
    ENS_Port_Fsm * portFsm;
    ENS_CmdGetParamEvt getParamEvt(OMX_IndexParamPortDefinition, &tunneledPortDef);
    ENS_CmdSetParamEvt setparamEvt(OMX_IndexParamPortDefinition, &tunneledPortDef);
    
    for(OMX_U32 i=0;i<getNumberInterConnectedPorts();i++) {
        pPort = *(mBufSharingPortList.mBufSharingPorts + i);
        tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
        tunneledPortDef.nPortIndex  = pPort->getPortIndex();
        getOmxIlSpecVersion(&tunneledPortDef.nVersion);
        portFsm = pPort->getPortFsm();

        // Obtain port settings
        error = portFsm->dispatch(&getParamEvt);
        if(error!=OMX_ErrorNone) return error;

        // set port settings
        tunneledPortDef.nBufferCountActual = bufferCountActual; 
        error = portFsm->dispatch(&setparamEvt);
        if(error!=OMX_ErrorNone) return error;
    }

    return OMX_ErrorNone;
}


OMX_ERRORTYPE ENS_Port::sendBufferReqs(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentParameterStructure) {
    OMX_BOOL isSharingPortContributing = OMX_FALSE;
    OMX_BOOL isTunneledPortContributing=  OMX_FALSE;
    OMX_U32 bufferCountActual=0;
    OMX_ERRORTYPE error;

    if(nParamIndex!=OMX_IndexParamPortDefinition) return OMX_ErrorNone;

    CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_PORTDEFINITIONTYPE);
            OMX_PARAM_PORTDEFINITIONTYPE *portdef = 
                static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>
                    (pComponentParameterStructure);
    bufferCountActual = portdef->nBufferCountActual;

    DBC_ASSERT(bufferCountActual);

    switch(getPortRole()) {
        case ENS_SHARING_PORT:
            isSharingPortContributing = OMX_FALSE;
            isTunneledPortContributing = OMX_TRUE;
            break;
        case ENS_NON_TUNNELED_PORT:
        case ENS_NON_SUPPLIER_PORT:
            isSharingPortContributing = OMX_TRUE;
            isTunneledPortContributing = OMX_FALSE;
            break;
        case ENS_ALLOCATOR_PORT:
            isSharingPortContributing = OMX_TRUE;
            isTunneledPortContributing = OMX_TRUE;
            break;
    }


    if(isSharingPortContributing) {
        //Are there any sharing ports
        if(getNumberInterConnectedPorts()) {
            error = setSharingReqs(bufferCountActual);
            if(error!=OMX_ErrorNone) return error;
        }
    }

    if(isTunneledPortContributing) {
        OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
        tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
        tunneledPortDef.nPortIndex  = getTunneledPort();
        getOmxIlSpecVersion(&tunneledPortDef.nVersion);
        error = OMX_GetParameter(getTunneledComponent(), 
                OMX_IndexParamPortDefinition, &tunneledPortDef);
        if(error != OMX_ErrorNone) return error;

        // We have to call setparameter to remove any mismatch along the chain
        tunneledPortDef.nBufferCountActual = bufferCountActual;
        error = OMX_SetParameter(getTunneledComponent(), 
                OMX_IndexParamPortDefinition, &tunneledPortDef);
        if (error != OMX_ErrorNone) return error;
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_Port::forwardBuffer(OMX_BUFFERHEADERTYPE **ppBufferHdr,OMX_PTR pAppPrivate,
                            OMX_U32 bufferSize, OMX_U8* pBuffer,
                            void *bufferAllocInfo,void *portPrivateInfo){
    OMX_ERRORTYPE error;
    OMX_BOOL isSharingPortContributing = OMX_FALSE;
    OMX_BOOL isTunneledPortContributing=  OMX_FALSE;

    switch(getPortRole()) {
        case ENS_SHARING_PORT:
            isSharingPortContributing = OMX_FALSE;
            isTunneledPortContributing = OMX_TRUE;
            break;
        case ENS_NON_TUNNELED_PORT:
        case ENS_NON_SUPPLIER_PORT:
            isSharingPortContributing = OMX_TRUE;
            isTunneledPortContributing = OMX_FALSE;
            break;
        case ENS_ALLOCATOR_PORT:
            isSharingPortContributing = OMX_TRUE;
            isTunneledPortContributing = OMX_TRUE;
            break;
    }

    if(isSharingPortContributing && getNumberInterConnectedPorts()!=0) {
        //Forward the buffer to sharing ports
        // This call forwards only buffercountactual field along the chain
        OMX_ERRORTYPE error;
        ENS_Port* pPort;
        ENS_Port_Fsm * portFsm;

        ENS_CmdUseSharedBufferEvt evt(0,0,pAppPrivate
                                      ,bufferSize,pBuffer
                                      ,(void*)getSharedChunk());

        
        for(OMX_U32 i=0;i<getNumberInterConnectedPorts();i++) {
            pPort = *(mBufSharingPortList.mBufSharingPorts + i);
            portFsm = pPort->getPortFsm();
            error = portFsm->dispatch(&evt);
            if(error!=OMX_ErrorNone) return error;
        }

    }

    if(isTunneledPortContributing) {
        if (mBufferCountCurrent==0 && mSharedChunk) {
            // Send the config for shared chunk
            error = mSharedChunk->SetConfigExtension(getTunneledComponent(), getTunneledPort());
        }

        // Forward the buffer to tunneled port.
        error = OMX_UseBuffer(getTunneledComponent(), ppBufferHdr, mTunneledPort, pAppPrivate, bufferSize, pBuffer);
        if(error != OMX_ErrorNone) return error;
    }

    return OMX_ErrorNone;
}



OMX_ERRORTYPE ENS_Port::forwardInputBuffer(OMX_BUFFERHEADERTYPE *pBufferHdr){
    OMX_ERRORTYPE error;
    ENS_Port* pPort;
    ENS_Port_Fsm * portFsm;
    ENS_CmdEmptyThisSharedBufferEvt emptyThisSharedBufferEvt(pBufferHdr);
    
    for(OMX_U32 i=0;i<getNumberInterConnectedPorts();i++) {
        pPort = *(mBufSharingPortList.mBufSharingPorts + i);
        portFsm = pPort->getPortFsm();

        // Obtain port settings
        error = portFsm->dispatch(&emptyThisSharedBufferEvt);
        if(error!=OMX_ErrorNone) return error;
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_Port::forwardOutputBuffer(OMX_BUFFERHEADERTYPE *pBufferHdr){
    OMX_ERRORTYPE error;
    ENS_Port* pPort;
    ENS_Port_Fsm * portFsm;
    ENS_CmdFillThisSharedBufferEvt fillThisSharedBufferEvt(pBufferHdr);
    
    for(OMX_U32 i=0;i<getNumberInterConnectedPorts();i++) {
        pPort = *(mBufSharingPortList.mBufSharingPorts + i);
        portFsm = pPort->getPortFsm();

        // Obtain port settings
        error = portFsm->dispatch(&fillThisSharedBufferEvt);
        if(error!=OMX_ErrorNone) return error;
    }

    return OMX_ErrorNone;
}


void ENS_Port::bufferSentToSharingPort(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_U32 i = 0;
	DBC_ASSERT(OMX_ErrorNone == getBufferHdrTableIndex(pBuffer,i));
	DBC_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ALLOCATED)
			   && (mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ATCOMPONENT));
	mBufHdrTablePtr[i].mBufHdrStatus |= ENS_PORT_BUFFER_ATSHARINGPORT;
	mBufHdrTablePtr[i].mBufHdrStatus &= ~ENS_PORT_BUFFER_ATCOMPONENT;
    mBufHdrTablePtr[i].mAtSharingPorts = getNumberInterConnectedPorts();
}


OMX_BOOL ENS_Port::bufferReturnedFromSharingPort(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_U32 i = 0;
	DBC_ASSERT(OMX_ErrorNone == getBufferHdrTableIndex(pBuffer,i));
	DBC_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ALLOCATED)
			   && (mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ATSHARINGPORT));

    mBufHdrTablePtr[i].mAtSharingPorts--;
    if(!mBufHdrTablePtr[i].mAtSharingPorts) {
        // Shared Buffer returned by all ports
        mBufHdrTablePtr[i].mBufHdrStatus &= ~ENS_PORT_BUFFER_ATSHARINGPORT;
        mBufHdrTablePtr[i].mBufHdrStatus |= ENS_PORT_BUFFER_ATCOMPONENT;
        return OMX_TRUE;
    }

    return OMX_FALSE;
}

OMX_ERRORTYPE ENS_Port::useSharedBuffer(
        OMX_PTR pAppPrivate,
        OMX_U32 nSizeBytes,
        OMX_U8* pBuffer,
        void *pSharedChunk)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pBufferHdr;
    void *                  bufferAllocInfo;
    void *                  portPrivateInfo;    
    MMHwBuffer *sharedChunk = (MMHwBuffer *)pSharedChunk;
    
    if (nSizeBytes < mParamPortDefinition.nBufferSize) {
        return OMX_ErrorBadParameter;
    }

    DBC_ASSERT(getTunneledComponent()!=0);

    if (mBufferCountCurrent == 0) {
        mBufHdrTablePtr 
            = new ENS_BUFFERHEADER_TABLETYPE [mParamPortDefinition.nBufferCountActual];
        if (!mBufHdrTablePtr)  return OMX_ErrorInsufficientResources;
        
        setSharedChunk(sharedChunk);
        if (mSharedChunk) {
            // Send the config for shared chunk
            error = mSharedChunk->SetConfigExtension(getTunneledComponent(), getTunneledPort());
        }
    }

    mBufHdrTablePtr[mBufferCountCurrent].mBufHdrPtr = 0;
    mBufHdrTablePtr[mBufferCountCurrent].mBufHdrStatus = 0;
    

    error = getProcessingComponent().getMMHWBufferInfo(mParamPortDefinition.nPortIndex,
                                                       nSizeBytes,
                                                       pBuffer,
                                                       &bufferAllocInfo,
                                                       &portPrivateInfo);
    if(error != OMX_ErrorNone) return error;


    error = OMX_UseBuffer(getTunneledComponent(), &pBufferHdr ,mTunneledPort, pAppPrivate, nSizeBytes, pBuffer);
    if(error != OMX_ErrorNone) return error;


    pBufferHdr->pPlatformPrivate = bufferAllocInfo;
    if (mParamPortDefinition.eDir == OMX_DirInput) {
        pBufferHdr->pInputPortPrivate   = portPrivateInfo;
        pBufferHdr->nInputPortIndex     = mParamPortDefinition.nPortIndex;
    } else {
        pBufferHdr->pOutputPortPrivate  = portPrivateInfo;
        pBufferHdr->nOutputPortIndex    = mParamPortDefinition.nPortIndex;
    }

    error = getProcessingComponent().useBufferHeader(mParamPortDefinition.eDir, pBufferHdr);
    if(error != OMX_ErrorNone) return error;

    mBufHdrTablePtr[mBufferCountCurrent].mBufHdrPtr = pBufferHdr;
    mBufHdrTablePtr[mBufferCountCurrent].mBufHdrStatus = ENS_PORT_BUFFER_ALLOCATED | ENS_PORT_BUFFER_ATSHARINGPORT;
    // mAtSharingPorts should have value 1 in any case.
    DBC_ASSERT( getNumberInterConnectedPorts()==1);
    mBufHdrTablePtr[mBufferCountCurrent].mAtSharingPorts = getNumberInterConnectedPorts();

    mBufferCountCurrent++;

    if (mBufferCountCurrent == mParamPortDefinition.nBufferCountActual) {
        mParamPortDefinition.bPopulated = OMX_TRUE;
    }

    return OMX_ErrorNone;
}


OMX_ERRORTYPE ENS_Port::bufferSharingDeinit(){

   OMX_ERRORTYPE   error;
   void *                  portPrivateInfo;

    for (unsigned int i = 0; i < mParamPortDefinition.nBufferCountActual; i++) {
        OMX_BUFFERHEADERTYPE * pBufferHdr = mBufHdrTablePtr[i].mBufHdrPtr;

		DBC_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ALLOCATED) 
				   && (mBufHdrTablePtr[i].mBufHdrStatus & ENS_PORT_BUFFER_ATSHARINGPORT));
		
        mBufferCountCurrent--;

        if (mParamPortDefinition.eDir == OMX_DirInput) {
            portPrivateInfo = pBufferHdr->pInputPortPrivate;
        } else {
            portPrivateInfo = pBufferHdr->pOutputPortPrivate;
        }

        error = getProcessingComponent().freeSharedBuffer(mParamPortDefinition.nPortIndex,
                    mBufferCountCurrent, OMX_FALSE, pBufferHdr->pPlatformPrivate, portPrivateInfo);
        if(error != OMX_ErrorNone) return error;

        error = OMX_FreeBuffer(getTunneledComponent(), mTunneledPort, pBufferHdr);
        if(error != OMX_ErrorNone) return error;
	

		mBufHdrTablePtr[i].mBufHdrPtr = 0;
		mBufHdrTablePtr[i].mBufHdrStatus = 0;
		
    }

    mSharedChunk = 0;
    mParamPortDefinition.bPopulated = OMX_FALSE;

    delete [] mBufHdrTablePtr;
    mBufHdrTablePtr = 0;
    
    return OMX_ErrorNone;
}


void ENS_Port::bufferSharingEnabled(){
	BUFFER_SHARING_ENABLED = true;
	getPortFsm()->bufferSharingEnabled();
}

ENS_API_EXPORT OMX_BOOL ENS_Port::mustSendEventHandler(const OMX_PARAM_PORTDEFINITIONTYPE *pOld)
{
    return OMX_TRUE;
}



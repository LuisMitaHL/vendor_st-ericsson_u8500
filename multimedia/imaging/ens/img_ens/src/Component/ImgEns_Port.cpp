/*****************************************************************************/
/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
/*****************************************************************************/
#include "ImgEns_Shared.h"
#include <omxil/OMX_Types.h>
#include "ImgEns_Fsm.h"
#include "ImgEns_Trace.h"
#include "ImgEns_Port.h"
#include "ImgEns_Port_Fsm.h"
#include "ImgEns_Component.h"
#include "ImgEns_ProcessingComponent.h"
#include "ImgEns_Index.h"
#include "mmhwbuffer.h"

#define OMXCOMPONENT "ImgEns_PORT"
#define INSTANTIATED
#include "osi_trace.h"
//#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ens_proxy_ENS_Component_ENS_PortTraces.h"
#endif //OST_TRACE_COMPILER_IN_USE

#undef MAX
#define MAX(a,b) (((a)<(b))?(b):(a))

ImgEns_CommonPortData::ImgEns_CommonPortData(OMX_U32 nPortIndex, OMX_DIRTYPE eDir, OMX_U32 nBufferCountMin, OMX_U32 nBufferSizeMin, OMX_PORTDOMAINTYPE eDomain, OMX_BUFFERSUPPLIERTYPE eSupplierPref)
{
	mPortIndex          = nPortIndex;
	mDirection          = eDir;
	mBufferCountMin     = nBufferCountMin;
	mBufferSizeMin      = nBufferSizeMin;
	mPortDomain         = eDomain;
	mBufferSupplierPref = eSupplierPref;
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

ImgEns_Port::ImgEns_Port(const ImgEns_CommonPortData& commonPortData, ImgEns_Component &enscomp)
: mEnsComponent(enscomp)
{
	mPortFsm = new ImgEns_Port_Fsm(*this);

	mTunneledComponent      = 0;
	mTunneledPort           = 0;
	mTunnelFlags            = 0;
	mSupplierPreference     = commonPortData.mBufferSupplierPref;
	mBufferSupplier         = OMX_BufferSupplyUnspecified;
	mBufferCountCurrent     = 0;
	mUseStandardTunneling   = OMX_FALSE;

	// Fill in OMX_PARAM_PORTDEFINITIONTYPE
	mParamPortDefinition.nSize              = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	getImgOmxIlSpecVersion(&mParamPortDefinition.nVersion);
	mParamPortDefinition.nPortIndex         = commonPortData.mPortIndex;
	mParamPortDefinition.eDir               = commonPortData.mDirection;
	mParamPortDefinition.nBufferCountActual = commonPortData.mBufferCountMin;
	mParamPortDefinition.nBufferCountMin    = commonPortData.mBufferCountMin;
	mParamPortDefinition.nBufferSize        = commonPortData.mBufferSizeMin;
	mParamPortDefinition.bEnabled           = OMX_TRUE;
	mParamPortDefinition.bPopulated         = commonPortData.mBufferCountMin == 0? OMX_TRUE:OMX_FALSE;
	mParamPortDefinition.eDomain            = commonPortData.mPortDomain;
	mParamPortDefinition.nBufferAlignment   = 0;
	mParamPortDefinition.bBuffersContiguous = OMX_FALSE;

	// Put default value for format
	mParamPortDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
	mParamPortDefinition.format.video.nBitrate              = 0;
	mParamPortDefinition.format.video.xFramerate            = 0;
	mParamPortDefinition.format.video.pNativeRender         = NULL;
	mParamPortDefinition.format.video.pNativeWindow         = NULL;
	mParamPortDefinition.format.video.eCompressionFormat    = OMX_VIDEO_CodingUnused;
	mParamPortDefinition.format.video.cMIMEType             = NULL;
	mParamPortDefinition.format.video.nFrameWidth           = 0;
	mParamPortDefinition.format.video.nFrameHeight          = 0;
	mParamPortDefinition.format.video.nStride               = 0;
	mParamPortDefinition.format.video.nSliceHeight          = 0;
	mParamPortDefinition.format.video.eColorFormat          = OMX_COLOR_FormatUnused;

	mBufHdrTablePtr         = NULL;

	// init STOST trace stuff
	setTraceInfo(enscomp.getSharedTraceInfoPtr(), commonPortData.mPortIndex);
	mPortFsm->setTraceInfo(enscomp.getSharedTraceInfoPtr(), commonPortData.mPortIndex);

	mSharedChunk                         = 0;
	// Buffer sharing data initialization
	mBufSharingPortList.mBufSharingPorts = 0;
	mBufSharingPortList.numPorts         = 0;
	mIsAllocator                         = OMX_FALSE;

	m_bBufferSharingEnabled = true;
}

ImgEns_Port::~ImgEns_Port(void)
{
	if (mSharedChunk)
	{
		OMX_ERRORTYPE error = MMHwBuffer::Destroy(mSharedChunk);
		if (error != OMX_ErrorNone)
		{
			OstTraceInt1(TRACE_ERROR, "ENS_PROXY: ImgEns_Port::~ImgEns_Port error 0x%x of call to MMHwBuffer::Destroy\n", error);
		}
	}
	delete mPortFsm;
	if(mBufSharingPortList.mBufSharingPorts)
	{
		delete [] mBufSharingPortList.mBufSharingPorts;
	}
}

Img_ProcessingComponent & ImgEns_Port::getProcessingComponent()
{
	return mEnsComponent.getProcessingComponent();
}

const Img_ProcessingComponent & ImgEns_Port::getProcessingComponent() const
{
	return mEnsComponent.getProcessingComponent();
}

OMX_HANDLETYPE ImgEns_Port::getComponent(void) const
{
	OMX_HANDLETYPE hComp = mEnsComponent.getOMXHandle();
	IMGENS_ASSERT(hComp != 0);
	return hComp;
}

#if 0
inline ImgEns_Port * ImgEns_Port::getENSTunneledPort(void) const
{
	ImgEns_Component * tunneledComp = ::getImgEnsComponent(getTunneledComponent());
	if (tunneledComp)
		return tunneledComp->getPort(getTunneledPort());
	else
		return(NULL);
}
#endif

OMX_ERRORTYPE ImgEns_Port::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
	switch (nParamIndex)
	{
	case OMX_IndexParamCompBufferSupplier:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_BUFFERSUPPLIERTYPE);
			OMX_PARAM_BUFFERSUPPLIERTYPE *bufSupply =static_cast<OMX_PARAM_BUFFERSUPPLIERTYPE *> (pComponentParameterStructure);
			if(!getTunneledComponent())
			{// ignored if port not yet tunneled, should be changed to appropriate error later
				return OMX_ErrorNone;
			}

			if (bufSupply->eBufferSupplier != OMX_BufferSupplyUnspecified)
			{
				mBufferSupplier = bufSupply->eBufferSupplier;
			}
			else
			{
				return OMX_ErrorBadParameter;
			}

			if (getDirection() == OMX_DirInput && getTunneledComponent())
			{
				// if an input port gets this SetParameter, it means the IL
				// client is overriding the tunnel negotiation
				// the port is then responsible to signal the tunneled component
				bufSupply->nPortIndex = getTunneledPort();
				OMX_ERRORTYPE error = OMX_SetParameter( getTunneledComponent(), OMX_IndexParamCompBufferSupplier, bufSupply);
				if (error != OMX_ErrorNone)
					return error;
			}
			return OMX_ErrorNone;
		}
	case OMX_IndexParamPortDefinition:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_PORTDEFINITIONTYPE);
			OMX_PARAM_PORTDEFINITIONTYPE *portdef =static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>(pComponentParameterStructure);

			if (portdef->nBufferCountActual < mParamPortDefinition.nBufferCountMin)
			{
				return OMX_ErrorBadParameter;
			}

			mParamPortDefinition.nBufferCountActual = portdef->nBufferCountActual;
			if(!m_bBufferSharingEnabled)
			{
				return setFormatInPortDefinition(*portdef);
			}
			else
			{// Port Format must be set by the concrete port
				OMX_ERRORTYPE error =  setFormatInPortDefinition(*portdef);
				if(error!=OMX_ErrorNone)
					return error;
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

OMX_ERRORTYPE ImgEns_Port::updateSettings(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
	ImgEns_CmdUpdateSettingsEvt evt(nParamIndex, pComponentParameterStructure);
	return(Dispatch(&evt));
}

OMX_ERRORTYPE ImgEns_Port::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const
{
	switch (nParamIndex)
	{
	case OMX_IndexParamCompBufferSupplier:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_BUFFERSUPPLIERTYPE);
			OMX_PARAM_BUFFERSUPPLIERTYPE *bufSupply =static_cast<OMX_PARAM_BUFFERSUPPLIERTYPE *>(pComponentParameterStructure);
			bufSupply->eBufferSupplier = mBufferSupplier;
			return OMX_ErrorNone;
		}

	case OMX_IndexParamPortDefinition:
		{
			if(!m_bBufferSharingEnabled)
			{
				CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_PORTDEFINITIONTYPE);
				OMX_PARAM_PORTDEFINITIONTYPE *portdef =static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>(pComponentParameterStructure);
				*portdef = mParamPortDefinition;
				return OMX_ErrorNone;
			}
			else
			{
				//OMX_ERRORTYPE error;
				//OMX_U32 maxBufferSize = 0, maxBufferCountActual = 0;
				CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_PORTDEFINITIONTYPE);
				OMX_PARAM_PORTDEFINITIONTYPE *portdef = static_cast<OMX_PARAM_PORTDEFINITIONTYPE *> (pComponentParameterStructure);
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

OMX_ERRORTYPE ImgEns_Port::tunnelRequest(OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
	OMX_ERRORTYPE err;
	if (hTunneledComp == 0)
	{
		setTunneled(0, 0, getTunnelFlags());
		return OMX_ErrorNone;
	}
	if (mParamPortDefinition.nBufferCountMin == 0) {
		OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
		tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
		tunneledPortDef.nPortIndex  = nTunneledPort;
		getImgOmxIlSpecVersion(&tunneledPortDef.nVersion);
		err = OMX_GetParameter(hTunneledComp,
			OMX_IndexParamPortDefinition, &tunneledPortDef);
		if(err != OMX_ErrorNone) return err;
		if (tunneledPortDef.nBufferCountMin != 0) {
			OstTraceFiltInst0(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port:tunnelRequest return error because this port needs 0 buffer and tunneled port requires at least one");
			return OMX_ErrorPortsNotCompatible;
		}
	}

	if (mUseStandardTunneling == OMX_FALSE) 
	{
		// check if proprietary communication is possible
		err = proprietaryCommunicationRequest(hTunneledComp, nTunneledPort);
		if (err != OMX_ErrorNone) 
		{
			mUseStandardTunneling = OMX_TRUE;
		}
	}

	if (getDirection() == OMX_DirOutput) 
	{// Output Port
		pTunnelSetup->nTunnelFlags  = getTunnelFlags();
		pTunnelSetup->eSupplier     = getSupplierPreference();
		setTunneled(hTunneledComp, nTunneledPort, pTunnelSetup->nTunnelFlags);
		return OMX_ErrorNone;
	}
	else 
	{// Input Port
		OMX_BUFFERSUPPLIERTYPE bufferSupplierChoice;
		err = checkCompatibility(hTunneledComp, nTunneledPort);
		if (err != OMX_ErrorNone) 
			return err;
		setTunneled(hTunneledComp, nTunneledPort, pTunnelSetup->nTunnelFlags);
		// make a best guess on who should supply the buffer
		if (getSupplierPreference() == OMX_BufferSupplyOutput || pTunnelSetup->eSupplier == OMX_BufferSupplyOutput) 
		{// at least one port prefers output so select output
			bufferSupplierChoice = OMX_BufferSupplyOutput;
		}
		else if ( getSupplierPreference() == OMX_BufferSupplyUnspecified && pTunnelSetup->eSupplier == OMX_BufferSupplyUnspecified) 
		{// both ports don't care so select output
			bufferSupplierChoice = OMX_BufferSupplyOutput;
		}
		else 
		{
			bufferSupplierChoice = OMX_BufferSupplyInput;
		}

		// Inform output port of which port supplies the buffer
		OMX_PARAM_BUFFERSUPPLIERTYPE bufSupply;
		bufSupply.nSize         = sizeof(bufSupply);
		getImgOmxIlSpecVersion(&bufSupply.nVersion);
		bufSupply.nPortIndex      = nTunneledPort;
		bufSupply.eBufferSupplier = bufferSupplierChoice;

		err = OMX_SetParameter(hTunneledComp, OMX_IndexParamCompBufferSupplier, &bufSupply);
		if (err != OMX_ErrorNone)
			return err;
		setBufferSupplier(bufferSupplierChoice);
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Port::standardTunnelingInit(void)
{
	OMX_BUFFERHEADERTYPE *  pBufferHdr;
	OMX_U8 *                pBuffer;
	OMX_ERRORTYPE           error;

	OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
	OMX_U32 bufferSize = 0, bufferCountActual = 0;
	if(!m_bBufferSharingEnabled)
	{
		tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
		tunneledPortDef.nPortIndex  = getTunneledPort();
		getImgOmxIlSpecVersion(&tunneledPortDef.nVersion);
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
	}
	else
	{
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
	if(bufferCountActual != 0) 
	{
		mBufHdrTablePtr = new ImgEns_BUFFERHEADER_TABLETYPE [bufferCountActual];
		if (!mBufHdrTablePtr)  return OMX_ErrorInsufficientResources;

		for (unsigned int i = 0; i < bufferCountActual; i++) 
		{
			void *bufferAllocInfo, *portPrivateInfo;
			mBufHdrTablePtr[i].mBufHdrPtr      = 0;
			mBufHdrTablePtr[i].mBufHdrStatus   = 0;
			mBufHdrTablePtr[i].mAtSharingPorts = 0;
			error = getProcessingComponent().allocateBuffer(mParamPortDefinition.nPortIndex, i, bufferSize, &pBuffer, &bufferAllocInfo, &portPrivateInfo);
			if(error != OMX_ErrorNone) 
				return error;
			if(!m_bBufferSharingEnabled)
			{
				if (i==0 && mSharedChunk) 
				{// Send the config for shared chunk
					error = mSharedChunk->SetConfigExtension(getTunneledComponent(), getTunneledPort());
				}
				error = OMX_UseBuffer(getTunneledComponent(), &pBufferHdr, mTunneledPort, 0, bufferSize, pBuffer);
				if(error != OMX_ErrorNone) 
					return error;
			}
			else
			{
				error =  forwardBuffer(&pBufferHdr,0,bufferSize, pBuffer,bufferAllocInfo,portPrivateInfo);
				if(error != OMX_ErrorNone) return error;
			}
			pBufferHdr->pPlatformPrivate = bufferAllocInfo;
			if (mParamPortDefinition.eDir == OMX_DirInput) 
			{
				pBufferHdr->pInputPortPrivate   = portPrivateInfo;
				pBufferHdr->nInputPortIndex     = mParamPortDefinition.nPortIndex;
			} 
			else 
			{
				pBufferHdr->pOutputPortPrivate  = portPrivateInfo;
				pBufferHdr->nOutputPortIndex    = mParamPortDefinition.nPortIndex;
			}

			error = getProcessingComponent().useBufferHeader(mParamPortDefinition.eDir, pBufferHdr);
			if(error != OMX_ErrorNone) 
				return error;

			mBufHdrTablePtr[i].mBufHdrPtr      = pBufferHdr;
			mBufHdrTablePtr[i].mBufHdrStatus   = ImgEns_PORT_BUFFER_ALLOCATED | ImgEns_PORT_BUFFER_ATCOMPONENT;
			mBufHdrTablePtr[i].mAtSharingPorts = 0;
			mBufferCountCurrent++;
		}
	}
	mParamPortDefinition.bPopulated = OMX_TRUE;
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Port::standardTunnelingDeInit(void)
{
	OMX_ERRORTYPE           error;
	void *                  portPrivateInfo;

	for (unsigned int i = 0; i < mParamPortDefinition.nBufferCountActual; i++) 
	{
		OMX_BUFFERHEADERTYPE * pBufferHdr = mBufHdrTablePtr[i].mBufHdrPtr;

		IMGENS_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ALLOCATED)
			&& (mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ATCOMPONENT));

		mBufferCountCurrent--;

		if (mParamPortDefinition.eDir == OMX_DirInput) 
		{
			portPrivateInfo = pBufferHdr->pInputPortPrivate;
		} 
		else 
		{
			portPrivateInfo = pBufferHdr->pOutputPortPrivate;
		}

		error = getProcessingComponent().freeBuffer(mParamPortDefinition.nPortIndex, mBufferCountCurrent, OMX_TRUE, pBufferHdr->pPlatformPrivate, portPrivateInfo);
		if(error != OMX_ErrorNone) 
			return error;

		error = OMX_FreeBuffer(getTunneledComponent(), mTunneledPort, pBufferHdr);
		if(error != OMX_ErrorNone) 
			return error;
		mBufHdrTablePtr[i].mBufHdrPtr    = 0;
		mBufHdrTablePtr[i].mBufHdrStatus = 0;
	}
	mSharedChunk = 0;
	mParamPortDefinition.bPopulated = OMX_FALSE;
	delete [] mBufHdrTablePtr;
	mBufHdrTablePtr = 0;
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Port::standardTunnelingTransfertInit(void)
{
	OMX_ERRORTYPE error;

	if (!isBufferSupplier())    
		return OMX_ErrorNone;
	if(m_bBufferSharingEnabled)
	{
		if (!isAllocatorPort())     
			return OMX_ErrorNone;
	}

	for (unsigned int i = 0; i < mParamPortDefinition.nBufferCountActual; i++) 
	{
		OMX_BUFFERHEADERTYPE * pBufferHdr = mBufHdrTablePtr[i].mBufHdrPtr;

		if (!mBufHdrTablePtr[i].mBufHdrStatus) 
			continue;
		if (mBufHdrTablePtr[i].mBufHdrStatus != (ImgEns_PORT_BUFFER_ALLOCATED | ImgEns_PORT_BUFFER_ATCOMPONENT)) 
			continue;

		if ( mParamPortDefinition.eDir == OMX_DirOutput ) 
		{
			if(!m_bBufferSharingEnabled){
				bufferSentToProcessingComponent(pBufferHdr);
				error = getProcessingComponent().fillThisBuffer(pBufferHdr);
				if (error != OMX_ErrorNone) 
				{
					bufferReturnedFromProcessingComp(pBufferHdr);
					return error;
				}
			}
			else
			{
				// Buffer-supplier output ports must send empty buffers to Processing Component
				if(getNumberInterConnectedPorts())
				{
					error = forwardOutputBuffer(pBufferHdr);
					if(error!=OMX_ErrorNone) 
						return error;
					bufferSentToSharingPort(pBufferHdr);
				}
				else
				{
					bufferSentToProcessingComponent(pBufferHdr);
					error = getProcessingComponent().fillThisBuffer(pBufferHdr);
					if (error != OMX_ErrorNone) 
					{
						bufferReturnedFromProcessingComp(pBufferHdr);
						return error;
					}
				}
			}
		} 
		else 
		{
			// Buffer-supplier input ports must send empty buffers to tunneled port
			bufferSentToNeighbor(pBufferHdr);
			error = OMX_FillThisBuffer(mTunneledComponent, pBufferHdr);
			if (error != OMX_ErrorNone) 
			{
				bufferReturnedFromNeighbor(pBufferHdr);
				return error;
			}
		}
	}
	return OMX_ErrorNone;
}


OMX_BUFFERHEADERTYPE * ImgEns_Port::allocateBufferHeader(OMX_PTR pAppPrivate, OMX_U32 nSizeBytes)
{
	OMX_BUFFERHEADERTYPE *  pBufferHdr = 0;
	pBufferHdr = new OMX_BUFFERHEADERTYPE;
	if (!pBufferHdr) 
		return 0;

	//Store pointer to BufferHeader in Port
	if (mBufferCountCurrent == 0) 
	{
		mBufHdrTablePtr= new ImgEns_BUFFERHEADER_TABLETYPE [mParamPortDefinition.nBufferCountActual];
		if (!mBufHdrTablePtr)  return 0;
	}

	mBufHdrTablePtr[mBufferCountCurrent].mBufHdrPtr      = pBufferHdr;
	mBufHdrTablePtr[mBufferCountCurrent].mBufHdrStatus   = ImgEns_PORT_BUFFER_ALLOCATED | ImgEns_PORT_BUFFER_ATNEIGHBOR;
	mBufHdrTablePtr[mBufferCountCurrent].mAtSharingPorts = 0;


	pBufferHdr->nSize = sizeof(OMX_BUFFERHEADERTYPE);
	getImgOmxIlSpecVersion(&pBufferHdr->nVersion);

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

	if (mParamPortDefinition.eDir == OMX_DirInput) 
	{
		pBufferHdr->pOutputPortPrivate  = 0;
		pBufferHdr->nInputPortIndex     = mParamPortDefinition.nPortIndex;
		pBufferHdr->nOutputPortIndex    = getTunneledPort();
	}
	else 
	{
		pBufferHdr->pInputPortPrivate   = 0;
		pBufferHdr->nInputPortIndex     = getTunneledPort();
		pBufferHdr->nOutputPortIndex    = mParamPortDefinition.nPortIndex;
	}

	return pBufferHdr;
}

void ImgEns_Port::freeBufferHeader(OMX_BUFFERHEADERTYPE *pBufferHdr) 
{
	delete pBufferHdr;
	if (mBufferCountCurrent == 0) 
	{
		delete [] mBufHdrTablePtr;
		mBufHdrTablePtr = 0;
	}
}

OMX_ERRORTYPE ImgEns_Port::useBuffer(OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8* pBuffer)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferHdr;
	void *portPrivateInfo;

	if (nSizeBytes < mParamPortDefinition.nBufferSize) 
	{
		return OMX_ErrorBadParameter;
	}
	pBufferHdr = allocateBufferHeader(pAppPrivate, nSizeBytes);
	if(!pBufferHdr) 
		return OMX_ErrorInsufficientResources;

	pBufferHdr->pBuffer = pBuffer;
	*ppBufferHdr        = pBufferHdr;
	mIsBufferAllocator  = false;

	error = getProcessingComponent().useBuffer(mParamPortDefinition.nPortIndex, mBufferCountCurrent, pBufferHdr, &portPrivateInfo);
	if(error != OMX_ErrorNone) 
		return error;

	if (mParamPortDefinition.eDir == OMX_DirInput) 
	{
		pBufferHdr->pInputPortPrivate = portPrivateInfo;
	} 
	else 
	{
		pBufferHdr->pOutputPortPrivate = portPrivateInfo;
	}

	mBufferCountCurrent++;

	if (mBufferCountCurrent == mParamPortDefinition.nBufferCountActual) 
	{
		mParamPortDefinition.bPopulated = OMX_TRUE;
	}
	if(m_bBufferSharingEnabled)
	{
		error =  forwardBuffer(0,pAppPrivate,nSizeBytes,pBuffer,0,0);
		if(error != OMX_ErrorNone) 
			return error;
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Port::allocateBuffer(OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes)
{
	OMX_BUFFERHEADERTYPE *  pBufferHdr;
	OMX_U8 *                pBuffer;
	OMX_ERRORTYPE           error;
	void *                  bufferAllocInfo;
	void *                  portPrivateInfo;

	if (nSizeBytes<mParamPortDefinition.nBufferSize) 
	{
		return OMX_ErrorBadParameter;
	}

	if (getTunneledComponent() != 0) 
	{
		return OMX_ErrorIncorrectStateOperation;
	}

	pBufferHdr = allocateBufferHeader(pAppPrivate, nSizeBytes);
	if(!pBufferHdr) 
		return OMX_ErrorInsufficientResources;

	error = getProcessingComponent().allocateBuffer(mParamPortDefinition.nPortIndex, mBufferCountCurrent, nSizeBytes, &pBuffer, &bufferAllocInfo, &portPrivateInfo);
	if(error != OMX_ErrorNone) 
		return error;

	pBufferHdr->pBuffer             = pBuffer;
	pBufferHdr->pPlatformPrivate    = bufferAllocInfo;

	*ppBufferHdr = pBufferHdr;

	if (mParamPortDefinition.eDir == OMX_DirInput) 
	{
		pBufferHdr->pInputPortPrivate = portPrivateInfo;
	} 
	else 
	{
		pBufferHdr->pOutputPortPrivate = portPrivateInfo;
	}

	error = getProcessingComponent().useBufferHeader(mParamPortDefinition.eDir, pBufferHdr);
	if(error != OMX_ErrorNone) 
		return error;

	mBufferCountCurrent++;
	mIsBufferAllocator = true;

	if (mBufferCountCurrent == mParamPortDefinition.nBufferCountActual) 
	{
		mParamPortDefinition.bPopulated = OMX_TRUE;
	}
	return OMX_ErrorNone;
}


OMX_ERRORTYPE ImgEns_Port::freeBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_ERRORTYPE   error;
	OMX_PTR         portPrivateInfo;
	OMX_BOOL        bufferAllocated;

	for (unsigned int i = 0; i < mParamPortDefinition.nBufferCountActual; i++) 
	{
		OMX_BUFFERHEADERTYPE *pBufferHdr = mBufHdrTablePtr[i].mBufHdrPtr;
		if (pBufferHdr == pBuffer) 
		{
			IMGENS_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ALLOCATED) && (mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ATNEIGHBOR));
			mBufferCountCurrent--;
			mParamPortDefinition.bPopulated = OMX_FALSE;
			if (mParamPortDefinition.eDir == OMX_DirInput) 
			{
				portPrivateInfo = pBufferHdr->pInputPortPrivate;
			} 
			else 
			{
				portPrivateInfo = pBufferHdr->pOutputPortPrivate;
			}

			if (getTunneledComponent() == 0 && mIsBufferAllocator) 
			{
				bufferAllocated = OMX_TRUE;
			} 
			else 
			{
				bufferAllocated = OMX_FALSE;
			}

			error = getProcessingComponent().freeBuffer(mParamPortDefinition.nPortIndex, mBufferCountCurrent, bufferAllocated, pBufferHdr->pPlatformPrivate, portPrivateInfo);
			if (error != OMX_ErrorNone) 
				return error;

			mBufHdrTablePtr[i].mBufHdrPtr    = 0;
			mBufHdrTablePtr[i].mBufHdrStatus = 0;
			freeBufferHeader(pBufferHdr);
			if (mSharedChunk) 
			{
				if (mBufferCountCurrent==0) 
				{ // ER342234
					if (!bufferAllocated) 
					{
						error = MMHwBuffer::Close(mSharedChunk);
						IMGENS_ASSERT(error == OMX_ErrorNone);
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


OMX_ERRORTYPE ImgEns_Port::getBufferHdrTableIndex(OMX_BUFFERHEADERTYPE* pBuffer, OMX_U32& Index)
{
	for (OMX_U32 i = 0; i < mParamPortDefinition.nBufferCountActual; i++) 
	{
		OMX_BUFFERHEADERTYPE *pBufferHdr = mBufHdrTablePtr[i].mBufHdrPtr;
		if (pBuffer == pBufferHdr) 
		{
			Index = i;
			return OMX_ErrorNone;
		}
	}
	return OMX_ErrorUndefined;
}

OMX_ERRORTYPE ImgEns_Port::mapSharedBufferHeader(OMX_BUFFERHEADERTYPE* pBuffer, OMX_BUFFERHEADERTYPE** ppLocalBuffer)
{
	for (OMX_U32 i = 0; i < mParamPortDefinition.nBufferCountActual; i++) 
	{
		OMX_BUFFERHEADERTYPE *pBufferHdr = mBufHdrTablePtr[i].mBufHdrPtr;
		if (pBuffer->pBuffer == pBufferHdr->pBuffer) 
		{
			*ppLocalBuffer = pBufferHdr;
			return OMX_ErrorNone;
		}
	}
	return OMX_ErrorUndefined;
}


OMX_BOOL ImgEns_Port::isAllBuffersAtOwner()
{
	OMX_U32 i;
	if(useProprietaryCommunication()) return OMX_TRUE;
	if(useStandardTunneling() && isBufferSupplier())
	{
		if(!m_bBufferSharingEnabled)
		{
			for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) 
			{
				if (!mBufHdrTablePtr[i].mBufHdrStatus) 
					continue;
				if (mBufHdrTablePtr[i].mBufHdrStatus != (ImgEns_PORT_BUFFER_ALLOCATED | ImgEns_PORT_BUFFER_ATCOMPONENT))
				{
					return OMX_FALSE;
				}
			}
		}
		else
		{
			if(isAllocatorPort()) 
			{
				for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) 
				{
					if (!mBufHdrTablePtr[i].mBufHdrStatus) 
						continue;
					if (mBufHdrTablePtr[i].mBufHdrStatus != (ImgEns_PORT_BUFFER_ALLOCATED | ImgEns_PORT_BUFFER_ATCOMPONENT))
					{
						return OMX_FALSE;
					}
				}
			}
			else
			{
				for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) 
				{
					if (!mBufHdrTablePtr[i].mBufHdrStatus) 
						continue;
					if (mBufHdrTablePtr[i].mBufHdrStatus != (ImgEns_PORT_BUFFER_ALLOCATED | ImgEns_PORT_BUFFER_ATSHARINGPORT))
					{
						return OMX_FALSE;
					}
				}
			}
		}
	} 
	else 
	{
		for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) 
		{
			if (!mBufHdrTablePtr[i].mBufHdrStatus) 
				continue;
			if (mBufHdrTablePtr[i].mBufHdrStatus != (ImgEns_PORT_BUFFER_ALLOCATED | ImgEns_PORT_BUFFER_ATNEIGHBOR))
			{
				return OMX_FALSE;
			}
		}
	}
	return OMX_TRUE;
}

OMX_BOOL ImgEns_Port::isAllBuffersReturnedFromNeighbor()
{
	OMX_U32 i;
	if(!useStandardTunneling()) 
		return OMX_TRUE;
	for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) 
	{
		if (!mBufHdrTablePtr[i].mBufHdrStatus) continue;
		if (mBufHdrTablePtr[i].mBufHdrStatus == (ImgEns_PORT_BUFFER_ALLOCATED | ImgEns_PORT_BUFFER_ATNEIGHBOR)){
			return OMX_FALSE;
		}
	}
	return OMX_TRUE;
}


OMX_BOOL ImgEns_Port::isAllBuffersReturnedFromSharingPorts()
{
	OMX_U32 i;
	if( !useStandardTunneling() ) 
		return OMX_TRUE;
	for (i = 0; i < mParamPortDefinition.nBufferCountActual; i++) 
	{
		if (!mBufHdrTablePtr[i].mBufHdrStatus) continue;
		if (mBufHdrTablePtr[i].mBufHdrStatus == (ImgEns_PORT_BUFFER_ALLOCATED | ImgEns_PORT_BUFFER_ATSHARINGPORT))
		{
			return OMX_FALSE;
		}
	}
	return OMX_TRUE;
}


void ImgEns_Port::bufferSentToProcessingComponent(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_U32 i = 0;
	OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port:bufferSentToProcessingComp  0x%x", (unsigned int)pBuffer);
	IMGENS_ASSERT(OMX_ErrorNone == getBufferHdrTableIndex(pBuffer,i));
	IMGENS_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ALLOCATED) && (mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ATCOMPONENT));
	mBufHdrTablePtr[i].mBufHdrStatus |= ImgEns_PORT_BUFFER_ATPROCESSINGCOMP;
	mBufHdrTablePtr[i].mBufHdrStatus &= ~ImgEns_PORT_BUFFER_ATCOMPONENT;
}

void ImgEns_Port::bufferReturnedFromProcessingComp(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_U32 i = 0;
	OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ImgEns_Port:bufferReturnedFromProcessingComp  0x%x", (unsigned int)pBuffer);
	IMGENS_ASSERT(OMX_ErrorNone == getBufferHdrTableIndex(pBuffer,i));
	IMGENS_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ALLOCATED)
		&& (mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ATPROCESSINGCOMP));
	mBufHdrTablePtr[i].mBufHdrStatus &= ~ImgEns_PORT_BUFFER_ATPROCESSINGCOMP;
	mBufHdrTablePtr[i].mBufHdrStatus |= ImgEns_PORT_BUFFER_ATCOMPONENT;
}

void ImgEns_Port::bufferSentToNeighbor(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_U32 i = 0;
	IMGENS_ASSERT(OMX_ErrorNone == getBufferHdrTableIndex(pBuffer,i));
	IMGENS_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ALLOCATED)
		&& (mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ATCOMPONENT));
	mBufHdrTablePtr[i].mBufHdrStatus |= ImgEns_PORT_BUFFER_ATNEIGHBOR;
	mBufHdrTablePtr[i].mBufHdrStatus &= ~ImgEns_PORT_BUFFER_ATCOMPONENT;
}

void ImgEns_Port::bufferReturnedFromNeighbor(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_U32 i = 0;
	IMGENS_ASSERT(OMX_ErrorNone == getBufferHdrTableIndex(pBuffer,i));
	IMGENS_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ALLOCATED)
		&& (mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ATNEIGHBOR));
	mBufHdrTablePtr[i].mBufHdrStatus &= ~ImgEns_PORT_BUFFER_ATNEIGHBOR;
	mBufHdrTablePtr[i].mBufHdrStatus |= ImgEns_PORT_BUFFER_ATCOMPONENT;
}



OMX_ERRORTYPE ImgEns_Port::calculateBufferReqs(OMX_U32 *pMaxBufferSize, OMX_U32 *pMaxBufferCount) const
{// Calculate requirement components
	OMX_ERRORTYPE error;
	OMX_U32 maxBufferSize=0;
	OMX_U32 maxBufferCount = 0;

	OMX_BOOL isSharingPortContributing = OMX_FALSE;
	OMX_BOOL isTunneledPortContributing=  OMX_FALSE;

	switch(getPortRole())
	{
	case ImgEns_SHARING_PORT:
		isSharingPortContributing  = OMX_FALSE;
		isTunneledPortContributing = OMX_TRUE;
		break;
	case ImgEns_NON_TUNNELED_PORT:
	case ImgEns_NON_SUPPLIER_PORT:
		isSharingPortContributing  = OMX_TRUE;
		isTunneledPortContributing = OMX_FALSE;
		break;
	case ImgEns_ALLOCATOR_PORT:
		isSharingPortContributing  = OMX_TRUE;
		isTunneledPortContributing = OMX_TRUE;
		break;
	}

	if(isSharingPortContributing) 
	{
		OMX_U32 bufferSize=0;
		OMX_U32 bufferCount = 0;

		//Are there any sharing ports
		if(getNumberInterConnectedPorts()) 
		{
			error = getSharingReqs(&bufferSize,&bufferCount);
			if(error!=OMX_ErrorNone) return error;
		}
		maxBufferSize = MAX(bufferSize, maxBufferSize);
		maxBufferCount = MAX(bufferCount,maxBufferCount);
	}

	if(isTunneledPortContributing) 
	{
		OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
		tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
		tunneledPortDef.nPortIndex  = getTunneledPort();
		getImgOmxIlSpecVersion(&tunneledPortDef.nVersion);
		error = OMX_GetParameter(getTunneledComponent(), OMX_IndexParamPortDefinition, &tunneledPortDef);
		if(error != OMX_ErrorNone) 
			return error;
		maxBufferSize = MAX(maxBufferSize, tunneledPortDef.nBufferSize);
		maxBufferCount = MAX(maxBufferCount,tunneledPortDef.nBufferCountActual);
	}
	*pMaxBufferSize  = maxBufferSize;
	*pMaxBufferCount = maxBufferCount;
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Port::getSharingReqs(OMX_U32 *pMaxBufferSize, OMX_U32 *pMaxBufferCount) const
{
	OMX_ERRORTYPE error;
	OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
	ImgEns_Port* pPort;
	OMX_U32 maxBufferSize=0;
	OMX_U32 maxBufferCount =0;
	ImgEns_CmdGetParamEvt evt(OMX_IndexParamPortDefinition, &tunneledPortDef);

	for(OMX_U32 i=0;i<getNumberInterConnectedPorts();i++)
	{
		pPort = *(mBufSharingPortList.mBufSharingPorts + i);
		tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
		tunneledPortDef.nPortIndex  = pPort->getPortIndex();
		getImgOmxIlSpecVersion(&tunneledPortDef.nVersion);
		error=pPort->Dispatch(&evt);
		/*
		portFsm = pPort->getPortFsm();

		error = portFsm->dispatch(&evt); */
		if(error!=OMX_ErrorNone)
			return error;

		maxBufferSize = MAX(maxBufferSize, tunneledPortDef.nBufferSize);
		maxBufferCount = MAX(maxBufferCount,tunneledPortDef.nBufferCountActual);
	}

	*pMaxBufferSize = maxBufferSize;
	*pMaxBufferCount = maxBufferCount;
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Port::setSharingReqs(OMX_U32 bufferCountActual){
	// This call forwards only buffercountactual field along the chain
	OMX_ERRORTYPE error;
	OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
	ImgEns_Port* pPort;
	ImgEns_CmdGetParamEvt getParamEvt(OMX_IndexParamPortDefinition, &tunneledPortDef);
	ImgEns_CmdSetParamEvt setparamEvt(OMX_IndexParamPortDefinition, &tunneledPortDef);

	for(OMX_U32 i=0;i<getNumberInterConnectedPorts();i++) 
	{
		pPort = *(mBufSharingPortList.mBufSharingPorts + i);
		tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
		tunneledPortDef.nPortIndex  = pPort->getPortIndex();
		getImgOmxIlSpecVersion(&tunneledPortDef.nVersion);

		error=Dispatch(&getParamEvt);
		if(error!=OMX_ErrorNone)
			return error;

		// set port settings
		tunneledPortDef.nBufferCountActual = bufferCountActual;
		error=Dispatch(&setparamEvt);
		if(error!=OMX_ErrorNone)
			return error;
	}

	return OMX_ErrorNone;
}


OMX_ERRORTYPE ImgEns_Port::sendBufferReqs(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentParameterStructure) 
{
	OMX_BOOL isSharingPortContributing = OMX_FALSE;
	OMX_BOOL isTunneledPortContributing=  OMX_FALSE;
	OMX_U32 bufferCountActual=0;
	OMX_ERRORTYPE error;

	if(nParamIndex!=OMX_IndexParamPortDefinition) return OMX_ErrorNone;

	CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_PORTDEFINITIONTYPE);
	OMX_PARAM_PORTDEFINITIONTYPE *portdef = static_cast<OMX_PARAM_PORTDEFINITIONTYPE *> (pComponentParameterStructure);
	bufferCountActual = portdef->nBufferCountActual;

	IMGENS_ASSERT(bufferCountActual);

	switch(getPortRole()) 
	{
	case ImgEns_SHARING_PORT:
		isSharingPortContributing  = OMX_FALSE;
		isTunneledPortContributing = OMX_TRUE;
		break;
	case ImgEns_NON_TUNNELED_PORT:
	case ImgEns_NON_SUPPLIER_PORT:
		isSharingPortContributing  = OMX_TRUE;
		isTunneledPortContributing = OMX_FALSE;
		break;
	case ImgEns_ALLOCATOR_PORT:
		isSharingPortContributing  = OMX_TRUE;
		isTunneledPortContributing = OMX_TRUE;
		break;
	}


	if(isSharingPortContributing) 
	{//Are there any sharing ports
		if(getNumberInterConnectedPorts()) 
		{
			error = setSharingReqs(bufferCountActual);
			if(error!=OMX_ErrorNone) return error;
		}
	}

	if(isTunneledPortContributing) 
	{
		OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
		tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
		tunneledPortDef.nPortIndex  = getTunneledPort();
		getImgOmxIlSpecVersion(&tunneledPortDef.nVersion);
		error = OMX_GetParameter(getTunneledComponent(),
			OMX_IndexParamPortDefinition, &tunneledPortDef);
		if(error != OMX_ErrorNone) 
			return error;

		// We have to call setparameter to remove any mismatch along the chain
		tunneledPortDef.nBufferCountActual = bufferCountActual;
		error = OMX_SetParameter(getTunneledComponent(),
			OMX_IndexParamPortDefinition, &tunneledPortDef);
		if (error != OMX_ErrorNone) 
			return error;
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Port::forwardBuffer(OMX_BUFFERHEADERTYPE **ppBufferHdr,OMX_PTR pAppPrivate, OMX_U32 bufferSize, OMX_U8* pBuffer, void * /*bufferAllocInfo*/,void * /*portPrivateInfo*/)
{
	OMX_ERRORTYPE error;
	OMX_BOOL isSharingPortContributing  = OMX_FALSE;
	OMX_BOOL isTunneledPortContributing=  OMX_FALSE;
	switch(getPortRole()) 
	{
	case ImgEns_SHARING_PORT:
		isSharingPortContributing  = OMX_FALSE;
		isTunneledPortContributing = OMX_TRUE;
		break;
	case ImgEns_NON_TUNNELED_PORT:
	case ImgEns_NON_SUPPLIER_PORT:
		isSharingPortContributing  = OMX_TRUE;
		isTunneledPortContributing = OMX_FALSE;
		break;
	case ImgEns_ALLOCATOR_PORT:
		isSharingPortContributing  = OMX_TRUE;
		isTunneledPortContributing = OMX_TRUE;
		break;
	}

	if(isSharingPortContributing && getNumberInterConnectedPorts()!=0) 
	{//Forward the buffer to sharing ports
		// This call forwards only buffercountactual field along the chain
		OMX_ERRORTYPE error;
		ImgEns_Port* pPort;
		ImgEns_CmdUseSharedBufferEvt evt(0, 0, pAppPrivate, bufferSize,pBuffer, (void*)getSharedChunk() );
		for(OMX_U32 i=0;i<getNumberInterConnectedPorts();i++) 
		{
			pPort = *(mBufSharingPortList.mBufSharingPorts + i);
			error=Dispatch(&evt);
			if(error!=OMX_ErrorNone)
				return error;
		}
	}

	if(isTunneledPortContributing) 
	{
		if (mBufferCountCurrent==0 && mSharedChunk) 
		{// Send the config for shared chunk
			error = mSharedChunk->SetConfigExtension(getTunneledComponent(), getTunneledPort());
		}
		// Forward the buffer to tunneled port.
		error = OMX_UseBuffer(getTunneledComponent(), ppBufferHdr, mTunneledPort, pAppPrivate, bufferSize, pBuffer);
		if(error != OMX_ErrorNone) 
			return error;
	}
	return OMX_ErrorNone;
}


OMX_ERRORTYPE ImgEns_Port::forwardInputBuffer(OMX_BUFFERHEADERTYPE *pBufferHdr)
{
	OMX_ERRORTYPE error;
	ImgEns_Port* pPort;
	ImgEns_CmdEmptyThisSharedBufferEvt emptyThisSharedBufferEvt(pBufferHdr);

	for(OMX_U32 i=0;i<getNumberInterConnectedPorts();i++) {
		pPort = *(mBufSharingPortList.mBufSharingPorts + i);
		error=Dispatch(&emptyThisSharedBufferEvt);
		if(error!=OMX_ErrorNone)
			return error;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Port::forwardOutputBuffer(OMX_BUFFERHEADERTYPE *pBufferHdr)
{
	OMX_ERRORTYPE error;
	ImgEns_Port* pPort;
	ImgEns_CmdFillThisSharedBufferEvt fillThisSharedBufferEvt(pBufferHdr);

	for(OMX_U32 i=0;i<getNumberInterConnectedPorts();i++)
	{
		pPort = *(mBufSharingPortList.mBufSharingPorts + i);
		error=Dispatch(&fillThisSharedBufferEvt);
		if(error!=OMX_ErrorNone)
			return error;
	}
	return OMX_ErrorNone;
}


void ImgEns_Port::bufferSentToSharingPort(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_U32 i = 0;
	IMGENS_ASSERT(OMX_ErrorNone == getBufferHdrTableIndex(pBuffer,i));
	IMGENS_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ALLOCATED)
		&& (mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ATCOMPONENT));
	mBufHdrTablePtr[i].mBufHdrStatus |= ImgEns_PORT_BUFFER_ATSHARINGPORT;
	mBufHdrTablePtr[i].mBufHdrStatus &= ~ImgEns_PORT_BUFFER_ATCOMPONENT;
	mBufHdrTablePtr[i].mAtSharingPorts = getNumberInterConnectedPorts();
}


OMX_BOOL ImgEns_Port::bufferReturnedFromSharingPort(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_U32 i = 0;
	IMGENS_ASSERT(OMX_ErrorNone == getBufferHdrTableIndex(pBuffer,i));
	IMGENS_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ALLOCATED)
		&& (mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ATSHARINGPORT));

	mBufHdrTablePtr[i].mAtSharingPorts--;
	if(!mBufHdrTablePtr[i].mAtSharingPorts)
	{// Shared Buffer returned by all ports
		mBufHdrTablePtr[i].mBufHdrStatus &= ~ImgEns_PORT_BUFFER_ATSHARINGPORT;
		mBufHdrTablePtr[i].mBufHdrStatus |= ImgEns_PORT_BUFFER_ATCOMPONENT;
		return OMX_TRUE;
	}
	return OMX_FALSE;
}

OMX_ERRORTYPE ImgEns_Port::useSharedBuffer(OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8* pBuffer,void *pSharedChunk)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferHdr;
	void *                bufferAllocInfo;
	void *                portPrivateInfo;
	MMHwBuffer *sharedChunk = (MMHwBuffer *)pSharedChunk;

	if (nSizeBytes < mParamPortDefinition.nBufferSize) 
	{
		return OMX_ErrorBadParameter;
	}

	IMGENS_ASSERT(getTunneledComponent()!=0);

	if (mBufferCountCurrent == 0)
	{
		mBufHdrTablePtr= new ImgEns_BUFFERHEADER_TABLETYPE [mParamPortDefinition.nBufferCountActual];
		if (!mBufHdrTablePtr)
			return OMX_ErrorInsufficientResources;

		setSharedChunk(sharedChunk);
		if (mSharedChunk)
		{// Send the config for shared chunk
			error = mSharedChunk->SetConfigExtension(getTunneledComponent(), getTunneledPort());
		}
	}

	mBufHdrTablePtr[mBufferCountCurrent].mBufHdrPtr    = 0;
	mBufHdrTablePtr[mBufferCountCurrent].mBufHdrStatus = 0;


	error = getProcessingComponent().getMMHWBufferInfo(mParamPortDefinition.nPortIndex, nSizeBytes, pBuffer, &bufferAllocInfo, &portPrivateInfo);
	if(error != OMX_ErrorNone) 
		return error;


	error = OMX_UseBuffer(getTunneledComponent(), &pBufferHdr ,mTunneledPort, pAppPrivate, nSizeBytes, pBuffer);
	if(error != OMX_ErrorNone) 
		return error;


	pBufferHdr->pPlatformPrivate = bufferAllocInfo;
	if (mParamPortDefinition.eDir == OMX_DirInput) 
	{
		pBufferHdr->pInputPortPrivate   = portPrivateInfo;
		pBufferHdr->nInputPortIndex     = mParamPortDefinition.nPortIndex;
	} 
	else 
	{
		pBufferHdr->pOutputPortPrivate  = portPrivateInfo;
		pBufferHdr->nOutputPortIndex    = mParamPortDefinition.nPortIndex;
	}

	error = getProcessingComponent().useBufferHeader(mParamPortDefinition.eDir, pBufferHdr);
	if(error != OMX_ErrorNone) 
		return error;

	mBufHdrTablePtr[mBufferCountCurrent].mBufHdrPtr    = pBufferHdr;
	mBufHdrTablePtr[mBufferCountCurrent].mBufHdrStatus = ImgEns_PORT_BUFFER_ALLOCATED | ImgEns_PORT_BUFFER_ATSHARINGPORT;
	// mAtSharingPorts should have value 1 in any case.
	IMGENS_ASSERT( getNumberInterConnectedPorts()==1);
	mBufHdrTablePtr[mBufferCountCurrent].mAtSharingPorts = getNumberInterConnectedPorts();

	mBufferCountCurrent++;

	if (mBufferCountCurrent == mParamPortDefinition.nBufferCountActual) 
	{
		mParamPortDefinition.bPopulated = OMX_TRUE;
	}
	return OMX_ErrorNone;
}


OMX_ERRORTYPE ImgEns_Port::bufferSharingDeinit()
{
	OMX_ERRORTYPE   error;
	void          * portPrivateInfo;

	for (unsigned int i = 0; i < mParamPortDefinition.nBufferCountActual; i++)
	{
		OMX_BUFFERHEADERTYPE * pBufferHdr = mBufHdrTablePtr[i].mBufHdrPtr;
		IMGENS_ASSERT((mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ALLOCATED)
			&& (mBufHdrTablePtr[i].mBufHdrStatus & ImgEns_PORT_BUFFER_ATSHARINGPORT));
		mBufferCountCurrent--;
		if (mParamPortDefinition.eDir == OMX_DirInput)
		{
			portPrivateInfo = pBufferHdr->pInputPortPrivate;
		}
		else
		{
			portPrivateInfo = pBufferHdr->pOutputPortPrivate;
		}

		error = getProcessingComponent().freeSharedBuffer(mParamPortDefinition.nPortIndex,
			mBufferCountCurrent, OMX_FALSE, pBufferHdr->pPlatformPrivate, portPrivateInfo);
		if(error != OMX_ErrorNone)
			return error;

		error = OMX_FreeBuffer(getTunneledComponent(), mTunneledPort, pBufferHdr);
		if(error != OMX_ErrorNone)
			return error;

		mBufHdrTablePtr[i].mBufHdrPtr    = NULL;
		mBufHdrTablePtr[i].mBufHdrStatus = 0;

	}

	mSharedChunk = 0;
	mParamPortDefinition.bPopulated = OMX_FALSE;

	delete [] mBufHdrTablePtr;
	mBufHdrTablePtr = NULL;

	return OMX_ErrorNone;
}

void ImgEns_Port::bufferSharingEnabled(bool enable)
{
	m_bBufferSharingEnabled = enable;
	getPortFsm()->bufferSharingEnabled(enable);
}

OMX_BOOL ImgEns_Port::mustSendEventHandler(const OMX_PARAM_PORTDEFINITIONTYPE * /*pOld*/)
{
	return OMX_TRUE;
}

OMX_ERRORTYPE ImgEns_Port::Dispatch(const ImgEns_FsmEvent *pEvent)
{
	ImgEns_Port_Fsm *portfsm = getPortFsm();
	if (!portfsm)
	{
		return OMX_ErrorBadPortIndex;
	}
	int status=portfsm->dispatch(pEvent);
	if (status== ImgEns_Fsm::eError_IncorrectStateOperation)
		status= OMX_ErrorIncorrectStateOperation; //translate to right OMX error index
	return((OMX_ERRORTYPE)status);
}

OMX_BOOL ImgEns_Port::isSharingPort() const
{
	if (getPortRole() == ImgEns_SHARING_PORT)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}

OMX_BOOL ImgEns_Port::isBufferSharingPort() const
{
	if (mBufSharingPortList.numPorts)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}

OMX_BOOL ImgEns_Port::isBufferSharingEnabled() const
{
	if (mBufSharingPortList.numPorts == 0)
	{
		return OMX_FALSE;
	} else
		return OMX_FALSE;
}

void ImgEns_Port::setBufferSharingPortList(ImgEns_Port **portList, OMX_U32 numPorts)
{
	mBufSharingPortList.mBufSharingPorts = portList;
	mBufSharingPortList.numPorts         = numPorts;
}

ImgEns_PORT_BUFFER_SUPPLY_ROLE ImgEns_Port::getPortRole() const
{
	if (!isBufferSupplier())
	{
		if (getTunneledComponent())
			return ImgEns_NON_SUPPLIER_PORT;
		else
			return ImgEns_NON_TUNNELED_PORT;
	}
	else
	{
		if (mIsAllocator)
			return ImgEns_ALLOCATOR_PORT;
		else
		{
			IMGENS_ASSERT(getNumberInterConnectedPorts()!=0);
			return ImgEns_SHARING_PORT;
		}
	}
}

void ImgEns_Port::setAllocatorRole(OMX_BOOL value)
{
	mIsAllocator = value;
}

OMX_BOOL ImgEns_Port::isAllocatorPort() const
{
	if (getPortRole() == ImgEns_ALLOCATOR_PORT)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}

void ImgEns_Port::setSharedChunk(MMHwBuffer *sharedChunk)
{
	if (sharedChunk)
		IMGENS_ASSERT(mSharedChunk==0 ? 1 : mSharedChunk==sharedChunk);
	mSharedChunk = sharedChunk;
}



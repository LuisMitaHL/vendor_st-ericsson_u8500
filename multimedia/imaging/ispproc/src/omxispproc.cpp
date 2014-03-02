/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* +CR337836 */
#define DECLARE_AUTOVAR
/* -CR337836 */

#include "omxispproc.h"
#include "ispproc.h"
#include "ImgConfig.h"
#include <string.h>
		
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "ispproc_src_omxispprocTraces.h"
#endif

OMX_ERRORTYPE OMXISPProcFactoryMethod(ENS_Component_p * ppENSComponent)
{
	IN0("\n");
    OstTraceInt0(TRACE_FLOW, "Entry OMXISPProcFactoryMethod");

	COmxIspProc * omxispproc = new COmxIspProc();
	if (omxispproc == 0)
	{
		OUTR(" ",(OMX_ErrorInsufficientResources));
        OstTraceInt0(TRACE_FLOW, "Exit OMXISPProcFactoryMethod OMX_ErrorInsufficientResources");
		return OMX_ErrorInsufficientResources;
	}

	*ppENSComponent = omxispproc;

	IspProc * ispproc = new IspProc(*omxispproc,ePrimaryCamera);
	if (ispproc == 0)
	{
		OUTR(" ",(OMX_ErrorInsufficientResources));
        OstTraceInt0(TRACE_FLOW, "Exit OMXISPProcFactoryMethod OMX_ErrorInsufficientResources");
		return OMX_ErrorInsufficientResources;
	}

	(*ppENSComponent)->setProcessingComponent(ispproc);

	OUTR(" ",(OMX_ErrorNone));
    OstTraceInt0(TRACE_FLOW, "Exit OMXISPProcFactoryMethod OMX_ErrorNone");
	return OMX_ErrorNone;
}



OMX_ERRORTYPE OMXISPProcSecondaryFactoryMethod(ENS_Component_p * ppENSComponent)
{
	IN0("\n");
    OstTraceInt0(TRACE_FLOW, "Entry OMXISPProcSecondaryFactoryMethod");

	COmxIspProc * omxispproc = new COmxIspProc();
	if (omxispproc == 0)
	{
		OUTR(" ",(OMX_ErrorInsufficientResources));
        OstTraceInt0(TRACE_FLOW, "Exit OMXISPProcSecondaryFactoryMethod OMX_ErrorInsufficientResources");
		return OMX_ErrorInsufficientResources;
	}

	*ppENSComponent = omxispproc;

	IspProc * ispproc = new IspProc(*omxispproc,eSecondaryCamera);
	if (ispproc == 0)
	{
		OUTR(" ",(OMX_ErrorInsufficientResources));
        OstTraceInt0(TRACE_FLOW, "Exit OMXISPProcSecondaryFactoryMethod OMX_ErrorInsufficientResources");
		return OMX_ErrorInsufficientResources;
	}

	(*ppENSComponent)->setProcessingComponent(ispproc);

	OUTR(" ",(OMX_ErrorNone));
    OstTraceInt0(TRACE_FLOW, "Exit OMXISPProcSecondaryFactoryMethod OMX_ErrorNone");
	return OMX_ErrorNone;
}


COmxIspProc_RDB::COmxIspProc_RDB(): ENS_ResourcesDB()
{
	IN0("\n");
	setDefaultNMFDomainType(RM_NMFD_PROCSIA);
	OUTR(" ",(0));
}

OMX_ERRORTYPE COmxIspProc::createResourcesDB()
{
	IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry COmxIspProc::createResourcesDB");
	mRMP = new COmxIspProc_RDB();
	if (mRMP == 0)
	{
		OUTR(" ",(OMX_ErrorInsufficientResources));
		OstTraceFiltInst0(TRACE_FLOW, "Exit COmxIspProc::createResourcesDB OMX_ErrorInsufficientResources");
		return OMX_ErrorInsufficientResources;
	}
	else
	{
		OUTR(" ",(OMX_ErrorNone));
		OstTraceFiltInst0(TRACE_FLOW, "Exit COmxIspProc::createResourcesDB OMX_ErrorNone");
		return OMX_ErrorNone;
	}
}

RM_STATUS_E COmxIspProc::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl, OMX_INOUT RM_EMDATA_T* pEstimationData) {
    // resource estimation for Camera. See SAS.

    pEstimationData->sMcpsSva.nMax = 0;
    pEstimationData->sMcpsSva.nAvg = 0;
    pEstimationData->sMcpsSia.nMax = 3;  // 3 MIPS estimated for GRAB/GRAB DD/ISPCTL. You might have more concrete data (I would expect much less, this is assumed conservative)
    pEstimationData->sMcpsSia.nAvg = 3;  // same as max
    pEstimationData->sTcmSva.nSizeX = 0;
    pEstimationData->sTcmSva.nSizeY = 0;
    pEstimationData->sTcmSia.nSizeX = 144+54; /* 144 for ispctl, 54 for memgrabctl, as pulled from CM_GetComponentMemoryInfo() */
    pEstimationData->sTcmSia.nSizeY = 0;
    pEstimationData->sEsramBufs.nSvaHwPipe = RM_ESBUFID_EMPTY;
    pEstimationData->sEsramBufs.nSiaHwXP70 = RM_ESBUFID_DEFAULT;
    pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_BIGGEST;  /* only needed for grab cache on port HR */
    pEstimationData->eUccBitmap = 0; // 0 for HDR. or RM_UCC_VISUAL_FHD;

    return RM_E_NONE;
}

OMX_ERRORTYPE COmxIspProc::setConfig(OMX_INDEXTYPE nIndex,OMX_PTR p)
{
		switch ((t_uint32)nIndex)
		{
			case IFM_IndexConfigCommonReadPe :
				mIfmReadPEType = *(IFM_CONFIG_READPETYPE*)p;
				break;
			case IFM_IndexConfigCommonWritePe :
				mIfmWritePEType = *(IFM_CONFIG_WRITEPETYPE*)p;
				break;
			case OMX_Symbian_IndexConfig_AutoRotationControl :
				{
					mAutoRotateParam = *((OMX_SYMBIAN_CONFIG_BOOLEANTYPE *) p);
					bAutoRotation  = mAutoRotateParam.bEnabled;
				}
				break;
			case IFM_IndexLatency_Ispproc:
				break;

			default :
				return ENS_Component::setConfig(nIndex,p);
		}

		return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxIspProc::getConfig(OMX_INDEXTYPE idx, OMX_PTR p) const
{
	IspProc* Isp = (IspProc*)(&getProcessingComponent());

	switch ((t_uint32)idx)
	{

		case IFM_IndexConfigCommonReadPe :
			memcpy(p, &mIfmReadPEType, sizeof(IFM_CONFIG_READPETYPE));
			break;
		case IFM_IndexConfigCommonWritePe :
			memcpy(p, &mIfmWritePEType, sizeof(IFM_CONFIG_WRITEPETYPE));
			break;
		case IFM_IndexIsISPShared:
			memcpy(p, &mIspShared, sizeof(IFM_CONFIG_ISPSHARED));
			break;
		case OMX_IndexConfigCommonRotate:
			{
				memcpy(p, &mPortRotateParam, sizeof(OMX_CONFIG_ROTATIONTYPE));
			}
			break;
		case OMX_Symbian_IndexConfig_AutoRotationControl :
			{
				memcpy(p, &mAutoRotateParam, sizeof(OMX_SYMBIAN_CONFIG_BOOLEANTYPE));
			}
			break;
		case IFM_IndexLatency_Ispproc:
			{
				memcpy(p, &(Isp->mlatency_Ispproc), sizeof(IFM_LATENCY_ISPPROC));
			}
			break;
		default :
			return ENS_Component::getConfig(idx,p);
	}

	return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxIspProc::setParameter(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentParameterStructure)
{
	IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry COmxIspProc::setParameter");
	OMX_ERRORTYPE err = OMX_ErrorNone;

	if (pComponentParameterStructure == 0)
	{
		OUTR(" ",(OMX_ErrorBadParameter));
		OstTraceFiltInst0(TRACE_FLOW, "Exit COmxIspProc::setParameter OMX_ErrorBadParameter");
		return OMX_ErrorBadParameter;
	}

	err = ENS_Component::setParameter(nParamIndex,pComponentParameterStructure);

	OUTR(" ",(err));
	OstTraceFiltInst1(TRACE_FLOW, "Exit COmxIspProc::setParameter (0x%x)", (err));
	return err;
}


OMX_ERRORTYPE COmxIspProc::getParameter(OMX_INDEXTYPE idx, OMX_PTR p) const {
	IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry COmxIspProc::getParameter");
	OMX_ERRORTYPE err = OMX_ErrorNone;
	if (p == 0)
	{
		OUTR(" ",(OMX_ErrorBadParameter));
		OstTraceFiltInst0(TRACE_FLOW, "Exit COmxIspProc::getParameter OMX_ErrorBadParameter");
		return OMX_ErrorBadParameter;
	}

	switch (idx)
	{
		// Mandatory Parameters (1.1.1 @ 307)
		case OMX_IndexParamVideoInit :
			{
					OMX_PORT_PARAM_TYPE * portTypesParam = (OMX_PORT_PARAM_TYPE *) p;
					portTypesParam->nPorts = ISPPROC_NB_PORTS;
					portTypesParam->nStartPortNumber = 0;
					portTypesParam->nSize = sizeof(OMX_PORT_PARAM_TYPE);
					portTypesParam->nVersion = getVersion();
			}
			break;
		case OMX_IndexParamCommonSensorMode:
			//memcpy(p, &mSensorMode, sizeof(OMX_PARAM_SENSORMODETYPE));
			break;
		default :
			err = ENS_Component::getParameter(idx,p);
			break;
	}

	OUTR(" ",(err));
	OstTraceFiltInst1(TRACE_FLOW, "Exit COmxIspProc::getParameter (0x%x)", (err));
	return err;
}

OMX_ERRORTYPE COmxIspProc::getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const
{
	IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry COmxIspProc::getExtensionIndex");
	OMX_ERRORTYPE err = OMX_ErrorNone;

	if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_AUTOROTATIONCONTROL_NAME, 128))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfig_AutoRotationControl;
	}
	else
	{
		err = ENS_Component::getExtensionIndex(cParameterName, pIndexType);
	}

	OUTR(" ",(err));
	OstTraceFiltInst0(TRACE_FLOW, "Exit COmxIspProc::getExtensionIndex OMX_ErrorBadParameter");
	return err;
}


OMX_ERRORTYPE COmxIspProc::createIspprocPort(OMX_U32 nPortIndex, OMX_DIRTYPE eDir, OMX_BUFFERSUPPLIERTYPE eSupplierPref)
{
	ENS_Port * port;
	EnsCommonPortData commonPortData(nPortIndex,eDir,1,0,OMX_PortDomainVideo,eSupplierPref);
	port = new ispprocport(commonPortData, *this);
	if (port == NULL) return OMX_ErrorInsufficientResources;
	addPort(port);
	return OMX_ErrorNone;
}

COmxIspProc::COmxIspProc(): ENS_Component()
{

/* +CR337836 */
    GET_AND_SET_TRACE_LEVEL(ispproc);
/* -CR337836 */

	ENS_Component::construct(ISPPROC_NB_PORTS);


    OMX_ERRORTYPE error = createIspprocPort(ISPPROC_PORT_IN0, OMX_DirInput, OMX_BufferSupplyUnspecified);
    if(error != OMX_ErrorNone) DBC_ASSERT(0); //TODO: What to do with this error!
    error = createIspprocPort(ISPPROC_PORT_OUT0, OMX_DirOutput, OMX_BufferSupplyOutput);
    if(error != OMX_ErrorNone) DBC_ASSERT(0); //TODO: What to do with this error!
    error = createIspprocPort(ISPPROC_PORT_OUT1, OMX_DirOutput, OMX_BufferSupplyOutput);
    if(error != OMX_ErrorNone) DBC_ASSERT(0); //TODO: What to do with this error!

    mAutoRotateParam.nSize = sizeof(OMX_SYMBIAN_CONFIG_BOOLEANTYPE);
    mAutoRotateParam.nVersion = getVersion();
    mAutoRotateParam.bEnabled = OMX_FALSE;
    mAutoRotateParam.nPortIndex = ISPPROC_PORT_OUT1;

    mPortRotateParam.nSize = sizeof(OMX_CONFIG_ROTATIONTYPE);
    mPortRotateParam.nVersion = getVersion();
    mPortRotateParam.nRotation = OMX_SYMBIAN_OrientationRowTopColumnLeft;
    mPortRotateParam.nPortIndex = ISPPROC_PORT_OUT1;

    bAutoRotation = OMX_FALSE;
}

/*
  This function is called by ENS when an NMF Panic happens in the system (either on SIA/SVA or on ARM-EE)
  In case of panic coming from SIA, our OMX component is supposed to destroy the MPC NMF network in order
   to free SIA and allow NMF to shutdown SIA and restart it again.
*/
void COmxIspProc::NmfPanicCallback(void *contextHandler,t_nmf_service_type serviceType, t_nmf_service_data *serviceData)
{
  switch (serviceType) {
  case NMF_SERVICE_PANIC:
  {
    if (serviceData->panic.panicSource==HOST_EE || serviceData->panic.info.mpc.coreid!=((t_nmf_core_id)SIA_CORE_ID)) {
        return;
    }
    OMX_ERRORTYPE error = OMX_ErrorUndefined;
    error = ((IspProc*)&getProcessingComponent())->errorRecovery();
    if (OMX_ErrorNone != error) {
      DBGT_ERROR("COmxIspProc::NmfPanicCallback : errorRecovery error =0x%x\n",error);
      OstTraceFiltInst1(TRACE_ERROR, "COmxIspProc::NmfPanicCallback : errorRecovery error =0x%x", error);
    }
    /*
     just tell the IL client that an unrecoverable error has happened.
     From this point on, the IL client is supposed to DESTROY our OMX component.
     No state transitions allowed, no assumptions on returned buffers etc.
     At this point we are dead
    */
    eventHandler(OMX_EventError,(t_uint32)OMX_ErrorHardware,0);
    break;
  }
  case NMF_SERVICE_SHUTDOWN:
    if (serviceData->shutdown.coreid != ((t_nmf_core_id)SIA_CORE_ID)) return;
    /*
       implement something here if you want to rebuild the DSP network once the DSP has waken up again
       beware that this event is also generated when the network is detroyed in the nominal case (go to loaded)
    */
    break;
  default:
    break;
  }
}


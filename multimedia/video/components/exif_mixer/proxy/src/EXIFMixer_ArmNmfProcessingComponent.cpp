/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ EXIFMixer_Proxy

#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_exif_mixer_proxy_src_EXIFMixer_ArmNmfProcessingComponentTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE


#ifdef PACKET_VIDEO_SUPPORT
#define LOG_TAG "exif_mixer"
#include<cutils/log.h>
#endif

#include "EXIFMixer_Proxy.h"
#include "EXIFMixer_ArmNmfProcessingComponent.h"


#include "VFM_Port.h"
#include "host/eventhandler.hpp"
#include "SharedBuffer.h"

#define RETURN_OMX_ERROR_IF_ERROR_OST(_x)                { OMX_ERRORTYPE _error; _error = (_x); if (_error!=OMX_ErrorNone) { exif_mixer_process_assert((0==1), (OMX_U32)_error, __LINE__, OMX_FALSE); return _error; } }


void EXIFMixer_ArmNmfProcessingComponent::exif_mixer_process_assert(OMX_U32 condition, OMX_U32 errorType, OMX_U32 line, OMX_BOOL isFatal)
{
    if (!condition)
    {
        OstTraceFiltInst2(TRACE_ERROR, "EXIFMIXER_ARMProcessing: errorType : 0x%x error line %d\n", errorType,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}


EXIFMixer_ArmNmfProcessingComponent::EXIFMixer_ArmNmfProcessingComponent(ENS_Component &enscomp): VFM_NmfHost_ProcessingComponent(enscomp)
{
	OstTraceInt1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : codecCreate DONE <line no %d> ",__LINE__);
 	gpsLocationValues.bAltitudeRefAboveSea = OMX_FALSE;
 	gpsLocationValues.bLatitudeRefNorth = OMX_FALSE;
 	gpsLocationValues.bLocationKnown = OMX_FALSE;
 	gpsLocationValues.bLongitudeRefEast = OMX_FALSE;
 	gpsLocationValues.nAltitudeMeters = 0;
 	gpsLocationValues.nLatitudeDegrees = 0;
 	gpsLocationValues.nLatitudeMinutes = 0;
 	gpsLocationValues.nLatitudeSeconds = 0;
 	gpsLocationValues.nLongitudeDegrees = 0;
 	gpsLocationValues.nLongitudeMinutes = 0;
 	gpsLocationValues.nLongitudeSeconds = 0;
    gpsLocationValues.nHours = 0;
 	gpsLocationValues.nMinutes = 0;
 	gpsLocationValues.nSeconds = 0;
 	/* ER 429860 */
 	gpsLocationValues.nYear = 0;
 	gpsLocationValues.nMonth = 0;
 	gpsLocationValues.nDay = 0;
 	/* ER 429860 */
 	gpsLocationValues.nSize = 0;
 	gpsLocationValues.nPortIndex = 0;
    gpsLocationValues.processingValue = (OMX_KEY_GPS_PROCESSING_METHODTYPE)0;
    for(int i=0;i<32;i++)
        gpsLocationValues.nPrMethodDataValue[i] = 0;
 	isSetConfigDone = OMX_FALSE;
	pProxyComponent = (EXIFMixer_Proxy *)(&mENSComponent);
    //+ER 354962
    OMXHandle = (void *)0;
    mNbPendingCommands = 0;
    omxilosalservices::OmxILOsalMutex::MutexCreate(mMutexPendingCommand);
    //-ER 354962
}


void EXIFMixer_ArmNmfProcessingComponent::doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : doSpecificEmptyBufferDone_cb DONE pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
}

void EXIFMixer_ArmNmfProcessingComponent::doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : doSpecificFillBufferDone_cb pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
}

OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : emptyThisBuffer pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
	if(pBuffer->nFilledLen > 65536) //Differentitating betwen port 0 and port 1
		pProxyComponent->size_jpeg = pBuffer->nFilledLen;
    return NmfHost_ProcessingComponent::emptyThisBuffer(pBuffer);
}

OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : fillThisBuffer pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
	OstTraceFiltInst0(TRACE_FLOW, "In EXIFMixer_ArmNmfProcessingComponent : fillThisBuffer calling notifyportsetting \n");
    pProxyComponent->notifyportsetting(pBuffer->nAllocLen);
    return NmfHost_ProcessingComponent::fillThisBuffer(pBuffer);
}

EXIFMixer_ArmNmfProcessingComponent::~EXIFMixer_ArmNmfProcessingComponent()
{
	OstTraceFiltStatic1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : destructor DONE <line no %d> ",pProxyComponent,__LINE__);
    //+ER 354962
    omxilosalservices::OmxILOsalMutex::MutexFree(mMutexPendingCommand);
    //-ER 354962
}


OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::codecInstantiate()
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : In codecInstantiate  <line no %d> ",__LINE__);
    t_nmf_error error;
    RETURN_OMX_ERROR_IF_ERROR_OST(createPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer[0]", "emptybufferdone[0]"));
    RETURN_OMX_ERROR_IF_ERROR_OST(createPortInterface((VFM_Port *)mENSComponent.getPort(1), "emptythisbuffer[1]", "emptybufferdone[1]"));
    RETURN_OMX_ERROR_IF_ERROR_OST(createPortInterface((VFM_Port *)mENSComponent.getPort(2), "fillthisbuffer", "fillbufferdone"));

	error = mCodec->bindFromUser("getConfig",1,&getconfiguration) ;
	if (error != NMF_OK)
	{
        OstTraceFiltInst0(TRACE_ERROR, "EXIFMIXER_ARMProcessing: error in binding getconfig interface \n");
		exif_mixer_process_assert((0==1), (OMX_U32)OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined ;
	}

	error = mCodec->bindFromUser("setConfig",2,&setconfiguration) ;
	if (error != NMF_OK)
	{
        OstTraceFiltInst0(TRACE_ERROR, "EXIFMIXER_ARMProcessing: error in binding setconfig interface \n");
		exif_mixer_process_assert((0==1), (OMX_U32)OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined ;
	}
    //+ER 354962
    OMXHandle = mENSComponent.getOMXHandle();
    error = EnsWrapper_bindToUser(OMXHandle,mCodec,"iCommandAck",(exif_mixer_arm_nmf_api_cmd_ackDescriptor*)this,4);
    if (error != NMF_OK)
	{
        OstTraceFiltInst0(TRACE_ERROR, "EXIFMIXER_ARMProcessing: error in binding iCommandAck interface \n");
		exif_mixer_process_assert((0==1), (OMX_U32)OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined ;
	}
    //-ER 354962
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : codecInstantiate DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
}


OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler)
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : doSpecificEventHandler_cb DONE <Event Type %d> ",event);
	deferEventHandler = OMX_FALSE;
	return OMX_ErrorNone;
}


OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
	OstTraceFiltInst2(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : doSpecificSendCommand Command : %d Data : %d ",eCmd,nData);
	bDeferredCmd = OMX_FALSE;
	return OMX_ErrorNone;
}


OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::codecStart()
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : codecStart DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
}


OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::codecStop()
{
    //+ER 354962
    mMutexPendingCommand->MutexLock();
    exif_mixer_process_assert((mNbPendingCommands==0),(OMX_U32)OMX_ErrorUndefined, __LINE__, OMX_FALSE);
    mMutexPendingCommand->MutexUnlock();
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : codecStop DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
    //-ER 354962

}


OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::configure()
{   
	if (isSetConfigDone)
	{
        //+ER 354962
        OstTraceFiltInst1(TRACE_FLOW, "In EXIFMixer_ArmNmfProcessingComponent : configure : setConfigure called on mixer_core <line no %d> ",__LINE__);
        addPendingCommand();
        //-ER 354962
        isSetConfigDone = OMX_FALSE; /* + reset the values after setting ER 352049 */
		setconfiguration.setConfigure(OMX_IndexConfigImageGPSLocation,&gpsLocationValues);
	}
    OMX_U32 width = pProxyComponent->getFrameWidth(1);
    OMX_U32 height = pProxyComponent->getFrameHeight(1);
    //+ER 354962
    addPendingCommand();
    //-ER 354962
    setconfiguration.setDimension(width,height);
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : configure DONE <line no %d> ",__LINE__);
	return OMX_ErrorNone;
}


OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::codecConfigure()
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : codecConfigure DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
}



OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::codecDeInstantiate()
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : In codecDeInstantiate <line no %d> ",__LINE__);
	t_nmf_error error;

    RETURN_OMX_ERROR_IF_ERROR_OST(destroyPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer[0]", "emptybufferdone[0]"));
    RETURN_OMX_ERROR_IF_ERROR_OST(destroyPortInterface((VFM_Port *)mENSComponent.getPort(1), "emptythisbuffer[1]", "emptybufferdone[1]"));
    RETURN_OMX_ERROR_IF_ERROR_OST(destroyPortInterface((VFM_Port *)mENSComponent.getPort(2), "fillthisbuffer", "fillbufferdone"));

	error = mCodec->unbindFromUser("getConfig");
	if (error != NMF_OK)
	{
        OstTraceFiltInst0(TRACE_ERROR, "EXIFMIXER_ARMProcessing: error in unbinding getconfig interface \n");
		exif_mixer_process_assert((0==1), (OMX_U32)OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}

	error = mCodec->unbindFromUser("setConfig");
	if (error != NMF_OK)
	{
        OstTraceFiltInst0(TRACE_ERROR, "EXIFMIXER_ARMProcessing: error in unbinding setconfig interface \n");
		exif_mixer_process_assert((0==1), (OMX_U32)OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}
    //+ER 354962
    error = EnsWrapper_unbindToUser(OMXHandle,mCodec, "iCommandAck");
    if (error != NMF_OK)
	{
        OstTraceFiltInst0(TRACE_ERROR, "EXIFMIXER_ARMProcessing: error in unbinding iCommandAck interface \n");
		exif_mixer_process_assert((0==1), (OMX_U32)OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}
    //-ER 354962
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : codecDeInstantiate DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
}


void EXIFMixer_ArmNmfProcessingComponent::registerStubsAndSkels()
{

}

void EXIFMixer_ArmNmfProcessingComponent::unregisterStubsAndSkels()
{
}

OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::codecCreate(OMX_U32 domainId)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
    
	mCodec = exif_mixer_arm_nmf_exifmixerCreate();
	if(!mCodec)
	{
        OstTraceFiltInst0(TRACE_ERROR, "EXIFMIXER_ARMProcessingComponent: in codecCreate \n");
		error = OMX_ErrorUndefined;
		exif_mixer_process_assert((0==1), (OMX_U32)OMX_ErrorUndefined, __LINE__, OMX_TRUE);
	}

	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : codecCreate DONE <line no %d> ",__LINE__);
	return error;
}

void EXIFMixer_ArmNmfProcessingComponent::codecDestroy(void)
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : codecDestroy DONE <line no %d> ",__LINE__);
    exif_mixer_arm_nmf_exifmixerDestroy((exif_mixer_arm_nmf_exifmixer *&)mCodec);
}



OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::retrieveConfig(OMX_INDEXTYPE nConfigIndex,OMX_PTR pComponentConfigStructure)
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : in retrieveConfig <line no %d> ",__LINE__);
    //+ER 354962
	addPendingCommand();
    //-ER 354962
	getconfiguration.getConfigure(nConfigIndex,pComponentConfigStructure);
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : retrieveConfig DONE <line no %d> ",__LINE__);
	return OMX_ErrorNone;
}


OMX_ERRORTYPE EXIFMixer_ArmNmfProcessingComponent::applyConfig(
                    OMX_INDEXTYPE nParamIndex,
                    OMX_PTR pComponentConfigStructure)
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : in applyConfig <line no %d> ",__LINE__);
	configure();
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : applyConfig DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
}

//+ER 354962
void EXIFMixer_ArmNmfProcessingComponent::addPendingCommand()
{
    OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : in addPendingCommand <line no %d> ",__LINE__);
	OstTraceFiltInst1(TRACE_FLOW, "In EXIFMixer_ArmNmfProcessingComponent : in addPendingCommand : mNbPendingCommands : %d ",mNbPendingCommands);
    if (mNbPendingCommands==0) {
        // lock the mutex as we now wait for  feedback
        mMutexPendingCommand->MutexLock();
    }
    mNbPendingCommands++;
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent :  addPendingCommand DONE <line no %d> ",__LINE__);
}

void EXIFMixer_ArmNmfProcessingComponent::pendingCommandAck()
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent : in pendingCommandAck <line no %d> ",__LINE__);
	OstTraceFiltInst1(TRACE_FLOW, "In EXIFMixer_ArmNmfProcessingComponent : in pendingCommandAck : mNbPendingCommands : %d ",mNbPendingCommands);
    exif_mixer_process_assert((mNbPendingCommands>0),(OMX_U32)OMX_ErrorUndefined, __LINE__, OMX_TRUE);
    mNbPendingCommands--;
    if (mNbPendingCommands==0) {
        // unlock the mutex as we do not wait for any other feedback
        mMutexPendingCommand->MutexUnlock();
    }
	OstTraceFiltInst1(TRACE_API, "In EXIFMixer_ArmNmfProcessingComponent :  pendingCommandAck DONE <line no %d> ",__LINE__);
}
//-ER 354962

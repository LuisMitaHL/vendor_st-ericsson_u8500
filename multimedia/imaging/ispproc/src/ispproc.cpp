/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ispproc.h"
#include "osi_trace.h"
#include "SharedBuffer.h"
#include "omxispproc.h"
#include "extradata.h"
#include "IFM_Trace.h"

#include "host/grabctl/api/error.hpp"
#include "host/eventhandler.hpp"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "ispproc_src_ispprocTraces.h"
#endif

#include "ExifReader.h"

#define OMX_VERSION_1_1_2 0x00020101

#define ISPPROC_TUNING_DATABASE_CLIENT_NAME "ISPPROC"
#define ENDIANCONVERT16(a) ((a >> 8) | ((a & 0xff) << 8))
#define ENDIANCONVERT32(a) ((a >> 24) | ((a & 0x00FF0000) >> 8) | ((a & 0xFF00) << 8) | ((a & 0xff) << 24))

#define EXIF_SIZE_HEADER_SIZE 8
#define EXIF_APP_HEADER_SIZE 10
#define EXIF_TIFF_HEADER_SIZE 8     
void IspProc::doSpecificFillBufferDone_cb( OMX_BUFFERHEADERTYPE* pBuffer)
{

	switch (pBuffer->nOutputPortIndex) {
			case (ISPPROC_PORT_OUT0) :
			{
				fillBufferDoneVPB1(pBuffer);
				break;
			}
			case (ISPPROC_PORT_OUT1) :
			{
				fillBufferDoneVPB2(pBuffer);
				break;
			}
			default :
				DBC_ASSERT(0);

	}

}

void IspProc::doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{

	switch (pBuffer->nInputPortIndex) {
			case (ISPPROC_PORT_IN0) :
			{
				emptyBufferDoneVPB0(pBuffer);
				break;
			}

			default :
				DBC_ASSERT(0);

	}

}

OMX_ERRORTYPE IspProc::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_ERRORTYPE err = OMX_ErrorUndefined;

	switch (pBuffer->nInputPortIndex) {
			case (ISPPROC_PORT_IN0) :
			{
				err = emptyThisBufferVPB0(pBuffer);
				return(err);
			}

			default :
				DBC_ASSERT(0);
	}
	return(err);
}



void IspProc::fillBufferDoneVPB2(OMX_BUFFERHEADERTYPE* pOmxBufHdr) {

	ASYNC_IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::fillBufferDoneVPB2");

    //Changes for stripes
    if((stripeCount != (stripeNumber + 1)) && (stripeCount > 1))
        {
       	ExtradataLoadCounter = MAX_ALLOCATED_BUFFER_NB - 1;
        
        stripeNumber = stripeNumber + 1;

        s_scf_event event;
		event.type.other=0;
        event.sig = ISPPROC_STRIPE_SIG;
		iDeferredEventMgr.queuePriorEvent(&event);
        
		s_scf_event devent;
		devent.sig = Q_DEFERRED_EVENT_SIG;
	    p_isp_sm->ProcessEvent(&devent);

        return;
        }
    stripeNumber = 0;

	/* copy extradata received from camera in buffer */
	OMX_ERRORTYPE error = copyExtradata_Mem2Buffer(pOmxBufHdr);
	if (OMX_ErrorNone != error)
	{
		DBC_ASSERT(0);
	}
	
	MSG1("pOmxBufHdr->nFlags=0x%lx\n",pOmxBufHdr->nFlags);
	OstTraceFiltInst1 (TRACE_DEBUG, "pOmxBufHdr->nFlags=0x%lx",pOmxBufHdr->nFlags);

	if(p_isp_sm->captureRequest[ISPPROC_PORT_OUT1]!=0)
	{
		if(OMX_FALSE == bPortSettingChanged)
		{
			/*Since buffer received is not the valid capture request fulfilled */
			p_isp_sm->captureRequest[ISPPROC_PORT_OUT1]--;
		}
		MSG0("OMX_BUFFERFLAG_EOS/EOF + OMX_EventBufferFlag\n");
		OstTraceFiltInst0 (TRACE_DEBUG, "OMX_BUFFERFLAG_EOS/EOF + OMX_EventBufferFlag");
		
		if(pOmxBufHdr->nFlags & OMX_BUFFERFLAG_EOS ) {
			MSG0("OMX_EventBufferFlag OMX_BUFFERFLAG_EOS\n");
			OstTraceFiltInst0 (TRACE_DEBUG, "OMX_EventBufferFlag OMX_BUFFERFLAG_EOS");
			mENSComponent.eventHandlerCB(OMX_EventBufferFlag, ISPPROC_PORT_OUT1, OMX_BUFFERFLAG_EOS, 0);
			IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
			OMX_SYMBIAN_CAPTUREPARAMETERSTYPE CaptureParameters;
            OMX_ERRORTYPE error = OMX_ErrorNone;
            memset((void*)&CaptureParameters,0,sizeof(CaptureParameters));
			error = pIspProc->getCaptureParameters(&CaptureParameters);
			if(OMX_ErrorNone != error) {
				DBGT_ERROR("Failed to get dampers extradata\n");
				OstTraceFiltStatic0 (TRACE_ERROR, "Failed to get dampers extradata", (&mENSComponent));
				DBC_ASSERT(0);
			}
			
			//for updating orientation in exif tag
			if ((0 == pOmxBufHdr) || (0 == pOmxBufHdr->pBuffer) || (0 == pOmxBufHdr->nAllocLen)) {
			 DBC_ASSERT(0);
			}
			else if (0 == pOmxBufHdr->nFilledLen) {
			 DBC_ASSERT(0);
			}

			/* get extradata start pointer */
			OMX_U8* pCur = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;


		    /*finding the memory occupied by structures before exif extradata*/
		    OMX_U32 size_extradata_before_exif = ((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(IFM_BMS_CAPTURE_CONTEXT) + 3) & ~0x3) +
				   ((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(IFM_DAMPERS_DATA) + 3 ) & ~0x3) +
				   ((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE) + 3) & ~0x3) +
				   ((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE) + 3) & ~0x3) +
				  ((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE) + 3) & ~0x3); 


			 /*adding the size occupied by structures before exif extradata to reach exif extradata*/
 		       pCur +=  size_extradata_before_exif;
		   	OMX_OTHER_EXTRADATATYPE *ext_data = (OMX_OTHER_EXTRADATATYPE*)pCur;
			if(ext_data->eType == (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataEXIFAppMarker)
				{
				pCur = (OMX_U8*)(&(ext_data->data) + 8);
				ExifReader Reader;
				uint32_t maxsize=0;
				ExifPtrDataValue Ptr;
				int result= Reader.AttachBuffer((char*)pCur, pOmxBufHdr->nAllocLen-(pOmxBufHdr->nFilledLen+size_extradata_before_exif));
				if (result==OMX_ErrorNone)
					{
					ExifTag *pTag;
					pTag=Reader.FindTag(eExifTag_Orientation);
					if (pTag!= NULL)
						{ 
						pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
						if (bAutoRotation==OMX_TRUE)
							{
							*Ptr.usValue = OMX_SYMBIAN_OrientationRowTopColumnLeft;
							}
						else
							{
							*Ptr.usValue= CaptureParameters.eSceneOrientation;
							}
						}

					pTag = Reader.FindTag(eExifTag_PixelXDimension);
					if (pTag != NULL)
					{
						ispprocport* port = (ispprocport*)mENSComponent.getPort(ISPPROC_PORT_OUT1);
						pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);

						*Ptr.uiValue = (OMX_U32) (port->getParamPortDefinition().format.video.nFrameWidth);
					}

					pTag = Reader.FindTag(eExifTag_PixelYDimension);
					if (pTag != NULL)
					{
						ispprocport* port = (ispprocport*)mENSComponent.getPort(ISPPROC_PORT_OUT1);
						pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
						*Ptr.uiValue = port->getParamPortDefinition().format.video.nSliceHeight;
					}
					}
				}

			
		   //for updating exif tag end
			if ( ((CaptureParameters.eSceneOrientation == OMX_SYMBIAN_OrientationRowRightColumnTop) ||
				(CaptureParameters.eSceneOrientation == OMX_SYMBIAN_OrientationRowLeftColumnBottom)) && (OMX_TRUE == bAutoRotation) )
			{ //setting rotation information to default
				CaptureParameters.eSceneOrientation=OMX_SYMBIAN_OrientationRowTopColumnLeft;
			}
			OMX_VERSIONTYPE version = CaptureParameters.nExtraDataVersion;
			pIspProc->setCaptureParameters(pOmxBufHdr,&CaptureParameters,version);

			//performance traces start
			p_isp_sm->latencyMeasure(&p_isp_sm->mTime);
			mlatency_Ispproc.e_OMXISPSTILL_Process.t0 = p_isp_sm->mTime;
			//performance traces end

		}
		else if (pOmxBufHdr->nFlags & OMX_BUFFERFLAG_ENDOFFRAME) 	{
			MSG0("OMX_EventBufferFlag OMX_BUFFERFLAG_ENDOFFRAME\n");
			OstTraceFiltInst0 (TRACE_DEBUG, "OMX_EventBufferFlag OMX_BUFFERFLAG_ENDOFFRAME");
			mENSComponent.eventHandlerCB(OMX_EventBufferFlag, ISPPROC_PORT_OUT1, OMX_BUFFERFLAG_ENDOFFRAME, 0);
		}
    
		MSG2("\np_isp_sm->captureRequest[ISPPROC_PORT_OUT0]=%d,p_isp_sm->captureRequest[ISPPROC_PORT_IN0]=%d\n",p_isp_sm->captureRequest[ISPPROC_PORT_OUT0],p_isp_sm->captureRequest[ISPPROC_PORT_IN0]);
		OstTraceFiltInst2 (TRACE_DEBUG, "\np_isp_sm->captureRequest[ISPPROC_PORT_OUT0]=%d,p_isp_sm->captureRequest[ISPPROC_PORT_IN0]=%d",p_isp_sm->captureRequest[ISPPROC_PORT_OUT0],p_isp_sm->captureRequest[ISPPROC_PORT_IN0]);

        if ((p_isp_sm->captureRequest[ISPPROC_PORT_OUT0]==0) && (p_isp_sm->captureRequest[ISPPROC_PORT_IN0]==0)) {
			OstTraceInt1 (TRACE_LATENCY, "e_OMXISPPROC_Process %d", 0);
        	s_scf_event event;
            event.type.other=0;
			event.sig = ISPPROC_EXIT_CAPTURE_SIG;
			iDeferredEventMgr.queuePriorEvent(&event);

			s_scf_event devent;
			devent.sig = Q_DEFERRED_EVENT_SIG;
			p_isp_sm->ProcessEvent(&devent);
        }
    }

    ASYNC_OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::fillBufferDoneVPB2");
}


void IspProc::fillBufferDoneVPB1(OMX_BUFFERHEADERTYPE* pOmxBufHdr) {

	ASYNC_IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::fillBufferDoneVPB1");
	
		/* we had the buffer, go back to VF through Camera */		/* copy extradata received from camera in buffer */
		OMX_ERRORTYPE error = copyExtradata_Mem2Buffer(pOmxBufHdr);
		if (OMX_ErrorNone != error)
		{
			DBC_ASSERT(0);
		}
	
		MSG1("pOmxBufHdr->nFlags=0x%lx\n",pOmxBufHdr->nFlags);
		OstTraceFiltInst1 (TRACE_DEBUG, "pOmxBufHdr->nFlags=0x%lx",pOmxBufHdr->nFlags);

		if(p_isp_sm->captureRequest[ISPPROC_PORT_OUT0]!=0)
		{
			p_isp_sm->captureRequest[ISPPROC_PORT_OUT0]--;

			MSG0("OMX_BUFFERFLAG_EOS/EOF + OMX_EventBufferFlag\n");
			OstTraceFiltInst0 (TRACE_DEBUG, "OMX_BUFFERFLAG_EOS/EOF + OMX_EventBufferFlag");
			
			if(pOmxBufHdr->nFlags & OMX_BUFFERFLAG_EOS) {
				MSG0("OMX_EventBufferFlag OMX_BUFFERFLAG_EOS\n");
				OstTraceFiltInst0 (TRACE_DEBUG, "OMX_EventBufferFlag OMX_BUFFERFLAG_EOS");
				mENSComponent.eventHandlerCB(OMX_EventBufferFlag, ISPPROC_PORT_OUT0, OMX_BUFFERFLAG_EOS, 0);
				IspProc* pIspProc = (IspProc*)&mENSComponent.getProcessingComponent();
				OMX_SYMBIAN_CAPTUREPARAMETERSTYPE CaptureParameters;
                OMX_ERRORTYPE error = OMX_ErrorUndefined;
                error = pIspProc->getCaptureParameters(&CaptureParameters);
                if(OMX_ErrorNone != error) {
                    DBGT_ERROR("Failed to get capture parameters extradata\n");
                    OstTraceFiltStatic0 (TRACE_ERROR, "Failed to get capture parameters extradata", (&mENSComponent));
                    DBC_ASSERT(0);
                }

			//for updating orientation in exif tag
			if ((0 == pOmxBufHdr) || (0 == pOmxBufHdr->pBuffer) || (0 == pOmxBufHdr->nAllocLen)) {
			 DBC_ASSERT(0);
			}
			else if (0 == pOmxBufHdr->nFilledLen) {
			 DBC_ASSERT(0);
			}

			/* get extradata start pointer */
			OMX_U8* pCur = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;

		    	/*finding the memory occupied by structures before exif extradata*/
		    	OMX_U32 size_extradata_before_exif = ((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(IFM_BMS_CAPTURE_CONTEXT) + 3) & ~0x3) +
				((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(IFM_DAMPERS_DATA) + 3 ) & ~0x3) +
				((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE) + 3) & ~0x3) +
				((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE) + 3) & ~0x3) +
				((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE) + 3) & ~0x3); 


			 /*adding the size occupied by structures before exif extradata to reach exif extradata*/
 		       pCur +=  size_extradata_before_exif;
		   	OMX_OTHER_EXTRADATATYPE *ext_data = (OMX_OTHER_EXTRADATATYPE*)pCur;
			if(ext_data->eType == (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataEXIFAppMarker)
			{
				pCur = (OMX_U8*)(&(ext_data->data) + 8);
				ExifReader Reader;
				uint32_t maxsize=0;
				ExifPtrDataValue Ptr;
				int result= Reader.AttachBuffer((char*)pCur, pOmxBufHdr->nAllocLen-(pOmxBufHdr->nFilledLen+size_extradata_before_exif));
				if (result==OMX_ErrorNone)
				{
					ExifTag *pTag;
					pTag = Reader.FindTag(eExifTag_PixelXDimension);
					if (pTag != NULL)
					{
						ispprocport* port = (ispprocport*)mENSComponent.getPort(ISPPROC_PORT_OUT0);
						pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
						*Ptr.uiValue = (OMX_U32) (port->getParamPortDefinition().format.video.nFrameWidth);
					}

					pTag = Reader.FindTag(eExifTag_PixelYDimension);
					if (pTag != NULL)
					{
						ispprocport* port = (ispprocport*)mENSComponent.getPort(ISPPROC_PORT_OUT0);
						pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
						*Ptr.uiValue = port->getParamPortDefinition().format.video.nSliceHeight;
					}
				}
			}
			OMX_VERSIONTYPE version = CaptureParameters.nExtraDataVersion;
			pIspProc->setCaptureParameters(pOmxBufHdr,&CaptureParameters,version);

				//performance traces start
				p_isp_sm->latencyMeasure(&p_isp_sm->mTime);
				mlatency_Ispproc.e_OMXISPSNAP_Process.t0 = p_isp_sm->mTime;
				//performance traces end
			}
			else if (pOmxBufHdr->nFlags & OMX_BUFFERFLAG_ENDOFFRAME) 	{
				MSG0("OMX_EventBufferFlag OMX_BUFFERFLAG_ENDOFFRAME\n");
				OstTraceFiltInst0 (TRACE_DEBUG, "OMX_EventBufferFlag OMX_BUFFERFLAG_ENDOFFRAME");
				mENSComponent.eventHandlerCB(OMX_EventBufferFlag, ISPPROC_PORT_OUT0, OMX_BUFFERFLAG_ENDOFFRAME, 0);
			}
            		
            if ((p_isp_sm->captureRequest[ISPPROC_PORT_OUT1]==0) && (p_isp_sm->captureRequest[ISPPROC_PORT_IN0]==0))
            {
            	OstTraceInt1 (TRACE_LATENCY, "e_OMXISPPROC_Process %d", 0);
                s_scf_event event;
                event.type.other=0;
				event.sig = ISPPROC_EXIT_CAPTURE_SIG;
				iDeferredEventMgr.queuePriorEvent(&event);

				s_scf_event devent;
				devent.sig = Q_DEFERRED_EVENT_SIG;
				p_isp_sm->ProcessEvent(&devent);
            }
        }

    ASYNC_OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::fillBufferDoneVPB1");
}


void IspProc::emptyBufferDoneVPB0(OMX_BUFFERHEADERTYPE* pOmxBufHdr) {

	ASYNC_IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::emptyBufferDoneVPB0");
    
    p_isp_sm->captureRequest[ISPPROC_PORT_IN0]--;
	
    // we need to wait that HR, LR and BML DMA tasks end before stopping pipe.
    // exemple : if we have pipe aspect ratios different from sensor aspect ratio, LR & HR tasks can end before BML one
    if ((p_isp_sm->captureRequest[ISPPROC_PORT_OUT1]==0) && (p_isp_sm->captureRequest[ISPPROC_PORT_OUT0]==0))
    {
		OstTraceInt1 (TRACE_LATENCY, "e_OMXISPPROC_Process %d", 0);
        s_scf_event event;
        event.type.other=0;
        event.sig = ISPPROC_EXIT_CAPTURE_SIG;
        iDeferredEventMgr.queuePriorEvent(&event);

        s_scf_event devent;
        devent.sig = Q_DEFERRED_EVENT_SIG;
        p_isp_sm->ProcessEvent(&devent);
    }
    
    ASYNC_OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::emptyBufferDoneVPB0");
}


OMX_ERRORTYPE IspProc::emptyThisBufferVPB0(OMX_BUFFERHEADERTYPE* pOmxBufHdr)
{
	ASYNC_IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::emptyThisBufferVPB0");


    SharedBuffer *sharedbuffer = static_cast<SharedBuffer *>(pOmxBufHdr->pInputPortPrivate);

    DBC_ASSERT(pOmxBufHdr->nFilledLen <= pOmxBufHdr->nAllocLen);

    sharedbuffer->updateMPCHeader();

    mIemptyThisBuffer[pOmxBufHdr->nInputPortIndex].emptyThisBuffer(sharedbuffer->getMPCHeader());

	//RSRSRS
	/* TBD : if the ispproc is for still then lock(2) */
	/* TODO :   if ended  do not dot*/
	iResourceSharerManager.mRSLock.Lock(ISPPROC_SINGLE_STILL); //all havs same prio.

	/* save BMS context from extradata */
	/* save extradata */
	OMX_ERRORTYPE error = copyExtradata_Buffer2Mem(pOmxBufHdr);
	if (OMX_ErrorNone != error)
	{
		DBC_ASSERT(0);
	}
	

	ASYNC_OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::emptyThisBufferVPB0");
	return OMX_ErrorNone;
}


void ispprocErrorCbk::error(enum e_grabError error_id, t_uint16 data, enum e_grabPipeID pipe_id) {

	ASYNC_IN0("\n");
	OstTraceInt0(TRACE_FLOW, "Entry ispprocErrorCbk::error");

	DBGT_ERROR("grabctl error: pipe_id=%d error_id=0x%X data=0x%X\n", pipe_id, error_id, data);
	OstTraceInt3 (TRACE_ERROR, "grabctl error: pipe_id=%d error_id=0x%X data=0x%X", pipe_id, error_id, data);
	/* TODO: handle errors :p */

	ASYNC_OUT0("\n");
	OstTraceInt0(TRACE_FLOW, "Exit ispprocErrorCbk::error");
}





OMX_ERRORTYPE IspProc::doSpecificEventHandler_cb(OMX_EVENTTYPE ev, OMX_U32 data1, OMX_U32 data2,OMX_BOOL & bDeferredEventHandler) {

    IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::doSpecificEventHandler_cb");
    OMX_STATETYPE exitedState = OMX_StateInvalid;
	mENSComponent.GetState(&mENSComponent, &exitedState);

    bDeferredEventHandler = OMX_FALSE;

    switch(ev){
		case OMX_EventCmdComplete:
			{
				if(data1 == OMX_CommandStateSet)
				{
					if (data2 == OMX_StateIdle && exitedState == OMX_StateExecuting)
					{
					}
					else if (data2 == OMX_StateIdle && exitedState == OMX_StateLoaded)
					{
						constructSM(mENSComponent);
#ifdef MS_IMP
						p_isp_sm->init(this); // the CB will be sent after the init sequence.
#elif defined(EMPTY_IMP) || defined(PR_IMP)
						// JMV: We initialize the isp state machine.
						SCF_START_SM(p_isp_sm);
#else
	#error "implementation missing"
#endif
			            bDeferredEventHandler = OMX_FALSE;
					}
					else if (data2 == OMX_StateExecuting && exitedState == OMX_StateIdle)
					{
						///p_isp_sm->init(this);
						//bDeferredEventHandler = OMX_TRUE;
					}

				}
				else if (data1 == OMX_CommandPortEnable)
				{
					if ((exitedState == OMX_StateExecuting) && (bPortSettingChanged == OMX_TRUE))
					{
						bPortSettingChanged = OMX_FALSE;
						s_scf_event event;
						event.type.other=0;
						event.sig = UPDATE_PORT_FOR_ROTATION_SIG;
						p_isp_sm->pDeferredEventMgr->queuePriorEvent(&event);
						s_scf_event devent;
						devent.sig = Q_DEFERRED_EVENT_SIG;
					    p_isp_sm->ProcessEvent(&devent);
					}

					if((exitedState == OMX_StateIdle)||(exitedState == OMX_StateExecuting))
					{

                        bDeferredEventHandler = OMX_FALSE;

					}
				}
				else  if (data1 == OMX_CommandPortDisable){
						if(data2==OMX_ALL) {
						unbindSharedBuf(ISPPROC_PORT_OUT0,OMX_TRUE);
						unbindSharedBuf(ISPPROC_PORT_OUT1,OMX_TRUE);
						unbindSharedBuf(ISPPROC_PORT_IN0,OMX_TRUE); 
						} else
						unbindSharedBuf(data2,OMX_TRUE);
				}
			}
			break;
		default:
			break;
	}
    OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::doSpecificEventHandler_cb");
    return OMX_ErrorNone;
}


/*
 * Construct ISPPROC_SM
 */
void IspProc::constructSM(ENS_Component &enscomp){


	p_isp_sm = new ISPPROC_SM(enscomp);
	p_isp_sm->pIspctlCom = &iIspctlCom;
	p_isp_sm->pDeferredEventMgr =&iDeferredEventMgr ;
	p_isp_sm->pTrace = &iTrace;
	p_isp_sm->pMemGrabControl = &iMemGrabControl;
	p_isp_sm->pResourceSharerManager = &iResourceSharerManager;
	p_isp_sm->pOmxStateMgr =&iOmxStateMgr ;
	p_isp_sm->pTuningDataBase = pTuningDataBase;
	p_isp_sm->pIspDampers = &iIspDampers;
	p_isp_sm->pRotation = &rotation;
	p_isp_sm->pAutoRotation =  &(((COmxIspProc*)&mENSComponent)->bAutoRotation);
	bAutoRotation = *(&(((COmxIspProc*)&mENSComponent)->bAutoRotation));
	p_isp_sm->pPortSettingChanged = &bPortSettingChanged;

	p_isp_sm->pPictureSettings = &iPictureSettings;
	iIspctlComponentManager.setSM((CScfStateMachine*)p_isp_sm);
	iResourceSharerManager.setSM((CScfStateMachine*)p_isp_sm);
	//performance traces start
	p_isp_sm->bTrace_latency = OMX_FALSE;
	//performance traces end
}

/*
 * Destroy ISPPROC_SM
 */



void IspProc::destroySM(void){
	delete p_isp_sm;
}

t_uint16 IspProc::ispprocInstanceCounter = 0;


IspProc::IspProc(ENS_Component &enscomp, enumCameraSlot cam) :
	IFM_NmfProcessingComp(enscomp),
	iIspctlComponentManager(SIA_CLIENT_ISPPROC,enscomp),
	iIspctlCom(SIA_CLIENT_ISPPROC, &enscomp),
	iMemGrabControl(SIA_CLIENT_ISPPROC,enscomp),
	iOmxStateMgr(),
	iDeferredEventMgr(),
	iResourceSharerManager(SIA_CLIENT_ISPPROC, &enscomp),
	AutoPauseAfterCapture(OMX_FALSE),
	iIspDampers(&enscomp),
	iPictureSettings(&enscomp)
{

	IN0("\n");
	OstTraceInt0(TRACE_FLOW, "Entry IspProc::IspProc");
	bCaptureRequestServed = 0;
	pTuningDataBase = NULL;

	iIspctlComponentManager.setClientId((t_sia_client_id)(iIspctlComponentManager.getClientId() + getInstance()));
	iIspctlCom.setClientId((t_sia_client_id)(iIspctlCom.getClientId() + getInstance()));
	iMemGrabControl.setClientId((t_sia_client_id)(iMemGrabControl.getClientId() + getInstance()));
	iResourceSharerManager.setClientId((t_sia_client_id)(iResourceSharerManager.getClientId() + getInstance()));

	incrInstance();

	MSG0("Default configuration on VPB0\n");
	OstTraceInt0 (TRACE_DEBUG, "Default configuration on VPB0");
    OMX_PARAM_PORTDEFINITIONTYPE defaultParam;
    defaultParam.nPortIndex = ISPPROC_PORT_OUT0;
    defaultParam.format.video.nFrameWidth = ispprocport::DEFAULT_WIDTH;
    defaultParam.format.video.nFrameHeight =ispprocport::DEFAULT_HEIGHT;
    defaultParam.format.video.nSliceHeight =ispprocport::DEFAULT_HEIGHT;
    defaultParam.format.video.eColorFormat=ispprocport::DEFAULT_COLOR_FORMAT;
    defaultParam.format.video.xFramerate=ispprocport::DEFAULT_FRAMERATE;
    ispprocport* port = (ispprocport*)mENSComponent.getPort(ISPPROC_PORT_OUT0);
    port->setDefaultFormatInPortDefinition(&defaultParam);

    MSG0("Default configuration on VPB1\n");
    OstTraceInt0 (TRACE_DEBUG, "Default configuration on VPB1");
    defaultParam.nPortIndex = ISPPROC_PORT_OUT1;
    port = (ispprocport*)mENSComponent.getPort(ISPPROC_PORT_OUT1);
    port->setDefaultFormatInPortDefinition(&defaultParam);

    MSG0("Default configuration on IN0\n");
    OstTraceInt0 (TRACE_DEBUG, "Default configuration on IN0");
    defaultParam.nPortIndex = ISPPROC_PORT_IN0;
    defaultParam.format.video.eColorFormat=ispprocport::DEFAULT_BAYER_COLOR_FORMAT;
    port = (ispprocport*)mENSComponent.getPort(ISPPROC_PORT_IN0);
    port->setDefaultFormatInPortDefinition(&defaultParam);

	/* init extradata array indexes */
	ExtradataStoreCounter = 0;
	ExtradataLoadCounter = MAX_ALLOCATED_BUFFER_NB - 1;

    //Changes for stripes
    stripeCount = 1;//1 -> For no stripes, 2 -> For two stripes ;
    stripeNumber = 0;
	stripeSyncCount = 0;
	bPortSettingChanged = OMX_FALSE;
	//performance traces start
	memset(&(mlatency_Ispproc),0,sizeof(mlatency_Ispproc));
	mlatency_Ispproc.nSize = sizeof(IFM_LATENCY_ISPPROC);
	mlatency_Ispproc.nVersion.nVersion = OMX_VERSION_1_1_2;
	//performance traces end
    rotation = (int)OMX_FALSE;
    bAutoRotation = (int)OMX_FALSE;
    memset((void*)&BMS_capture_context,0,sizeof(BMS_capture_context));
	OUTR(" ",(0));
	OstTraceInt0(TRACE_FLOW, "Exit IspProc::IspProc");
}

IspProc::~IspProc() {
	IN0("\n");
	OstTraceInt0(TRACE_FLOW, "Entry IspProc::~IspProc");

	MSG0("ISPProc deleted\n");
	OstTraceInt0 (TRACE_DEBUG, "ISPProc deleted");
	decrInstance();

	OUTR(" ",(0));
	OstTraceInt0(TRACE_FLOW, "Exit IspProc::~IspProc");
}



OMX_ERRORTYPE IspProc::retrieveConfig(OMX_INDEXTYPE aIdx, OMX_PTR p)  {
	IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::retrieveConfig");
	OMX_ERRORTYPE err=OMX_ErrorNone;

	MSG1("getConfig with parameter 0x%x\n",aIdx);
	OstTraceFiltInst1 (TRACE_DEBUG, "getConfig with parameter 0x%x",aIdx);


	switch ((t_uint32)aIdx) {


		case IFM_IndexIsISPShared:
			{		// '{...}' required for linux build !
				IFM_CONFIG_ISPSHARED * ispShared = (IFM_CONFIG_ISPSHARED *)p;
				ispShared->bEnabled=OMX_TRUE;
				break;
			}
		case IFM_IndexConfigCommonWritePe :
		{
			IFM_CONFIG_WRITEPETYPE * pPE = (IFM_CONFIG_WRITEPETYPE *)p;
			*pPE = iTrace.iListOfPeToWrite;
			break;
		}
		
		case IFM_IndexConfigCommonReadPe :
		{
			IFM_CONFIG_READPETYPE * pPE = (IFM_CONFIG_READPETYPE *)p;
			*pPE = iTrace.iListOfPeToRead;
			break;
		}

		default:
			err = mENSComponent.getConfig(aIdx,p);
	}


	OUTR(" ",(err));
	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::retrieveConfig (0x%x)", err);
	return err;

}


OMX_ERRORTYPE IspProc::applyConfig(
		OMX_INDEXTYPE nIndex,
		OMX_PTR pStructure) {

            s_scf_event event;
            event.type.other=0;
	IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::applyConfig");
	if (pStructure == 0) {
		OUTR(" ",OMX_ErrorBadParameter);
		OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::applyConfig OMX_ErrorBadParameter");
		return OMX_ErrorBadParameter;
	}

	COmxIspProc * OMXIspProc = (COmxIspProc*)&mENSComponent;
	switch ((t_uint32)nIndex) {

		case IFM_IndexConfigCommonReadPe :
		{
            event.type.other=0;
			event.type.readStruct = OMXIspProc->mIfmReadPEType;
			event.sig = SHARED_READLISTPE_DEBUG_SIG;
			iDeferredEventMgr.queueNewEvent(&event);

			s_scf_event devent;
			devent.sig = Q_DEFERRED_EVENT_SIG;
			p_isp_sm->ProcessEvent(&devent);

			break;
		}

		case IFM_IndexConfigCommonWritePe :
		{
            event.type.other=0;
			event.type.writeStruct = OMXIspProc->mIfmWritePEType;
			event.sig = SHARED_WRITELISTPE_SIG;
			iDeferredEventMgr.queueNewEvent(&event);

			s_scf_event devent;
			devent.sig = Q_DEFERRED_EVENT_SIG;
			p_isp_sm->ProcessEvent(&devent);


			break;

		}

		case OMX_Symbian_IndexConfig_AutoRotationControl :
		break;

		case IFM_IndexLatency_Ispproc:
		break;
		default :
			OUTR(" ",OMX_ErrorNone);
			OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::default");
			return OMX_ErrorNone;
	}

	OUTR(" ",OMX_ErrorNone);
	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::applyConfig (0x%x)", OMX_ErrorNone);
	return OMX_ErrorNone;



}


OMX_ERRORTYPE IspProc::start() {
	IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::start");
	OMX_ERRORTYPE omxerr = OMX_ErrorNone;



	omxerr = iIspctlComponentManager.startNMF();
	if (omxerr != OMX_ErrorNone){
         OUTR(" ",(omxerr));
     	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::start (0x%x)", omxerr);
         return omxerr;
    }


	iIspctlCom.setIspctlCmd(iIspctlComponentManager.getIspctlCmd());

	iResourceSharerManager.startNMF(); // no error returned

	iMemGrabControl.start();

	OUTR(" ",omxerr);
 	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::start (0x%x)", omxerr);
	return omxerr;

}


OMX_ERRORTYPE IspProc::instantiate() {
	IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::instantiate");



	OMX_ERRORTYPE error = OMX_ErrorNone;

	iIspctlComponentManager.setOMXHandle(mENSComponent.getOMXHandle());
	//iIspctlComponentManager.setHSM((CScfStateMachine*)&hsm);

	iMemGrabControl.setOMXHandle(mENSComponent.getOMXHandle());


	error = iIspctlComponentManager.instantiateNMF();
	if (error != OMX_ErrorNone){
         OUTR(" ",(error));
      	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::instantiate (0x%x)", error);
         return error;
    }

	error = iMemGrabControl.instantiate();
	if (error != OMX_ErrorNone){
         OUTR(" ",(error));
       	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::instantiate (0x%x)", error);
         return error;
    }


	error = bindSharedBuf(ISPPROC_PORT_OUT0);
	if (error != OMX_ErrorNone) return error;
	error = bindSharedBuf(ISPPROC_PORT_OUT1);
	if (error != OMX_ErrorNone) return error;
	error = bindSharedBuf(ISPPROC_PORT_IN0);
	if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponentFromHost(
			iMemGrabControl.getMemControllerHandle(),
			"sendcommand",
			(NMF::InterfaceReference*) getNmfSendCommandItf(),
			1);
	if (error != OMX_ErrorNone){
		OUTR(" ",(error));
      	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::instantiate (0x%x)", error);
		return error;
	}


	error = ENS::bindComponentFromHost(
			iMemGrabControl.getMemControllerHandle(),
			"fsminit",
			(NMF::InterfaceReference*) getNmfFsmInitItf(),
			1);
	if (error != OMX_ErrorNone){
		OUTR(" ",(error));
      	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::instantiate (0x%x)", error);
		return error;
	}

	//CBgrabctl_api_error cbGrabctlError = {this, grabctlError_cb};
	error = ENS::bindComponentToHost(
			mENSComponent.getOMXHandle(),
			iMemGrabControl.getMemControllerHandle(),
			"alert_error",
			(NMF::InterfaceDescriptor *)&(this->ErrorCbk),
			128);
	if (error != OMX_ErrorNone){
		DBGT_ERROR("Error : Bind MemGrabctl->Host (alert_error)  (error = %d)\n",error);
		OstTraceFiltInst1 (TRACE_ERROR, "Error : Bind MemGrabctl->Host (alert_error)  (error = 0x%x)",error);
		//  OUTR(" ",error);
		return error;
	}

    //CBeventhandler eventhandler = {this, eventHandler_cb};
	error = ENS::bindComponentToHost(
			mENSComponent.getOMXHandle(),
			iMemGrabControl.getMemControllerHandle(),
			"proxy",
			(NMF::InterfaceDescriptor*) this,
			6);
	if (error != OMX_ErrorNone){
		OUTR(" ",(error));
      	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::instantiate (0x%x)", error);
		return error;
	}

	error = ENS::bindComponentAsynchronous(
			iMemGrabControl.getMemControllerHandle(),
			"me",
			iMemGrabControl.getMemControllerHandle(),
			"postevent",
				 mENSComponent.getPort(ISPPROC_PORT_OUT1)->getBufferCountActual()
				+mENSComponent.getPort(ISPPROC_PORT_OUT0)->getBufferCountActual()
				+mENSComponent.getPort(ISPPROC_PORT_IN0)->getBufferCountActual());
	if (error != OMX_ErrorNone){
		OUTR(" ",(error));
      	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::instantiate (0x%x)", error);
		return error;
	}

	/* Resource sharer */
	iResourceSharerManager.setOMXHandle(mENSComponent.getOMXHandle());


	error =	iResourceSharerManager.instantiateNMF();
	if(error!= OMX_ErrorNone)
	{
		DBGT_ERROR("iResourceSharerManager.instantiateNMF 0x%X\n", error);
		OstTraceFiltInst1 (TRACE_ERROR, "iResourceSharerManager.instantiateNMF 0x%X", error);
		OUTR(" ",(OMX_ErrorHardware));
      	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::instantiate OMX_ErrorHardware");
		return OMX_ErrorHardware;	
	}		
		
		
	error = iResourceSharerManager.bindNMF();
	if(error!= OMX_ErrorNone)
	{
		DBGT_ERROR("iResourceSharerManager.bindNMF 0x%X\n", error);
		OstTraceFiltInst1 (TRACE_ERROR, "iResourceSharerManager.bindNMF 0x%X", error);
		OUTR(" ",(OMX_ErrorHardware));
      	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::instantiate OMX_ErrorHardware");
		return OMX_ErrorHardware;	
	}		

	pTuningDataBase = CTuningDataBase::getInstance(ISPPROC_TUNING_DATABASE_CLIENT_NAME);
	if(pTuningDataBase == NULL) {
		DBGT_ERROR("Error: could not bind to the Tuning Data Base: pTuningDataBase==NULL\n");
		OstTraceFiltInst0 (TRACE_ERROR, "Error: could not bind to the Tuning Data Base: pTuningDataBase==NULL");
		OUTR(" ",(OMX_ErrorInsufficientResources));
      	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::instantiate OMX_ErrorInsufficientResources");
		return OMX_ErrorInsufficientResources;
	}

	t_isp_dampers_error_code dErr = ISP_DAMPERS_OK;
	dErr = iIspDampers.Construct();
	if(dErr!=ISP_DAMPERS_OK) {
		DBGT_ERROR("Error: iIspDampers.Construct() err=%d (%s)\n", dErr, CIspDampers::ErrorCode2String(dErr));
		OstTraceFiltInst1 (TRACE_ERROR, "Error: iIspDampers.Construct() err=%d", dErr);
		OUTR(" ",(OMX_ErrorInsufficientResources));
      	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::instantiate OMX_ErrorInsufficientResources");
		return OMX_ErrorInsufficientResources;
	}

	OUTR(" ",(error));
  	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::instantiate (%d)", error);
	return error;
}

OMX_ERRORTYPE IspProc::stop() {
	IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::stop");
    OMX_ERRORTYPE omxerr = OMX_ErrorNone;
    
	
	omxerr= iIspctlComponentManager.stopNMF();
	if(omxerr!= OMX_ErrorNone)
	{
		DBGT_ERROR("iIspctlComponentManager.stopNMF 0x%X\n", omxerr);
		OstTraceFiltInst1 (TRACE_ERROR, "iIspctlComponentManager.stopNMF 0x%X", omxerr);
		OUTR(" ",(OMX_ErrorHardware));
	  	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::stop OMX_ErrorHardware");
		return OMX_ErrorHardware;	
	}	    
	
	
	omxerr= iMemGrabControl.stop();
	if(omxerr!= OMX_ErrorNone)
	{
		DBGT_ERROR("iMemGrabControl.stop 0x%X\n", omxerr);
		OstTraceFiltInst1 (TRACE_ERROR, "iMemGrabControl.stop 0x%X", omxerr);
		OUTR(" ",(OMX_ErrorHardware));
	  	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::stop OMX_ErrorHardware");
		return OMX_ErrorHardware;	
	}	    
	
	
	omxerr= iResourceSharerManager.stopNMF();
	if(omxerr!= OMX_ErrorNone)
	{
		DBGT_ERROR("iResourceSharerManager.stopNMF 0x%X\n", omxerr);
		OstTraceFiltInst1 (TRACE_ERROR, "iResourceSharerManager.stopNMF 0x%X", omxerr);
		OUTR(" ",(OMX_ErrorHardware));
	  	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::stop OMX_ErrorHardware");
		return OMX_ErrorHardware;	
	}		
	
	
	OUTR(" ",(omxerr));
  	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::stop (%d)", omxerr);
	return omxerr;
}

OMX_ERRORTYPE IspProc::configure() {
	IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::configure");
	OMX_ERRORTYPE omxerr = OMX_ErrorNone;

	OUTR(" ",omxerr);
  	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::configure (%d)", omxerr);
	return omxerr;
}

OMX_ERRORTYPE IspProc::doSpecificSendCommand(OMX_COMMANDTYPE cmd, OMX_U32 nParam, OMX_BOOL &bDeferredCmd)
{
    IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::doSpecificSendCommand");
    OMX_ERRORTYPE error = OMX_ErrorUndefined;
    // if a port has been enabled, then sharedbuf interfaces should be bound here
    if(cmd ==OMX_CommandPortEnable) {
        if(nParam==OMX_ALL){
            error = bindSharedBuf(ISPPROC_PORT_OUT0);
            if (OMX_ErrorNone != error) {
                return error;
            }
            error = bindSharedBuf(ISPPROC_PORT_OUT1);
            if (OMX_ErrorNone != error) {
                return error;
            }
            error = bindSharedBuf(ISPPROC_PORT_IN0);
            if (OMX_ErrorNone != error) {
                return error;
            }
        }
        else {
            error = bindSharedBuf(nParam);
            if (OMX_ErrorNone != error) {
                return error;
            }
        }
    }

    bDeferredCmd = OMX_FALSE;

    OUTR(" ",(OMX_ErrorNone));
    OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::doSpecificSendCommand (%d)", OMX_ErrorNone);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE IspProc::deInstantiate() {
	IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry IspProc::deInstantiate");
	OMX_ERRORTYPE error = OMX_ErrorNone;



	error = ENS::unbindComponentFromHost((NMF::InterfaceReference *) getNmfSendCommandItf());
	if (error != OMX_ErrorNone){
		OUTR(" ",(error));
	  	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::deInstantiate (%d)", error);
		return error;
	}

	error = ENS::unbindComponentFromHost((NMF::InterfaceReference *) getNmfFsmInitItf());
	if (error != OMX_ErrorNone){
		OUTR(" ",(error));
	  	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::deInstantiate (%d)", error);
		return error;
	}

    error = unbindSharedBuf(ISPPROC_PORT_OUT0);
    if (error != OMX_ErrorNone){
        OUTR(" ",(error));
        OstTraceFiltInst1(TRACE_FLOW, "IspProc::deInstantiate : unbindSharedBuf(ISPPROC_PORT_OUT0) (%d)", error);
        return error;
    }
    error = unbindSharedBuf(ISPPROC_PORT_OUT1);
    if (error != OMX_ErrorNone){
        OUTR(" ",(error));
        OstTraceFiltInst1(TRACE_FLOW, "IspProc::deInstantiate : unbindSharedBuf(ISPPROC_PORT_OUT1) (%d)", error);
        return error;
    }
    error = unbindSharedBuf(ISPPROC_PORT_IN0);
    if (error != OMX_ErrorNone){
        OUTR(" ",(error));
        OstTraceFiltInst1(TRACE_FLOW, "IspProc::deInstantiate : unbindSharedBuf(ISPPROC_PORT_IN0) (%d)", error);
        return error;
    }

	error = ENS::unbindComponentToHost(
			mENSComponent.getOMXHandle(),
			iMemGrabControl.getMemControllerHandle()
			, "proxy");
	if (error != OMX_ErrorNone){
		OUTR(" ",(error));
	  	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::deInstantiate (%d)", error);
		return error;
	}

	error = ENS::unbindComponentToHost(
            mENSComponent.getOMXHandle(),
            iMemGrabControl.getMemControllerHandle()
			, "alert_error");
	if (error != OMX_ErrorNone){
		OUTR(" ",(error));
	  	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::deInstantiate (%d)", error);
		return error;
	}

	error = ENS::unbindComponentAsynchronous(
			iMemGrabControl.getMemControllerHandle()
			, "me");
	if (error != OMX_ErrorNone){
		OUTR(" ",(error));
	  	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::deInstantiate (%d)", error);
		return error;
	}

	/* free remaining buffers (doing it in destructor is too late for RME) */
	// fix for ER318336
    	error = iMemGrabControl.freeGrabCache();
    	if (error != OMX_ErrorNone) {
        	return error;
 	}


	error = iIspctlComponentManager.deinstantiateNMF();
	if(error!= OMX_ErrorNone)
	{
		DBGT_ERROR("iIspctlComponentManager.deinstantiateNMF 0x%X\n", error);
		OstTraceFiltInst1 (TRACE_ERROR, "iIspctlComponentManager.deinstantiateNMF 0x%X", error);
		OUTR(" ",(OMX_ErrorHardware));
	  	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::deInstantiate OMX_ErrorHardware");
		return OMX_ErrorHardware;	
	}	
	
	error=iMemGrabControl.deinstantiate();
	if(error!= OMX_ErrorNone)
	{
		DBGT_ERROR("iMemGrabControl.deinstantiate 0x%X\n", error);
		OstTraceFiltInst1 (TRACE_ERROR, "iMemGrabControl.deinstantiate 0x%X", error);
		OUTR(" ",(OMX_ErrorHardware));
	  	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::deInstantiate OMX_ErrorHardware");
		return OMX_ErrorHardware;	
	}	
	
	error = iResourceSharerManager.deinstantiateNMF();
	if(error!= OMX_ErrorNone)
	{
		DBGT_ERROR("iResourceSharerManager.deinstantiateNMF 0x%X\n", error);
		OstTraceFiltInst1 (TRACE_ERROR, "iResourceSharerManager.deinstantiateNMF 0x%X", error);
		OUTR(" ",(OMX_ErrorHardware));
	  	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::deInstantiate OMX_ErrorHardware");
		return OMX_ErrorHardware;	
	}

	t_isp_dampers_error_code dErr = ISP_DAMPERS_OK;
	dErr = iIspDampers.Destroy();
	if(dErr!=ISP_DAMPERS_OK) {
		DBGT_ERROR("Error: iIspDampers.Destroy() err=%d (%s)\n", dErr, CIspDampers::ErrorCode2String(dErr));
		OstTraceFiltInst1 (TRACE_ERROR, "Error: iIspDampers.Destroy() err=%d", dErr);
		OUTR(" ", OMX_ErrorHardware);
	  	OstTraceFiltInst0(TRACE_FLOW, "Exit IspProc::deInstantiate OMX_ErrorHardware");
		return OMX_ErrorHardware;
	}

	if(pTuningDataBase != NULL) {
		pTuningDataBase->releaseInstance(ISPPROC_TUNING_DATABASE_CLIENT_NAME);
		MSG1("Unbound from Tuning Data Base: pTuningDataBase=%p\n", pTuningDataBase);
		OstTraceFiltInst1 (TRACE_DEBUG, "Unbound from Tuning Data Base: pTuningDataBase=0x%x", (t_uint32)pTuningDataBase);
		pTuningDataBase = NULL;
	}

	/* Exit from COM_SM Framework */
	s_scf_event devent;
	devent.sig = Q_FINAL_SIG;
	p_isp_sm->ProcessEvent(&devent);
	destroySM();

	OUTR(" ",(error));
  	OstTraceFiltInst1(TRACE_FLOW, "Exit IspProc::deInstantiate (%d)", error);
	return error;
}

OMX_ERRORTYPE IspProc::unbindSharedBuf(OMX_U32 portIndex,OMX_BOOL isDisabling)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;

	if (portIndex == ISPPROC_PORT_OUT0)
	{
		ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(ISPPROC_PORT_OUT0));
		if (port->isEnabled() == OMX_TRUE || isDisabling)
		{
			error = ENS::unbindComponent(getNmfSharedBuf(ISPPROC_PORT_OUT0), "mpc");
			if (error != OMX_ErrorNone) return error;

			error = ENS::unbindComponent(iMemGrabControl.getMemControllerHandle(), "fillbufferdone[0]");
			if (error != OMX_ErrorNone) return error;
		}
	}
	else if (portIndex == ISPPROC_PORT_OUT1)
	{
		ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(ISPPROC_PORT_OUT1));
		if (port->isEnabled() == OMX_TRUE || isDisabling)
		{
			error = ENS::unbindComponent(getNmfSharedBuf(ISPPROC_PORT_OUT1), "mpc");
			if (error != OMX_ErrorNone) return error;

			error = ENS::unbindComponent(iMemGrabControl.getMemControllerHandle(), "fillbufferdone[1]");
			if (error != OMX_ErrorNone) return error;
		}
	}
	else if (portIndex == ISPPROC_PORT_IN0)
	{
		ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(ISPPROC_PORT_IN0));
		if (port->isEnabled() == OMX_TRUE || isDisabling)
		{
			error = ENS::unbindComponent(getNmfSharedBuf(ISPPROC_PORT_IN0), "mpc");
			if (error != OMX_ErrorNone) return error;

			error = ENS::unbindComponent(iMemGrabControl.getMemControllerHandle(), "emptybufferdone");
			if (error != OMX_ErrorNone) return error;
		}
	}

	return error;
}

OMX_ERRORTYPE IspProc::bindSharedBuf(OMX_U32 portIndex)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;




	ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(portIndex));

	if (port->isEnabled() == OMX_TRUE)
	{
		if (portIndex == ISPPROC_PORT_OUT0 )//1
		{
			error = ENS::bindComponent(getNmfSharedBuf(portIndex), "mpc", iMemGrabControl.getMemControllerHandle(), "fillthisbuffer[1]");
			if (error != OMX_ErrorNone) return error;

			error = ENS::bindComponent(iMemGrabControl.getMemControllerHandle(), "fillbufferdone[0]", getNmfSharedBuf(portIndex), "emptythisbuffer");
			if (error != OMX_ErrorNone) return error;
		}
		else if (portIndex == ISPPROC_PORT_OUT1 )//2
		{
			error = ENS::bindComponent(getNmfSharedBuf(portIndex), "mpc", iMemGrabControl.getMemControllerHandle(), "fillthisbuffer[2]");
			if (error != OMX_ErrorNone) return error;

			error = ENS::bindComponent(iMemGrabControl.getMemControllerHandle(), "fillbufferdone[1]", getNmfSharedBuf(portIndex), "emptythisbuffer");
			if (error != OMX_ErrorNone) return error;
		}
		else if (portIndex == ISPPROC_PORT_IN0 )
		{
			error = ENS::bindComponent(getNmfSharedBuf(portIndex), "mpc", iMemGrabControl.getMemControllerHandle(), "emptythisbuffer");
			if (error != OMX_ErrorNone) return error;

			error = ENS::bindComponent(iMemGrabControl.getMemControllerHandle(), "emptybufferdone", getNmfSharedBuf(portIndex), "fillthisbuffer");
			if (error != OMX_ErrorNone) return error;
		}

	}
	return error;
}


OMX_ERRORTYPE IspProc::copyExtradata_Buffer2Mem(OMX_BUFFERHEADERTYPE* pOmxBufHdr)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
		
	if (0 != pOmxBufHdr->nFilledLen)
	{
		error = Extradata::CopyExtradata_Buffer2Mem(pOmxBufHdr, &ExtradataTab[ExtradataStoreCounter][0], EXTRADATA_STILLPACKSIZE);
		ExtradataStoreCounter = (ExtradataStoreCounter+1)%MAX_ALLOCATED_BUFFER_NB;
	}
	
	return error;
}


OMX_ERRORTYPE IspProc::copyExtradata_Mem2Buffer(OMX_BUFFERHEADERTYPE* pOmxBufHdr)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
		
	if (0 != pOmxBufHdr->nFilledLen)
	{
		MSG1("IspProc::copyExtradata_Mem2Buffer : copy extradata of frame index %d)\n",ExtradataLoadCounter);
		OstTraceFiltInst1 (TRACE_DEBUG, "IspProc::copyExtradata_Mem2Buffer : copy extradata of frame index %d)",ExtradataLoadCounter);
		error = Extradata::CopyExtradata_Mem2Buffer(pOmxBufHdr, &ExtradataTab[ExtradataLoadCounter][0], EXTRADATA_STILLPACKSIZE);
	}
	return error;
}


OMX_ERRORTYPE IspProc::getBMSCaptureContext(IFM_BMS_CAPTURE_CONTEXT* pExtradataData)
{
	OMX_VERSIONTYPE version;
	version.nVersion = 0;

	OMX_ERRORTYPE error = Extradata::GetExtradataFieldFromMem(&ExtradataTab[ExtradataLoadCounter][0], pExtradataData, version);

	return error;
}


OMX_ERRORTYPE IspProc::getCaptureParameters(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE* pExtradataData)
{
	OMX_VERSIONTYPE version;
	version.nVersion = 0;

	OMX_ERRORTYPE error = Extradata::GetExtradataFieldFromMem(&ExtradataTab[ExtradataLoadCounter][0], pExtradataData, version);
	return error;
}

//Added for extra data capture paramenter
OMX_ERRORTYPE IspProc::getCaptureParameters(OMX_STE_CAPTUREPARAMETERSTYPE* pExtradataData)
{
       OMX_VERSIONTYPE version;
       version.nVersion = 0;
	   
       OMX_ERRORTYPE error = Extradata::GetExtradataFieldFromMem(&ExtradataTab[ExtradataLoadCounter][0], pExtradataData, version);
                            
       return error;
}

OMX_ERRORTYPE IspProc::getCaptureParameters(OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE* pExtradataData)
{
	OMX_VERSIONTYPE version;
	version.nVersion = 0;

	OMX_ERRORTYPE error = Extradata::GetExtradataFieldFromMem(&ExtradataTab[ExtradataLoadCounter][0], pExtradataData, version);
	return error;
}

OMX_ERRORTYPE IspProc::setCaptureParameters(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_CAPTUREPARAMETERSTYPE* pExtradataData, OMX_VERSIONTYPE version)
{
	OMX_ERRORTYPE error = Extradata::SetExtradataField(pOmxBufHdr, pExtradataData, version);
	return error;
}

OMX_ERRORTYPE IspProc::getDampersData(IFM_DAMPERS_DATA* pExtradataData)
{
	OMX_VERSIONTYPE version;
	version.nVersion = 0;

	OMX_ERRORTYPE error = Extradata::GetExtradataFieldFromMem(&ExtradataTab[ExtradataLoadCounter][0], pExtradataData, version);

	return error;
}


void IspProc::retrieveCurrentFrameExtradata(void)
{
	ExtradataLoadCounter = (ExtradataLoadCounter+1)%MAX_ALLOCATED_BUFFER_NB;
	return;
}


/* 
  called from COmxIspProc::NmfPanicCallback in case of NMF panic on SIA.
  Brutally destroy the whole NMF network on DSP side
  Note that in this function we FORCE all components to stop and destroy 
    whatever the error code returned by NMF functions !
*/
OMX_ERRORTYPE IspProc::errorRecovery()
{
    MSG0("OMX.ISPPROC Entering error recovery ---- destroying NMF network\n");
    OstTraceFiltInst0 (TRACE_DEBUG, "OMX.ISPPROC Entering error recovery ---- destroying NMF network");
    OMX_ERRORTYPE error = OMX_ErrorUndefined;

    error = iMemGrabControl.stop();
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (iMemGrabControl.stop error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (iMemGrabControl.stop error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::stopNMFComponent(getNmfSharedBuf(ISPPROC_PORT_OUT0));
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (stopNMFComponent(getNmfSharedBuf(ISPPROC_PORT_OUT0)) error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (stopNMFComponent(getNmfSharedBuf(ISPPROC_PORT_OUT0)) error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::stopNMFComponent(getNmfSharedBuf(ISPPROC_PORT_OUT1));
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (stopNMFComponent(getNmfSharedBuf(ISPPROC_PORT_OUT1)) error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (stopNMFComponent(getNmfSharedBuf(ISPPROC_PORT_OUT1)) error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::stopNMFComponent(getNmfSharedBuf(ISPPROC_PORT_IN0));
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (stopNMFComponent(getNmfSharedBuf(ISPPROC_PORT_IN0)) error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (stopNMFComponent(getNmfSharedBuf(ISPPROC_PORT_IN0)) error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::unbindComponentFromHost((NMF::InterfaceReference *) getNmfSendCommandItf());
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponentFromHost SendCommand error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponentFromHost SendCommand error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::unbindComponentFromHost((NMF::InterfaceReference *) getNmfFsmInitItf());
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponentFromHost FsmInit error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponentFromHost FsmInit error =0x%x)", (&mENSComponent),error);
    }
    error = unbindSharedBuf(ISPPROC_PORT_OUT0);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (unbindSharedBuf ISPPROC_PORT_OUT0 error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (unbindSharedBuf ISPPROC_PORT_OUT0 error =0x%x)", (&mENSComponent),error);
    }
    error = unbindSharedBuf(ISPPROC_PORT_OUT1);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (unbindSharedBuf ISPPROC_PORT_OUT1 error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (unbindSharedBuf ISPPROC_PORT_OUT1 error =0x%x)", (&mENSComponent),error);
    }
    error = unbindSharedBuf(ISPPROC_PORT_IN0);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (unbindSharedBuf ISPPROC_PORT_IN0 error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (unbindSharedBuf ISPPROC_PORT_IN0 error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::unbindComponent(iMemGrabControl.getMemControllerHandle(), "fillbufferdone[0]");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponent fillbufferdone[0] error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponent fillbufferdone[0] error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::unbindComponent(iMemGrabControl.getMemControllerHandle(), "fillbufferdone[1]");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponent fillbufferdone[1] error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponent fillbufferdone[1] error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::unbindComponent(iMemGrabControl.getMemControllerHandle(), "emptybufferdone");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponent emptybufferdone error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponent emptybufferdone error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::unbindComponentToHost(mENSComponent.getOMXHandle(),iMemGrabControl.getMemControllerHandle(),"proxy");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponentToHost proxy error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponentToHost proxy error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::unbindComponentToHost(mENSComponent.getOMXHandle(),iMemGrabControl.getMemControllerHandle(),"alert_error");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponentToHost alert_error error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponentToHost alert_error error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::unbindComponentAsynchronous(iMemGrabControl.getMemControllerHandle(),"me");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponentAsynchronous me error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponentAsynchronous me error =0x%x)", (&mENSComponent),error);
    }
    error = iMemGrabControl.forcedDestroy();
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (iMemGrabControl.forcedDestroy error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (iMemGrabControl.forcedDestroy error =0x%x)", (&mENSComponent),error);
    }

    error = ENS::unbindComponentFromHost(&mIfillThisBuffer[ISPPROC_PORT_OUT0]);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponentFromHost fillThisBuffer ISPPROC_PORT_OUT0 error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponentFromHost fillThisBuffer ISPPROC_PORT_OUT0 error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::unbindComponentToHost(&mENSComponent, getNmfSharedBuf(ISPPROC_PORT_OUT0), "host");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponentToHost host ISPPROC_PORT_OUT0 error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponentToHost host ISPPROC_PORT_OUT0 error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::destroyNMFComponent(getNmfSharedBuf(ISPPROC_PORT_OUT0));
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::destroyNMFComponent(getNmfSharedBuf(ISPPROC_PORT_OUT0)) error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::destroyNMFComponent(getNmfSharedBuf(ISPPROC_PORT_OUT0)) error =0x%x)", (&mENSComponent),error);
    }

    error = ENS::unbindComponentFromHost(&mIfillThisBuffer[ISPPROC_PORT_OUT1]);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponentFromHost fillThisBuffer ISPPROC_PORT_OUT1 error =0x%x)\n",error);
    }
    error = ENS::unbindComponentToHost(&mENSComponent, getNmfSharedBuf(ISPPROC_PORT_OUT1), "host");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponentToHost host ISPPROC_PORT_OUT1 error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponentToHost host ISPPROC_PORT_OUT1 error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::destroyNMFComponent(getNmfSharedBuf(ISPPROC_PORT_OUT1));
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::destroyNMFComponent(getNmfSharedBuf(ISPPROC_PORT_OUT1)) error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::destroyNMFComponent(getNmfSharedBuf(ISPPROC_PORT_OUT1)) error =0x%x)", (&mENSComponent),error);
    }

    error = ENS::unbindComponentFromHost(&mIemptyThisBuffer[ISPPROC_PORT_IN0]);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponentFromHost emptyThisBuffer ISPPROC_PORT_IN0 error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponentFromHost emptyThisBuffer ISPPROC_PORT_IN0 error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::unbindComponentToHost(&mENSComponent, getNmfSharedBuf(ISPPROC_PORT_IN0), "host");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponentToHost host ISPPROC_PORT_IN0 error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponentToHost host ISPPROC_PORT_IN0 error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::destroyNMFComponent(getNmfSharedBuf(ISPPROC_PORT_IN0));
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::destroyNMFComponent(getNmfSharedBuf(ISPPROC_PORT_IN0)) error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::destroyNMFComponent(getNmfSharedBuf(ISPPROC_PORT_IN0)) error =0x%x)", (&mENSComponent),error);
    }

    error = ENS::stopNMFComponent(mNmfGenericFsmLib);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::stopNMFComponent(mNmfGenericFsmLib) error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::stopNMFComponent(mNmfGenericFsmLib) error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::stopNMFComponent(mNmfComponentFsmLib);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::stopNMFComponent(mNmfComponentFsmLib) error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::stopNMFComponent(mNmfComponentFsmLib) error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::stopNMFComponent(mOstTrace);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::stopNMFComponent(mOstTrace) error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::stopNMFComponent(mOstTrace) error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::unbindComponent(mNmfComponentFsmLib, "genericfsm");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::unbindComponent genericfsm error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::unbindComponent genericfsm error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::unbindComponent(mNmfComponentFsmLib, "osttrace");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery ( error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery ( error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::destroyNMFComponent(mOstTrace);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::destroyNMFComponent(mOstTrace) error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::destroyNMFComponent(mOstTrace) error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::destroyNMFComponent(mNmfComponentFsmLib);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::destroyNMFComponent(mNmfComponentFsmLib) error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::destroyNMFComponent(mNmfComponentFsmLib) error =0x%x)", (&mENSComponent),error);
    }
    error = ENS::destroyNMFComponent(mNmfGenericFsmLib);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (ENS::destroyNMFComponent(mNmfGenericFsmLib) error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (ENS::destroyNMFComponent(mNmfGenericFsmLib) error =0x%x)", (&mENSComponent),error);
    }

    /* stop & deinstantiate ISPCTL */
    error = iIspctlComponentManager.forcedStopNMF();
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (iIspctlComponentManager.forcedStopNMF error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (iIspctlComponentManager.forcedStopNMF error =0x%x)", (&mENSComponent),error);
    }
    error = iIspctlComponentManager.deinstantiateNMF();
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("IspProc::errorRecovery (iIspctlComponentManager.deinstantiateNMF error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "IspProc::errorRecovery (iIspctlComponentManager.deinstantiateNMF error =0x%x)", (&mENSComponent),error);
    }

    MSG0("OMX.ISPPROC error recovery ---- NMF network destroyed\n"); 
    OstTraceFiltInst0 (TRACE_DEBUG, "OMX.ISPPROC error recovery ---- NMF network destroyed");

    return OMX_ErrorNone;
}


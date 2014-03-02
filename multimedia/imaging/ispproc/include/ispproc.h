/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ISPPROC_H_
#define _ISPPROC_H_


#include "hsmispproc.h" //rename_me ispproc_sm.h
#include "IFM_NmfProcessingComp.h"
#include "IFM_Index.h"
#include "IFM_Types.h"
#include "ispctl_component_manager.h"
#include "ispprocport.h"
#include "extradata.h"
#include "tuning_data_base.h"
#include "isp_dampers.h"
#include "picture_settings.h"
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
#include "isp_dampers_ext.h"
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
#include "VhcElementDefs.h"
#include "host/grabctl/api/error.hpp"


#undef OMX_TRACE_UID
#define OMX_TRACE_UID 8

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(ispprocErrorCbk);
#endif

class ispprocErrorCbk : public grabctl_api_errorDescriptor {
public:
virtual void error(enum e_grabError error_id, t_uint16 data, enum e_grabPipeID pipe_id);
};



#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(IspProc);
#endif
class IspProc : public IFM_NmfProcessingComp{
public:

    IspProc(ENS_Component &enscomp, enumCameraSlot cam=ePrimaryCamera);
	virtual ~IspProc();

	ISPPROC_SM *p_isp_sm;
	ispprocErrorCbk ErrorCbk;

	static t_uint16 ispprocInstanceCounter;

	virtual OMX_ERRORTYPE applyConfig(
		OMX_INDEXTYPE nIndex,
		OMX_PTR pStructure
		);

	virtual OMX_ERRORTYPE retrieveConfig(
		OMX_INDEXTYPE nConfigIndex,
		OMX_PTR pComponentConfigStructure
		);

	virtual OMX_ERRORTYPE instantiate();
	virtual OMX_ERRORTYPE start() ;
	virtual OMX_ERRORTYPE stop() ;

	virtual OMX_ERRORTYPE configure();

	virtual OMX_ERRORTYPE deInstantiate() ;

    OMX_ERRORTYPE errorRecovery(void);

    virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE cmd, OMX_U32 nParam, OMX_BOOL &bDeferredCmd) ;

    virtual OMX_ERRORTYPE doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &bDeferredEventHandler) ;


 virtual void doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);
		virtual void doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);


	/* get BMS capture context extradata from internal memory */
	/* pointer must be allocated */
	OMX_ERRORTYPE getBMSCaptureContext(IFM_BMS_CAPTURE_CONTEXT* pExtradataData);
	OMX_ERRORTYPE getCaptureParameters(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE* pExtradataData);
	OMX_ERRORTYPE getCaptureParameters(OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE* pExtradataData);
	OMX_ERRORTYPE getCaptureParameters(OMX_STE_CAPTUREPARAMETERSTYPE* pExtradataData); //Added for extra capture parameter - 3 channel gamma values
	OMX_ERRORTYPE setCaptureParameters(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_CAPTUREPARAMETERSTYPE* pExtradataData, OMX_VERSIONTYPE version);
	OMX_ERRORTYPE getDampersData(IFM_DAMPERS_DATA* pExtradataData);
	
	/* set extradata tab index to current frame extradata index */
	void retrieveCurrentFrameExtradata(void);


	IFM_BMS_CAPTURE_CONTEXT BMS_capture_context; /* BMS capture context to restore */

	//performance traces start
	IFM_LATENCY_ISPPROC mlatency_Ispproc;	/*latency values for ispproc in this structure*/
	//performance traces end

	static t_uint16 getInstance() { return ispprocInstanceCounter;};
	static void incrInstance(void) {  ispprocInstanceCounter++;};
	static void decrInstance(void) {  ispprocInstanceCounter--;};
    
    //vineet
    t_uint32 getStripeCount(){return stripeCount;};
    t_uint32 getStripeNumber(){return stripeNumber;};
    //vineet
protected:

    void fillBufferDoneVPB1(OMX_BUFFERHEADERTYPE *pOMXBuffer);
    void fillBufferDoneVPB2(OMX_BUFFERHEADERTYPE *pOMXBuffer);

    //static void emptyBufferDone_cb(void * ctxt, t_uint32 dspBufferHdrArmAddress);
    void emptyBufferDoneVPB0(OMX_BUFFERHEADERTYPE *pOMXBuffer);

    OMX_ERRORTYPE emptyThisBufferVPB0(OMX_BUFFERHEADERTYPE* pBuffer);

    static void grabctlError_cb(void *ctx, enum e_grabError error_id, t_uint16 data, enum e_grabPipeID pipe_id) ;

    OMX_ERRORTYPE unbindSharedBuf(OMX_U32 portIndex, OMX_BOOL isDisabling=OMX_FALSE);
    OMX_ERRORTYPE bindSharedBuf(OMX_U32 portIndex);




private :

	void constructSM(ENS_Component &enscomp);
	void destroySM();

	/* copy extradata from OMX buffer to internal memory */
	OMX_ERRORTYPE copyExtradata_Buffer2Mem(OMX_BUFFERHEADERTYPE* pOmxBufHdr);
	/* copy extradata from internal memory to OMX buffer */
	OMX_ERRORTYPE copyExtradata_Mem2Buffer(OMX_BUFFERHEADERTYPE* pOmxBufHdr);
	/* memory dedicated to store extradata (bursts using more than MAX_ALLOCATED_BUFFER_NB buffers are not supported) */
	#define MAX_ALLOCATED_BUFFER_NB 10	
	OMX_U8 ExtradataTab[MAX_ALLOCATED_BUFFER_NB][EXTRADATA_STILLPACKSIZE];
	/* indexes in ExtradataTab (last Extradata array written/read) */
	t_uint8 ExtradataStoreCounter;
	t_uint8 ExtradataLoadCounter;

	// Delegates
	CIspctlComponentManager iIspctlComponentManager;
	CIspctlCom iIspctlCom;
	CMemGrabControl iMemGrabControl;
	COmxStateMgr iOmxStateMgr;
	CTrace iTrace;
	CDefferedEventMgr iDeferredEventMgr;
	CResourceSharerManager iResourceSharerManager;
	OMX_BOOL AutoPauseAfterCapture;

	/* Tuning */
	CTuningDataBase* pTuningDataBase;
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
	CIspDampersExt iIspDampers;
#else // CAMERA_ENABLE_OMX_3A_EXTENSION
	CIspDampers iIspDampers;
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
	CPictureSettings iPictureSettings;

	int rotation;
	int bAutoRotation;
	bool bCaptureRequestServed;
	OMX_BOOL bPortSettingChanged; // Is ispproc waiting for port_enable after update of port settings (e.g. rotation case)?
	t_uint32 stripeCount;
	t_uint32 stripeNumber;
	t_uint32 stripeSyncCount;
};

#endif

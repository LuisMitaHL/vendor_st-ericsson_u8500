
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define DBGT_PREFIX "CGrabControl"
#include "grabctl.h"
#include "ENS_DBC.h"
#include "osi_trace.h"
#include "ENS_Nmf.h"
#include "camera.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_grabctlTraces.h"
#endif

CGrabControl::~CGrabControl()
{
   OMX_ERRORTYPE error = freeStabBuffers();
    if (error != OMX_ErrorNone) {
           DBGT_ERROR("~CGrabControl: couldn't free Stab buffers, error %d\n", error);
           OstTraceFiltStatic1(TRACE_ERROR, "~CGrabControl: couldn't free Stab buffers, error %d", (&mENSComponent), error);
    }
}

void CGrabControl::setConfigureControl(Igrabctl_api_configure configureControl)
{
	mConfigureControl = configureControl;
}

void CGrabControl::setStabConfigure(Istab_api_configure stabConfigure)
{
    mStabConfigure = stabConfigure;
}

void CGrabControl::setBMLCapture(t_uint16 port_idx)
{
	MSG1("setBMLCapture ports are ignored!!!By default capturing on ports3 and 4%d\n", port_idx);
	OstTraceFiltStatic1(TRACE_DEBUG, "setBMLCapture %d", (&mENSComponent), port_idx);
	mConfigureControl.setConfig(GRBCTLPID_PORT_1,GRABCTL_CFG_CAPTURING,GRBCTL_CAPT_AFTERBML_1,0);
}
void CGrabControl::setEndCapture(t_uint16 port_idx)
{
	MSG1("StartEndCapture %d\n", port_idx);
	OstTraceFiltStatic1(TRACE_DEBUG, "StartEndCapture %d", (&mENSComponent), port_idx);
    grabPortsState[port_idx] = GRAB_STATE_ENDCAPTURE;
	mConfigureControl.setConfig((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,GRBCTL_CAPT_PENDING_EOS,0);
}

void CGrabControl::setEndCaptureNow(t_uint16 port_idx)
{
	MSG1("setEndCaptureNow %d\n", port_idx);
	OstTraceFiltStatic1(TRACE_DEBUG, "setEndCaptureNow %d", (&mENSComponent), port_idx);
    grabPortsState[port_idx] = GRAB_STATE_ENDCAPTURENOW;
	mConfigureControl.setConfig((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,GRBCTL_CAPT_STOP_NOW,0);
}


void CGrabControl::setStartCapture(t_uint16 port_idx)
{
	MSG1("setStartCapture %d\n", port_idx);
	OstTraceFiltStatic1(TRACE_DEBUG, "setStartCapture %d", (&mENSComponent), port_idx);
    grabPortsState[port_idx] = GRAB_STATE_STARTCAPTURE;
	mConfigureControl.setConfig((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,GRBCTL_CAPT_CAPTURING,0);
}

// start infinite grab but keep buffers in dsp
// on capturing bit dsp will forward nbBefore frames captured before capturing bit to camera
void CGrabControl::setStartHiddenCapture(t_uint16 port_idx, t_uint16 nbBefore)
{
	MSG1("StartHiddenCapture %d\n", port_idx);
	OstTraceFiltStatic1(TRACE_DEBUG, "StartHiddenCapture %d", (&mENSComponent), port_idx);
    grabPortsState[port_idx] = GRAB_STATE_STARTHIDDENCAPTURE;
	mConfigureControl.setConfig((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,GRBCTL_CAPT_CAPTURING_KEEP_BUFFERS,nbBefore);
}

void CGrabControl::setEndCaptureforHiddenBMS()
{
	mConfigureControl.endAllCaptures();
}

void CGrabControl::setStartOneShotCapture(t_uint16 port_idx)
{
	MSG1("StartOneShotCapture %d\n", port_idx);
	OstTraceFiltStatic1(TRACE_DEBUG, "StartOneShotCapture %d", (&mENSComponent), port_idx);
    grabPortsState[port_idx] = GRAB_STATE_STARTONESHOTCAPTURE;
	mConfigureControl.setConfig((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,GRBCTL_CAPT_ONE_SHOT,0);
}

void CGrabControl::startBurstCapture(t_uint16 port_idx, t_uint16 nbBurst)
{
	MSG1("startBurstCapture %d\n", port_idx);
	OstTraceFiltStatic1(TRACE_DEBUG, "startBurstCapture %d", (&mENSComponent), port_idx);
    grabPortsState[port_idx] = GRAB_STATE_STARTBURSTCAPTURE;
	mConfigureControl.setConfig((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,GRBCTL_CAPT_BURST,nbBurst);
}


void CGrabControl::setHiddenBMSBuffers(t_uint32* bufferadd, t_uint32 NoOfBuffers, t_uint16 xsize, t_uint16 ysize,t_uint16 linelen,t_uint16 pack)
{
//	MSG2("startBMSWithoutPort buffers:%d, %d\n", NoOfBuffers,bufferadd[0] );
	t_uint16 bufferAddH[6];
	t_uint16 bufferAddL[6];
	for(t_uint32 i=0;i<NoOfBuffers;i++)
	{
		bufferAddH[i] =(t_uint16) ((bufferadd[i]&0xFFFF0000)>>16);
		bufferAddL[i] =(t_uint16) ((bufferadd[i]&0x0FFFF));
	}
//	OstTraceFiltStatic1(TRACE_DEBUG, "startBMSWithoutPort on %d,%d", (&mENSComponent), (t_uint16) ((bufferadd&0xFFFF0000)>>16),(t_uint16)(bufferadd&0xFFFF));/	mConfigureControl.setConfigBMS((e_grabctlPortID)1, (t_uint16) ((bufferadd&0xFFFF0000)>>16),(t_uint16)(bufferadd&0xFFFF),xsize,ysize,linelen,pack);
	mConfigureControl.setConfigBMS((e_grabctlPortID)1, bufferAddH,bufferAddL,(t_uint16)(NoOfBuffers&0xFFFF),xsize,ysize,linelen,pack);
}


void CGrabControl::setStartHiddenBMS(t_uint16 hidden_bms, t_uint16 nbBefore)
{
	MSG1("setHiddenBMS %d\n", hidden_bms);
	
	OstTraceFiltStatic1(TRACE_DEBUG, "setHiddenBMS %d", (&mENSComponent), hidden_bms);
	mConfigureControl.setHiddenBMS(hidden_bms,nbBefore);
}
void CGrabControl::callInterface(t_grab_desc *pGrabDesc, struct s_grabParams grabparams)
{
	mConfigureControl.setParams(grabparams,(e_grabctlPortID)pGrabDesc->port_idx,pGrabDesc->bufferCount);
}

void CGrabControl::configureHook(struct s_grabParams &grabParams, t_uint16 port_idx)
{
    /* TODO: stab use case where HR isn't macroblock? (stab still needs grab cache) */
    if ((CGrabControlCommon::mGrabCacheHandle) && (((grabParams.output_format == GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED) ||
        (grabParams.output_format == GRBFMT_YUV420_MB_SWAP_SEMIPLANAR_DECIMATED)) ^ (mpOpModeMgr->CurrentOperatingMode == OpMode_Cam_VideoPreview))) {
		grabParams.disable_grab_cache = 0;
		grabParams.cache_buf_addr = mCacheAddrPhys;
    } else {
		grabParams.disable_grab_cache = 1;
		grabParams.cache_buf_addr = 0;
    }

    if ((CGrabControlCommon::mGrabCacheHandle) && (stab_buf_allocated)
        && (((port_idx == CAMERA_PORT_OUT2) && (mpOpModeMgr->CurrentOperatingMode == OpMode_Cam_VideoRecord))
        || ((port_idx == CAMERA_PORT_OUT0) && (mpOpModeMgr->CurrentOperatingMode == OpMode_Cam_VideoPreview)))) {
        grabParams.enable_stab=1;

        MMHwBuffer::TBufferInfo bufferInfo;
        OMX_ERRORTYPE omx_error = stab_buf_v->BufferInfo(0, bufferInfo);
        DBC_ASSERT(omx_error == OMX_ErrorNone);
        grabParams.proj_v_buf_addr=bufferInfo.iPhyAddr;

#ifdef HCURVE_IN_ESRAM
//for HCurve buffer in ESRAM start

        t_cm_system_address system_address;
        t_cm_error cm_error = CM_GetMpcMemorySystemAddress(stab_buf_h, &system_address);
        DBC_ASSERT(cm_error == CM_OK);
        grabParams.proj_h_buf_addr=system_address.physical;

//for HCurve buffer in ESRAM end
#else
//for HCurve buffer in DDR start

	    MMHwBuffer::TBufferInfo bufferInfo1;
	    omx_error = stab_buf_h->BufferInfo(0, bufferInfo1);
        DBC_ASSERT(omx_error == OMX_ErrorNone);
	    grabParams.proj_h_buf_addr=bufferInfo1.iPhyAddr;

//for HCurve buffer in DDR end
#endif

        int xsize, ysize, xsize_stab, ysize_stab;
        e_grabFormat grabFormat = GRBFMT_YUV422_MB_SEMIPLANAR;
        getOverscanResolution(grabParams.x_window_size, grabParams.y_window_size, grabParams.output_format, 1.0, xsize, ysize,port_idx);
		if((port_idx == CAMERA_PORT_OUT0) && (xsize > XGA_WIDTH) && IsStabEnabled())
		{
			rectifyOverscanResolution(grabParams.x_window_size,grabParams.y_window_size, xsize, ysize);
		}
        if (mPortRecord) {
			grabFormat = gconvertFormatForFW(mPortRecord->getParamPortDefinition().format.video.eColorFormat, false);
		}
        getOverscanResolution(grabParams.x_window_size, grabParams.y_window_size, grabFormat, 1.0, xsize_stab, ysize_stab,port_idx);
		if((port_idx == CAMERA_PORT_OUT0) && (xsize_stab > XGA_WIDTH) && IsStabEnabled())
		{
			rectifyOverscanResolution(grabParams.x_window_size,grabParams.y_window_size, xsize_stab, ysize_stab);
		}
	if (grabParams.rotation_cfg ==0){
        	grabParams.x_frame_size = grabParams.x_window_size = xsize;
        	grabParams.y_frame_size = grabParams.y_window_size = ysize;
	}else {
		grabParams.x_frame_size = grabParams.y_window_size = xsize;
		grabParams.y_frame_size = grabParams.x_window_size = ysize;
		}
        grabParams.x_stab_size_lr = xsize_stab;
        grabParams.y_stab_size_lr = ysize_stab;

    } else {
        /* this is not the primary stab port. Don't bother with buffer allocation. However, stab may still be enabled
        so still do overscan */
        grabParams.enable_stab = 0;
        int xsize, ysize;
        getOverscanResolution(grabParams.x_window_size, grabParams.y_window_size, grabParams.output_format, 1.0, xsize, ysize,port_idx);
	 	if((port_idx == CAMERA_PORT_OUT0) && (xsize > XGA_WIDTH) && IsStabEnabled())
	 	{
			rectifyOverscanResolution(grabParams.x_window_size,grabParams.y_window_size, xsize, ysize);
	 	}
	if (grabParams.rotation_cfg ==0){
          grabParams.x_window_size = xsize;
          grabParams.y_window_size = ysize;
	}else {
		grabParams.y_window_size = xsize;
		grabParams.x_window_size = ysize;
		}
        grabParams.x_stab_size_lr = 0;
        grabParams.y_stab_size_lr = 0;

    }
}

#include <stdio.h>
OMX_ERRORTYPE CGrabControl::configureStab(bool resSusp, t_sint32 rotation)
{
    OMX_U32 HRColorFormat;
    camport *portLR, *portHR;
    OMX_ERRORTYPE error;

    /* ugly hack: for configureHook, for stab LR-only use-case,
     * VF port needs to know HR port params to configure stab. I assume
     * we always call configureStab before configureGrabResolutionFormat,
     * so sniff the values that come through here
     * OpModeMgr will tell us which ports to usr for as LR and 'HR' */
    mpOpModeMgr->getStabPorts(&portLR, &portHR, &HRColorFormat);

    if (mStabEnabled == true) {
        t_uint32 proj_v_buf_phys=0, proj_h_buf_phys=0;
        t_sint32 * proj_v_buf_log=NULL, *proj_h_buf_log=NULL;
        int LR_xsize=0, LR_ysize=0, HR_xsize=0, HR_ysize=0, HR_max_xsize, HR_max_ysize;
        int hr_x_origin=0, hr_y_origin=0, lr_x_origin=0, lr_y_origin=0;

        if ((mpOpModeMgr->CurrentOperatingMode != OpMode_Cam_VideoPreview) && (mpOpModeMgr->CurrentOperatingMode != OpMode_Cam_VideoRecord)) return OMX_ErrorNone; //Stab only valid for video opmodes

        OMX_PARAM_PORTDEFINITIONTYPE LR_params = portLR->getParamPortDefinition();
        OMX_PARAM_PORTDEFINITIONTYPE HR_params = portHR->getParamPortDefinition();

        /* if resources are suspended, don't call allocateStabBuffers, but rather exit gracefully,
        as it will be allocated later when receiving unsuspendResources */
        if(resSusp)return OMX_ErrorInsufficientResources;



        /* video record case: use HR resolutions */
        error = allocateStabBuffers(portHR, proj_v_buf_phys, proj_v_buf_log, proj_h_buf_phys, proj_h_buf_log,rotation);
        if (error != OMX_ErrorNone) return error;

        mStabConfigure.activateStab(true,
                                    (mpOpModeMgr->CurrentOperatingMode == OpMode_Cam_VideoPreview),
                                    ((Camera*)(&mENSComponent.getProcessingComponent()))->iFramerate.getSensorFramerate_x100()/100);

        getOverscanResolution(portHR->mInternalFrameWidth,
                portHR->mInternalFrameHeight,
                gconvertFormatForFW(HR_params.format.video.eColorFormat, false),
                1.0,
                HR_xsize, HR_ysize,HR_params.nPortIndex);

        hr_x_origin = (HR_xsize - portHR->mInternalFrameWidth)/2;
        hr_y_origin = (HR_ysize - portHR->mInternalFrameHeight)/2;
        /* nStride is size in bytes, we want size in pixels */
        HR_max_xsize = portHR->getMaxOverscannedWidth();
        HR_max_ysize = portHR->getMaxOverscannedHeight();



        DBC_ASSERT(!(HR_max_xsize%16));
        DBC_ASSERT(!(HR_max_ysize%16));

        getOverscanResolution(portLR->mInternalFrameWidth,
                portLR->mInternalFrameHeight,
                gconvertFormatForFW(LR_params.format.video.eColorFormat, false),
                1.0,
                LR_xsize, LR_ysize,LR_params.nPortIndex);
	if((LR_params.nPortIndex == CAMERA_PORT_OUT0)&&(LR_xsize > XGA_WIDTH) && IsStabEnabled())
	{
		rectifyOverscanResolution(portLR->mInternalFrameWidth,portLR->mInternalFrameHeight, LR_xsize, LR_ysize);
	}


        lr_x_origin = (LR_xsize - portLR->mInternalFrameWidth)/2;
        lr_y_origin = (LR_ysize - portLR->mInternalFrameHeight)/2;

        mStabConfigure.informBuffers(proj_h_buf_phys, proj_h_buf_log,
                                     proj_v_buf_phys, proj_v_buf_log,
                                     LR_xsize, LR_ysize, HR_xsize, HR_ysize,
                                     lr_x_origin, lr_y_origin,
                                     hr_x_origin, hr_y_origin,
                                     portLR->getMaxOverscannedWidth(), portLR->getMaxOverscannedHeight(),
                                     HR_max_xsize, HR_max_ysize,rotation);
    } else {
        mStabConfigure.activateStab(false, false, 30);
        error = freeStabBuffers();
        if (OMX_ErrorNone != error) {
            DBGT_ERROR("CGrabControl::configureStab : couldn't free Stab buffers: omx error %d\n", error);
            OstTraceFiltStatic1(TRACE_ERROR, "CGrabControl::configureStab : couldn't free Stab buffers : omx error %d", (&mENSComponent), error);
            return error;
        }

        /* allocate grab cache for HR anyhow */
        OMX_PARAM_PORTDEFINITIONTYPE port_params = portHR->getParamPortDefinition();
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
        // portHR is enabled then we should allocate grab cache.
        if(portHR->isEnabled())
#endif
        if ((port_params.format.video.eColorFormat == OMX_COLOR_FormatYUV420Planar)||(port_params.format.video.eColorFormat ==(OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar) ||(port_params.format.video.eColorFormat ==(OMX_COLOR_FORMATTYPE)OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar))//for backward compatibility
        {
            t_cm_error cm_error;
            /* if resources are suspended, don't call allocateStabBuffers, but rather exit gracefully,
            as it will be allocated later when receiving unsuspendResources */
            if(resSusp)return OMX_ErrorInsufficientResources;

            cm_error = allocateGrabCache(portHR->mInternalFrameWidth);
            if (cm_error != CM_OK) {
                DBGT_ERROR("Unable to allocate grab cache for HR, error=%d", cm_error);
                OstTraceFiltStatic1(TRACE_ERROR, "Unable to allocate grab cache for HR, error=%d", (&mENSComponent), cm_error);
                DBC_ASSERT(0);
                return OMX_ErrorInsufficientResources;
            }
        }
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE CGrabControl::allocateStabBuffers(camport *port, t_uint32& proj_v_buf_phys, t_sint32 *&proj_v_buf_log, t_uint32& proj_h_buf_phys, t_sint32 *&proj_h_buf_log,t_sint32 rotation)
{
	/* TODO: proper error handling/rollback if any step fails */
    OMX_ERRORTYPE error=OMX_ErrorNone;
    OMX_U32 domain_id;

    if (stab_buf_allocated) {
        error = freeStabBuffers();
        if (OMX_ErrorNone != error) {
            DBGT_ERROR("Stab was already enabled, and couldn't reallocate buffers: omx error %d\n", error);
            OstTraceFiltStatic1(TRACE_ERROR, "Stab was already enabled, and couldn't reallocate buffers: omx error %d", (&mENSComponent), error);
            return error;
        }
    }

    OMX_PARAM_PORTDEFINITIONTYPE port_params = port->getParamPortDefinition();
    int xsize, ysize;
    getOverscanResolution(port->mInternalFrameWidth,
                          port->mInternalFrameHeight,
                          gconvertFormatForFW(port_params.format.video.eColorFormat, false),
                          1.0,
                          xsize, ysize,port_params.nPortIndex);
	if((port_params.nPortIndex == CAMERA_PORT_OUT0) && (xsize > XGA_WIDTH) && IsStabEnabled())
	{
		rectifyOverscanResolution(port->mInternalFrameWidth,port->mInternalFrameHeight, xsize, ysize);
    }

  // error = CM_GetDefaultDomain(SIA_CORE_ID, &domain_id);
    if(rotation == 0 || rotation == 360)
        error = allocateGrabCache(xsize);
    else
        error = allocateGrabCache(ysize);

	if (error != OMX_ErrorNone) {
		DBGT_ERROR("Unable to allocate grab cache for stab, error=%d", error);
		OstTraceFiltStatic1(TRACE_ERROR, "Unable to allocate grab cache for stab, error=%d", (&mENSComponent), error);
	}

#ifdef HCURVE_IN_ESRAM
//for HCurve buffer in ESRAM start

	/* TODO: we don't really care which domain it belongs to, as long as its in ESRAM */
	domain_id = mENSComponent.getNMFDomainHandle(RM_NMFD_HWPIPESIA);

    if(rotation == 0 || rotation == 360)
	    error = ENS::allocMpcMemory(domain_id, CM_MM_MPC_ESRAM16, xsize*2*sizeof(OMX_U32)/2, CM_MM_ALIGN_4WORDS, &stab_buf_h);
    else
	    error = ENS::allocMpcMemory(domain_id, CM_MM_MPC_ESRAM16, ysize*2*sizeof(OMX_U32)/2, CM_MM_ALIGN_4WORDS, &stab_buf_h);

    if (error != OMX_ErrorNone) {
        DBGT_ERROR("Unable to allocate ESRAM H buffer for stab (CM err %d)\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Unable to allocate ESRAM H buffer for stab (CM err %d)", (&mENSComponent), error);
        return OMX_ErrorInsufficientResources;
    }

//for HCurve buffer in ESRAM end
#endif

	MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
	poolAttrs.iBuffers = 1;
	poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;      // Memory type
	poolAttrs.iDomainID = 0;                                // MPC Domain ID (only requested for MPC memory type)

	if(rotation == 0 || rotation == 360)
		poolAttrs.iSize = ysize*2*sizeof(OMX_U32);                           // Size (in byte) of a buffer
	else
		poolAttrs.iSize = xsize*2*sizeof(OMX_U32);

	poolAttrs.iAlignment = 256;                             // Alignment applied to the base address of each buffer in the pool													                                     //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
	poolAttrs.iCacheAttr = MMHwBuffer::ENormalUnCached;
	stab_buf_v = NULL;
	error = MMHwBuffer::Create(poolAttrs, NULL, stab_buf_v);
	if (error != OMX_ErrorNone) {
        DBGT_ERROR("Unable to allocate SDRAM V buffer for stab (OMX err %d)\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Unable to allocate SDRAM V buffer for stab (OMX err %d)", (&mENSComponent), error);
        return OMX_ErrorInsufficientResources;
	}

	/* retrieve physical and logical addresses to pass up */
	MMHwBuffer::TBufferInfo bufferInfo;
	error = stab_buf_v->BufferInfo(0, bufferInfo);
	if (error != OMX_ErrorNone) {
		DBC_ASSERT(0);
	}
	proj_v_buf_phys=bufferInfo.iPhyAddr;
	proj_v_buf_log=(t_sint32 *)bufferInfo.iLogAddr;

#ifdef HCURVE_IN_ESRAM
//for HCurve buffer in ESRAM start, right now commented out
    t_cm_error cm_error;
	t_cm_system_address system_address;
	cm_error = CM_GetMpcMemorySystemAddress(stab_buf_h, &system_address);
	DBC_ASSERT(cm_error == CM_OK);
	proj_h_buf_phys=system_address.physical;
	proj_h_buf_log=(t_sint32 *)system_address.logical;
//for HCurve buffer in ESRAM end, right now commented out
#else
//for HCurve buffer in DDR start
	//MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
	poolAttrs.iBuffers = 1;
	poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;      // Memory type
	poolAttrs.iDomainID = 0;                                // MPC Domain ID (only requested for MPC memory type)

	if(rotation == 0 || rotation == 360)
		poolAttrs.iSize = xsize*2*sizeof(OMX_U32);// 2                           // Size (in byte) of a buffer
	else
		poolAttrs.iSize = ysize*2*sizeof(OMX_U32);

	poolAttrs.iAlignment = 256;                             // Alignment applied to the base address of each buffer in the pool
															//   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
	poolAttrs.iCacheAttr = MMHwBuffer::ENormalUnCached;
	stab_buf_h = NULL;
	error = MMHwBuffer::Create(poolAttrs, NULL, stab_buf_h);
	if (error != OMX_ErrorNone) {
        MSG1("Unable to allocate SDRAM V buffer for stab (OMX err %d)\n", error);
        OstTraceFiltStatic1(TRACE_ERROR, "Unable to allocate SDRAM V buffer for stab (OMX err %d)", (&mENSComponent), error);
        return OMX_ErrorInsufficientResources;
	}

	/* retrieve physical and logical addresses to pass up */
	//MMHwBuffer::TBufferInfo bufferInfo;
	error = stab_buf_h->BufferInfo(0, bufferInfo);
	if (error != OMX_ErrorNone) {
		DBC_ASSERT(0);
	}

	proj_h_buf_phys=bufferInfo.iPhyAddr;
	proj_h_buf_log=(t_sint32 *)bufferInfo.iLogAddr;
	//for HCurve buffer in DDR end
#endif

    stab_buf_allocated = true;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE CGrabControl::freeStabBuffers()
{
    if (!stab_buf_allocated) {
        return OMX_ErrorNone;
    }

    //t_cm_error cm_error;
    OMX_ERRORTYPE omx_error=OMX_ErrorNone;

#ifdef HCURVE_IN_ESRAM
//for HCurve buffer in ESRAM start, right now commented out
    omx_error = ENS::freeMpcMemory(stab_buf_h);
//for HCurve buffer in ESRAM end, right now commented out
#else
//for HCurve buffer in DDR start
    omx_error = MMHwBuffer::Destroy(stab_buf_h);
//for HCurve buffer in DDR end
#endif
    if (omx_error != OMX_ErrorNone) {
        DBGT_ERROR("Problem freeing Stab H ESRAM buffer (err=%d)\n", omx_error);
        OstTraceFiltStatic1(TRACE_ERROR, "Problem freeing Stab H ESRAM buffer (err=%d)", (&mENSComponent), omx_error);
		DBC_ASSERT(0);
    }
    //omx_error = HwBuffer::FreeBuffers(stab_buf_v);
    omx_error = MMHwBuffer::Destroy(stab_buf_v);
	if (omx_error != OMX_ErrorNone) {
        DBGT_ERROR("Problem freeing Stab V SDRAM buffer (err=%d)\n", omx_error);
        OstTraceFiltStatic1(TRACE_ERROR, "Problem freeing Stab V SDRAM buffer (err=%d)", (&mENSComponent), omx_error);
		DBC_ASSERT(0);
    }
    stab_buf_allocated = false;

    return OMX_ErrorNone;
}

void CGrabControl::getOverscanResolution(int xsize, int ysize, e_grabFormat grabformat, float zoomfactor, int &xoversize, int &yoversize, t_uint16 port_idx)
{

    MSG1("CGrabControl::getOverscanResolution For Port : %d\n", port_idx);
    if ((!mStabEnabled) || (port_idx == CAMERA_PORT_OUT1)) {
        xoversize = xsize;
        yoversize = ysize;
        MSG2("CGrabControl::getOverscanResolution xoversize : %d and yoversize: %d \n", xoversize,yoversize);
        return;
    }
    int nom, den;
    /* zoomfactor should influence nominator and denominator someday, currently fixed until adaptive overscan
    is imlpemented in all SW layers */
    nom = CAM_OVERSCAN_NOM;
    den = CAM_OVERSCAN_DEN;
    /* size = (size * 130%) rounded down to 16 (MB size) */
    xoversize = ((((xsize*nom)/den)+15)&(~0xF)) + 16; /*Correction for aspect ratio --should not be less than actual X/Y */
    yoversize = (((ysize*nom)/den)+15)&(~0xF);

    MSG2("CGrabControl::getOverscanResolution xoversize : %d and yoversize: %d \n", xoversize,yoversize);

}

void CGrabControl::rectifyOverscanResolution(int xsize, int ysize,int &xoversize, int &yoversize)
{

	MSG1("CGrabControl::rectifyOverscanResolution \n");
	xoversize = XGA_WIDTH;
	yoversize = (( (ysize*xoversize)/xsize)&(~0xF)); /*Need to maintain the original aspect ratio*/
	MSG2("CGrabControl::rectifyOverscanResolution xoversize : %d and yoversize: %d \n", xoversize,yoversize);
}


OMX_BOOL CGrabControl::IsStabEnabled()
{
    if ((true == mStabEnabled) && (OMX_FALSE == pOMXCam->mSensorMode.bOneShot)) {
        return OMX_TRUE;
    }
    else {
        return OMX_FALSE;
    }
}

#define STRINGIFY(mode) case mode: return #mode
const char* CGrabControl::stringfyGrabStatus(const uint32_t camport)
{
    IN0();
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CGrabControl::stringfyGrabStatus", (&mENSComponent));

    switch(grabPortsState[camport]) {
        STRINGIFY(GRAB_STATE_UNCONFIGURED);
        STRINGIFY(GRAB_STATE_ENDCAPTURE);
        STRINGIFY(GRAB_STATE_ENDCAPTURENOW);
        STRINGIFY(GRAB_STATE_STARTCAPTURE);
        STRINGIFY(GRAB_STATE_STARTONESHOTCAPTURE);
        STRINGIFY(GRAB_STATE_STARTBURSTCAPTURE);
        STRINGIFY(GRAB_STATE_STARTHIDDENCAPTURE);
        STRINGIFY(GRAB_STATE_INVALID);
        default: return "Invalide grab mode";
    }
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CGrabControl::stringfyGrabStatus", (&mENSComponent));
    OUT0();
}

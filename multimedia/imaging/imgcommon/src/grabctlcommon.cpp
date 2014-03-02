/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "grabctlcommon.h"
//#include <cm/inc/cm_macros.h>
#include "osi_trace.h"

//#include "ENS_Nmf.h"

#include "IFM_Types.h"
/* for traces */
#define OMXCOMPONENT "GRABCTLCOMMON"
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_grabctlcommonTraces.h"
#endif

t_cm_memory_handle CGrabControlCommon::mGrabCacheHandle = 0;

CGrabControlCommon::~CGrabControlCommon()
{
    OMX_ERRORTYPE error = freeGrabCache();
    if (error != OMX_ErrorNone) {
           MSG1("in CGrabControlCommon desctructor: couldn't free Grab Cache, error %d\n", error);
           OstTraceFiltStatic1(TRACE_DEBUG, "in CGrabControlCommon desctructor: couldn't free Grab Cache, error %d", (&mENSComponent), error);
    }
}


/*
 * This function allows to configure the MEMGRABCTL
 * Note that windowing is not handled for VPB1
 */
void CGrabControlCommon::configureMemGrabResolutionFormat( ifmport*         port,
                                                           t_uint32         embeddedLinesNb,
                                                           t_uint32         lineLength,
                                                           e_BMLPixelOrder  pixelOrder,
														   t_uint32			stripeCount,
														   t_uint32			stripeNumber,
														   t_uint32			stripeSize)
{

	t_grab_desc grabDesc;
    memset((void*)&grabDesc,0,sizeof(grabDesc));

/* Whatever the port is enabled or not, there is a default configuration */
	grabDesc.port_idx=port->getPortIndex();
	grabDesc.xframeSize= port->getParamPortDefinition().format.video.nFrameWidth;
	grabDesc.yframeSize=port->getParamPortDefinition().format.video.nFrameHeight;
	grabDesc.yframeSize -= embeddedLinesNb;

    //Changes for stripe
    if(stripeCount > 1)
        {
        switch (stripeNumber)
            {
            case 0:
				if((stripeSize%8) == 0)
					{
					grabDesc.xwindowSize = stripeSize;
					}
				else
					{
					grabDesc.xwindowSize = stripeSize + (8-(stripeSize%8));//1664; //grabDesc.xframeSize/2;
					}
                grabDesc.ywindowSize = grabDesc.yframeSize;
                grabDesc.xwindowOffset = 0;
                grabDesc.ywindowOffset = 0;
                break;

            case 1:
                if((stripeSize%8) == 0)
					{
					grabDesc.xwindowSize = stripeSize;
					}
				else
					{
					grabDesc.xwindowSize = stripeSize + (8-(stripeSize%8));//1664; //grabDesc.xframeSize/2;
					}
                grabDesc.ywindowSize = grabDesc.yframeSize;
                grabDesc.xwindowOffset = grabDesc.xframeSize - grabDesc.xwindowSize;
                grabDesc.ywindowOffset = 0;
                break;
            }
        }
    else
        {
        grabDesc.xwindowSize = grabDesc.xframeSize;
        grabDesc.ywindowSize = grabDesc.yframeSize;
        grabDesc.xwindowOffset = 0;
        grabDesc.ywindowOffset = 0;
        }

	/* TODO: It's dependent from the Sensor !!!!*/

	grabDesc.bmlDesc.nb_linetypes=5;
	grabDesc.bmlDesc.line_type[0]=BML_LINETYPE_STARTOFFRAME;
	grabDesc.bmlDesc.line_type_nb[0]=1;
	grabDesc.bmlDesc.line_type[1]=BML_LINETYPE_BLANK;
	grabDesc.bmlDesc.line_type_nb[1]=embeddedLinesNb-1;
	grabDesc.bmlDesc.line_type[2]=BML_LINETYPE_ACTIVE;
	grabDesc.bmlDesc.line_type_nb[2]=grabDesc.yframeSize-1;
	grabDesc.bmlDesc.line_type[3]=BML_LINETYPE_ACTIVELAST;
	grabDesc.bmlDesc.line_type_nb[3]=1;
	grabDesc.bmlDesc.line_type[4]=BML_LINETYPE_BLANK;
	grabDesc.bmlDesc.line_type_nb[4]=32;
	if(lineLength>grabDesc.xframeSize)
		grabDesc.bmlDesc.interline_size=lineLength-grabDesc.xframeSize;
	else
		grabDesc.bmlDesc.interline_size=0;

	grabDesc.bmlDesc.line_type_nb[5]=0;
	grabDesc.bmlDesc.line_type_nb[6]=0;
	grabDesc.bmlDesc.line_type_nb[7]=0;

	grabDesc.bmlDesc.pixel_order=pixelOrder;
	grabDesc.bmlDesc.nb_embedded_lines=embeddedLinesNb;


	grabDesc.grabFormat=port->getParamPortDefinition().format.video.eColorFormat;
	grabDesc.destBufferAddr =0;
	grabDesc.disableGrabCache=true;
	grabDesc.cacheBufferAddr=0;
	grabDesc.bufferCount = port->getParamPortDefinition().nBufferCountActual;
	grabDesc.rotation = 0;

	configure(&grabDesc, true);

}

/*
 * This function allows to configure the GRABCTL
 * added handling of windowing
 */
void CGrabControlCommon::configureGrabResolutionFormat(ifmport *port, t_uint8 keepRatio, t_sint32 rotation,
                                                        t_uint32 stripeCount, t_uint32 stripeNumber)
{

    t_grab_desc grabDesc;
    memset((void*)&grabDesc,0,sizeof(grabDesc));

    /* Whatever the port is enabled or not, there is a default configuration */
    grabDesc.port_idx=port->getPortIndex();
    grabDesc.yframeSize=port->getParamPortDefinition().format.video.nSliceHeight;
    grabDesc.grabFormat=port->getParamPortDefinition().format.video.eColorFormat;
    grabDesc.xframeSize=port->getParamPortDefinition().format.video.nFrameWidth;
    if(port->bIsOverScanned)
    {
        grabDesc.xframeSize=port->mOverScannedWidth;
    }
    grabDesc.bufferCount = port->getParamPortDefinition().nBufferCountActual;
    grabDesc.destBufferAddr =0;
    grabDesc.rotation = rotation;
    grabDesc.keepRatio = keepRatio;

    //Changes for Stripe
    if(stripeCount > 1)
        {
        switch (stripeNumber)
            {
            case 0:
                grabDesc.xwindowSize = grabDesc.xframeSize/2;
                grabDesc.ywindowSize = grabDesc.yframeSize;
                grabDesc.xwindowOffset = 0;
                grabDesc.ywindowOffset = 0;
                break;

            case 1:
                grabDesc.xwindowSize = grabDesc.xframeSize/2;
                grabDesc.ywindowSize = grabDesc.yframeSize;
                grabDesc.xwindowOffset = grabDesc.xframeSize/2;
                grabDesc.ywindowOffset = 0;
                break;
            }
        }
    else
        {
          grabDesc.xwindowSize=port->mInternalFrameWidth;
          grabDesc.ywindowSize=port->mInternalFrameHeight;
	       	grabDesc.xwindowOffset = 0;
        	grabDesc.ywindowOffset = 0;
        }

    grabDesc.bmlDesc.nb_linetypes=0;
    grabDesc.bmlDesc.interline_size = 0;
    grabDesc.bmlDesc.nb_embedded_lines = 0;

    /* TODO: stab use case where HR isn't macroblock? (stab still needs grab cache) */
    if ((CGrabControlCommon::mGrabCacheHandle) && ((grabDesc.grabFormat == OMX_COLOR_FormatYUV420Planar)||(grabDesc.grabFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar) ||(grabDesc.grabFormat == (OMX_COLOR_FORMATTYPE)OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar)))//for backward compatibility reason
	{
		grabDesc.disableGrabCache = false;
		grabDesc.cacheBufferAddr = mCacheAddrPhys;
    } else {
    	grabDesc.disableGrabCache=true;
		grabDesc.cacheBufferAddr=0;
    }
    configure(&grabDesc, false);
}

/*
 * allocate grab Cache. Though usually called from configureGrabResolutionFormat() above,
 * this function can also be called from HsmCamera CGrabControl::activateStab() for LR
 */
OMX_ERRORTYPE CGrabControlCommon::allocateGrabCache(t_uint16 xsize)
{
    OMX_ERRORTYPE error=OMX_ErrorNone;
    t_cm_domain_id domain_id;
    t_cm_error cmError;

    /* fre previous buffer if it existed */
    error = freeGrabCache();
    if (error != OMX_ErrorNone) return error;

    domain_id = mENSComponent.getNMFDomainHandle(RM_NMFD_HWPIPESIA);
    cmError=CM_AllocMpcMemory(domain_id, CM_MM_MPC_ESRAM16, xsize*3*16/sizeof(t_uint16), CM_MM_ALIGN_4WORDS, &CGrabControlCommon::mGrabCacheHandle);
    if (cmError != CM_OK) {
        MSG1("Couldn't allocate eSRAM for grab cache. Disabling grab cache, filtered format, stab! (err %d)\n", cmError);
        OstTraceFiltStatic1(TRACE_DEBUG, "Couldn't allocate eSRAM for grab cache. Disabling grab cache, filtered format, stab! (err %d)", (&mENSComponent), cmError);
        return OMX_ErrorHardware;
    } else {
        t_cm_system_address system_address;
        t_cm_error cm_error = 0;
        cm_error = CM_GetMpcMemorySystemAddress(CGrabControlCommon::mGrabCacheHandle, &system_address);
        if (cm_error != CM_OK) {
            MSG1("Couldn't get cache system address! (err %X)\n", cm_error);
            OstTraceFiltStatic1(TRACE_DEBUG, "Couldn't get cache system address! (err %X)", (&mENSComponent), cm_error);
            cm_error = CM_FreeMpcMemory(CGrabControlCommon::mGrabCacheHandle);
            if (cm_error != CM_OK) {
                MSG1("Couldn't free grab cache memory! (err %X)\n", cm_error);
                OstTraceFiltStatic1(TRACE_DEBUG, "Couldn't free grab cache memory! (err %X)", (&mENSComponent), cm_error);
                return OMX_ErrorHardware;
            } else {
                CGrabControlCommon::mGrabCacheHandle = 0;
            }
            return OMX_ErrorNone;
        }
        mCacheAddrPhys = system_address.physical;
    }
    return error;
}

OMX_ERRORTYPE CGrabControlCommon::freeGrabCache()
{
    t_cm_error cmError;
    if (CGrabControlCommon::mGrabCacheHandle) {
        cmError = CM_FreeMpcMemory(CGrabControlCommon::mGrabCacheHandle);
        if (cmError != OMX_ErrorNone) {
            MSG1("Couldn't free grab cache memory prior to reallocating it! (err %d)\n", cmError);
            OstTraceFiltStatic1(TRACE_DEBUG, "Couldn't free grab cache memory prior to reallocating it! (err %d)", (&mENSComponent), cmError);
            return OMX_ErrorHardware;
        }
        CGrabControlCommon::mGrabCacheHandle = 0;
    }
    return OMX_ErrorNone;
}

/*
 * This function will apply the configuration to the below layers
 */
void CGrabControlCommon::configure(t_grab_desc * pGrabDesc, t_bool isBML)
{
	IN0("\n");
	struct s_grabParams grabparams;

    grabparams.output_format=gconvertFormatForFW(pGrabDesc->grabFormat, isBML);
    //FIXME: windowing not yet managed
    grabparams.x_window_size=pGrabDesc->xwindowSize;
    grabparams.y_window_size=pGrabDesc->ywindowSize;
    grabparams.x_window_offset=pGrabDesc->xwindowOffset;
    grabparams.y_window_offset=pGrabDesc->ywindowOffset;
    grabparams.x_frame_size=pGrabDesc->xframeSize;
    grabparams.y_frame_size=pGrabDesc->yframeSize;
    grabparams.dest_buf_addr=pGrabDesc->destBufferAddr;
    grabparams.buf_id=0; /* filled by grabctl NMF component */
    grabparams.enable_stab=0;
    grabparams.x_stab_size_lr=0; /* filled by grabctl NMF component */
    grabparams.y_stab_size_lr=0; /* filled by grabctl NMF component */
    grabparams.proj_v_buf_addr=0;
    grabparams.proj_h_buf_addr=0;
    grabparams.disable_grab_cache=pGrabDesc->disableGrabCache;
    grabparams.cache_buf_addr=pGrabDesc->cacheBufferAddr;
    grabparams.lr_alpha = 0;
    grabparams.rotation_cfg = gconvertRotationForFW(pGrabDesc->rotation);
    grabparams.keepRatio = pGrabDesc->keepRatio;
    configureHook(grabparams, pGrabDesc->port_idx);

    grabparams.bml_params.pixel_order=pGrabDesc->bmlDesc.pixel_order;
    grabparams.bml_params.nb_linetypes=pGrabDesc->bmlDesc.nb_linetypes;
    for(int i=0;i<8;i++) {
    	grabparams.bml_params.line_type[i]=pGrabDesc->bmlDesc.line_type[i];
    	grabparams.bml_params.line_type_nb[i]=pGrabDesc->bmlDesc.line_type_nb[i];
    }
    grabparams.bml_params.interline_size = pGrabDesc->bmlDesc.interline_size;
    grabparams.bml_params.nb_embedded_lines = pGrabDesc->bmlDesc.nb_embedded_lines;

    MSG1("\t Configure Grabctl for port %ld\n", pGrabDesc->port_idx);
    OstTraceFiltStatic1(TRACE_DEBUG, "\t Configure Grabctl for port %ld", (&mENSComponent), pGrabDesc->port_idx);
    MSG2("grabparams.output_format = 0x%x (%s)\n", grabparams.output_format, grabFormatToString(grabparams.output_format));
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.output_format = 0x%x", (&mENSComponent), grabparams.output_format);
    MSG1("grabparams.x_window_size = 0x%x\n", grabparams.x_window_size);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.x_window_size = 0x%x", (&mENSComponent), grabparams.x_window_size);
    MSG1("grabparams.y_window_size = 0x%x\n", grabparams.y_window_size);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.y_window_size = 0x%x", (&mENSComponent), grabparams.y_window_size);
    MSG1("grabparams.x_window_offset =0x%x\n", grabparams.x_window_offset);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.x_window_offset =0x%x", (&mENSComponent), grabparams.x_window_offset);
    MSG1("grabparams.y_window_offset = 0x%x\n", grabparams.y_window_offset);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.y_window_offset = 0x%x", (&mENSComponent), grabparams.y_window_offset);
    MSG1("grabparams.x_frame_size = 0x%x\n", grabparams.x_frame_size);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.x_frame_size = 0x%x", (&mENSComponent), grabparams.x_frame_size);
    MSG1("grabparams.y_frame_size = 0x%x\n", grabparams.y_frame_size);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.y_frame_size = 0x%x", (&mENSComponent), grabparams.y_frame_size);
    // MSG1("grabparams.dest_buf_addr = 0x%x\n", grabparams.dest_buf_addr);
    // OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.dest_buf_addr = 0x%x", (&mENSComponent), grabparams.dest_buf_addr);
    // MSG1("grabparams.buf_id = 0x%x\n", grabparams.buf_id);
    // OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.buf_id = 0x%x", (&mENSComponent), grabparams.buf_id);
    MSG1("grabparams.disable_grab_cache = 0x%x\n", grabparams.disable_grab_cache);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.disable_grab_cache = 0x%x", (&mENSComponent), grabparams.disable_grab_cache);
    MSG1("grabparams.cache_buf_addr = 0x%lx\n", grabparams.cache_buf_addr);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.cache_buf_addr = 0x%lx", (&mENSComponent), grabparams.cache_buf_addr);
    MSG1("grabparams.rotation_cfg = 0x%x\n",grabparams.rotation_cfg);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.rotation_cfg = 0x%x", (&mENSComponent),grabparams.rotation_cfg);
    MSG1("grabparams.enable_stab = 0x%x\n", grabparams.enable_stab);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.enable_stab = 0x%x", (&mENSComponent), grabparams.enable_stab);
    MSG1("grabparams.x_stab_size_lr = 0x%x\n", grabparams.x_stab_size_lr);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.x_stab_size_lr = 0x%x", (&mENSComponent), grabparams.x_stab_size_lr);
    MSG1("grabparams.y_stab_size_lr = 0x%x\n", grabparams.y_stab_size_lr);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.y_stab_size_lr = 0x%x", (&mENSComponent), grabparams.y_stab_size_lr);
    MSG1("grabparams.proj_v_buf_addr = 0x%lx\n", grabparams.proj_v_buf_addr);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.proj_v_buf_addr = 0x%lx", (&mENSComponent), grabparams.proj_v_buf_addr);
    MSG1("grabparams.proj_h_buf_addr = 0x%lx\n", grabparams.proj_h_buf_addr);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.proj_h_buf_addr = 0x%lx", (&mENSComponent), grabparams.proj_h_buf_addr);
    MSG1("grabparams.bml_params.nb_linetypes= 0x%x\n", grabparams.bml_params.nb_linetypes);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.bml_params.nb_linetypes= 0x%x", (&mENSComponent), grabparams.bml_params.nb_linetypes);
    MSG1("grabparams.bml_params.nb_embedded_lines= 0x%x\n", grabparams.bml_params.nb_embedded_lines);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.bml_params.nb_embedded_lines= 0x%x", (&mENSComponent), grabparams.bml_params.nb_embedded_lines);
    MSG1("grabparams.bml_params.interline_size= 0x%lx\n", grabparams.bml_params.interline_size);
    OstTraceFiltStatic1(TRACE_DEBUG, "grabparams.bml_params.interline_size= 0x%lx", (&mENSComponent), grabparams.bml_params.interline_size);

    /* TODO : What a UGLY name. Change it as doConfigure ASAP */
    /* This method is a virtual that has to be overwritten by the ones which inherits from this class*/
    callInterface(pGrabDesc, grabparams);

	OUTR(" ",(0));
}
/*
 * This function is a helper to convert the OMX index into a "grab understandable" rotate.
 */

t_uint16 CGrabControlCommon::gconvertRotationForFW(t_sint32 rotation)
{
	t_uint16 rot=0;

	MSG1("Rotation is %ld\n",rotation);
	OstTraceFiltStatic1(TRACE_DEBUG, "Rotation is %ld", (&mENSComponent),rotation);
	switch (rotation)
	{
		case 0:
		case 360:
		rot=HW_ROTATION_NONE;
		break;

		case 90:
		case -270:
		rot=HW_ROTATION_CW;	//CW
		break;

		case 270:
		case -90:
		rot=HW_ROTATION_CCW;	//CCW
		break;

	      	default :
        	DBC_ASSERT(0); /* Assert in case the rotation is not supported or it has been corrupted*/
		break;
	}
	return rot;
}

/*
 * This function is a helper to convert the OMX index into a "grab understandable" format.
 */
e_grabFormat CGrabControlCommon::gconvertFormatForFW(OMX_COLOR_FORMATTYPE omxformat, t_bool isBML)
{
	MSG1("Grab format is x%x\n",omxformat);
	OstTraceFiltStatic1(TRACE_DEBUG, "Grab format is x%x", (&mENSComponent),omxformat);
    switch ((t_uint32)omxformat )
    {
        case OMX_COLOR_Format16bitRGB565 :
            return GRBFMT_R5G6B5;
        case OMX_COLOR_FormatYUV420Planar : // for backward compatibility
		case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
            return GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED;
		case OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar:
            return GRBFMT_YUV420_MB_SWAP_SEMIPLANAR_DECIMATED;
		case OMX_COLOR_FormatCbYCrY:
			return GRBFMT_YUV422_RASTER_INTERLEAVED;
		case OMX_COLOR_Format16bitARGB4444:
			return GRBFMT_A4R4G4B4;
		case OMX_COLOR_Format16bitARGB1555:
			return GRBFMT_A1R5G5B5;
		case OMX_COLOR_Format24bitRGB888:
			return GRBFMT_R8G8B8;
		case OMX_COLOR_Format32bitARGB8888:
			return GRBFMT_A8R8G8B8;
		case OMX_COLOR_FormatRawBayer8bit:
		case OMX_COLOR_FormatRawBayer8bitcompressed:
			if (true == isBML)
			{
				// BML
				return GRBFMT_FAST_RAW8;
			}
			else
			{
				return GRBFMT_RAW8;
			}
		case OMX_COLOR_FormatRawBayer10bit: /*backward compât*/
		case OMX_SYMBIAN_COLOR_FormatRawBayer12bit:
			if (true == isBML)
			{
				// BML
				return GRBFMT_FAST_RAW12;
			}
			else
			{
				return GRBFMT_RAW12;
			}
		case OMX_COLOR_FormatYUV420PackedPlanar:
		    return GRBFMT_YUV420_RASTER_PLANAR_I420;
        case OMX_STE_COLOR_FormatYUV420PackedSemiPlanar_NV21:
            return GRBFMT_YUV420_RASTER_SEMIPLANAR_NV21;
        case OMX_STE_COLOR_FormatYUV420PackedSemiPlanar_NV12:
            return GRBFMT_YUV420_RASTER_SEMIPLANAR_NV12;
        default :
        	DBC_ASSERT(0); /* Assert in case the format is not supported or it has been corrupted*/
            return (e_grabFormat)-1;
        }
}

const char * CGrabControlCommon::grabFormatToString(e_grabFormat grab_format)
{
    switch (grab_format)
    {
        case GRBFMT_YUV422_RASTER_INTERLEAVED: return "YUV422_RASTER_INTERLEAVED";
        case GRBFMT_YUV420_RASTER_PLANAR_I420: return "YUV420_PASTER_PLANAR_I420";
        case GRBFMT_YUV420_RASTER_PLANAR_YV12: return "YUV420_RASTER_PLANAR_YV12";
        case GRBFMT_YUV422_MB_SEMIPLANAR: return "YUV422_MB_SEMIPLANAR";
        case GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED: return "YUV420_MB_SEMIPLANAR_FILTERED";
        case GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED: return "YUV420_MB_SEMIPLANAR_DECIMATED";
        case GRBFMT_YUV420_MB_SWAP_SEMIPLANAR_DECIMATED: return "YUV420_MB_SWAP_SEMIPLANAR_DECIMATED";
        case GRBFMT_YUV420_RASTER_SEMIPLANAR_NV21: return "YUV420_RASTER_SEMIPLANAR_NV21";
        case GRBFMT_YUV420_RASTER_SEMIPLANAR_NV12: return "YUV420_RASTER_SEMIPLANAR_NV12";
        case GRBFMT_RGB30: return "RGB30";
        case GRBFMT_RAW8: return "RAW8";
        case GRBFMT_FAST_RAW8: return "FAST_RAW8";
        case GRBFMT_BML2_RAW8: return "BML2_RAW8";
        case GRBFMT_RAW12: return "RAW12";
        case GRBFMT_FAST_RAW12: return "FAST_RAW12";
        case GRBFMT_BML2_RAW12: return "BML2_RAW12";
        case GRBFMT_A4R4G4B4: return "ARGB4444";
        case GRBFMT_A1R5G5B5: return "ARGB1555";
        case GRBFMT_R5G6B5: return "RGB565";
        case GRBFMT_R8G8B8: return "RGB888";
        case GRBFMT_A8R8G8B8: return "ARGB8888";
        default: break;
    }
    return "Unknown grab format";
}

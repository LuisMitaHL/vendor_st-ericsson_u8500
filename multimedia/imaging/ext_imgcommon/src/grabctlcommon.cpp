/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_grabctlcommon.h"
#include "ENS_Nmf.h"
#include "IFM_Types.h"

#define DBGT_DECLARE_AUTOVAR
#undef   LOG_TAG
#define  LOG_TAG DBGT_TAG
#undef DBGT_LAYER
#define DBGT_LAYER 3
#undef DBGT_PREFIX
#define DBGT_PREFIX "GRBC"
#include "debug_trace.h"

/* for traces */
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8

#undef DBGT_LAYER
#define DBGT_LAYER 3


CGrabControlCommon::CGrabControlCommon(ENS_Component &enscomp):
    mGrabCacheHandle(0),
    mCacheAddrPhys(0),
    mENSComponent(enscomp)
{
    DBGT_TRACE_INIT(extcamera);
}

CGrabControlCommon::~CGrabControlCommon()
{
    if (mGrabCacheHandle) {
        t_cm_error cm_error = CM_FreeMpcMemory(mGrabCacheHandle);
        if (cm_error != CM_OK) {
            DBC_ASSERT(0);
            DBGT_PTRACE("Couldn't free grab cache memory prior to reallocating it! (err %d)", cm_error);
        }
        mGrabCacheHandle = 0;
    }
}

/*
 * This function allows to configure the GRABCTL
 */
void CGrabControlCommon::configureGrabResolutionFormat(ifmport *port)
{

    t_grab_desc grabDesc;
    grabDesc.port_idx=port->getPortIndex();

    /* Whatever the port is enabled or not, there is a default configuration */
    switch(grabDesc.port_idx)
    {
        case 1:
        {
            grabDesc.grabFormat             = port->getParamPortDefinition().format.image.eColorFormat;
            grabDesc.imageCompressionFormat = port->getParamPortDefinition().format.image.eCompressionFormat;
            break;
        }
        case 0:
        default:
        {
            grabDesc.grabFormat             = port->getParamPortDefinition().format.video.eColorFormat;
            grabDesc.imageCompressionFormat = OMX_IMAGE_CodingUnused; // unused fro port 0 and 2
            break;
        }
    }

    grabDesc.xframeSize    = port->mInternalFrameWidth;
    grabDesc.yframeSize    = port->mInternalFrameHeight;

    // frame size must aling top 16 due to encoder hw constraint
    if(grabDesc.xframeSize % 16 != 0){
        grabDesc.xframeSize  = (((grabDesc.xframeSize >> 4)+1)<<4);
    }
    if(grabDesc.yframeSize % 16 != 0){
        grabDesc.yframeSize  = (((grabDesc.yframeSize >> 4)+1)<<4);
    }

    grabDesc.xwindowSize   = port->mInternalFrameWidth;
    grabDesc.ywindowSize   = port->mInternalFrameHeight;
    grabDesc.xwindowOffset = 0;
    grabDesc.ywindowOffset = 0;

    grabDesc.bufferSize = port->getParamPortDefinition().nBufferSize;
    grabDesc.bufferCount = port->getParamPortDefinition().nBufferCountActual;
    grabDesc.destBufferAddr =0;

    if((grabDesc.grabFormat == OMX_COLOR_FormatYUV420Planar)||(grabDesc.grabFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)) // only in case of mb format
    {
        t_cm_error cm_error = 0;
        t_cm_domain_id domain_id;

        if (mGrabCacheHandle) {
            cm_error = CM_FreeMpcMemory(mGrabCacheHandle);
            if (cm_error != CM_OK) {
                DBGT_CRITICAL("Couldn't free grab cache memory prior to reallocating it! (err %X)", cm_error);
            }
            mGrabCacheHandle = 0;
        }
        domain_id = mENSComponent.getNMFDomainHandle(RM_NMFD_HWPIPESIA);
        // reserve 16 lines of yuv420MB > width line * 1.5 * 16
        cm_error = CM_AllocMpcMemory(domain_id, CM_MM_MPC_ESRAM16, grabDesc.xframeSize*3*16/2, CM_MM_ALIGN_4WORDS, &mGrabCacheHandle);
        if (cm_error != CM_OK) {
            DBGT_CRITICAL("Couldn't allocate eSRAM for grab cache. Disabling grab cache, filtered format, stab! (err %x)", cm_error);
            grabDesc.disableGrabCache=true;
            grabDesc.cacheBufferAddr=0;
        } else {
            t_cm_system_address system_address;
            cm_error = CM_GetMpcMemorySystemAddress(mGrabCacheHandle, &system_address);
            if (cm_error != CM_OK) {
                DBGT_CRITICAL("Couldn't get cache system address! (err %X)", cm_error);
            }
            grabDesc.disableGrabCache=false;
            grabDesc.cacheBufferAddr = system_address.physical;
        }
    } else    {
        grabDesc.disableGrabCache=true;
        grabDesc.cacheBufferAddr=0;
    }

    configure(&grabDesc);
}


/*
 * This function will apply the configuration to the below layers
 */
void CGrabControlCommon::configure(t_grab_desc * pGrabDesc)
{
    struct s_grabParams grabparams;

    if( (pGrabDesc->imageCompressionFormat == OMX_IMAGE_CodingJPEG)||
        (pGrabDesc->grabFormat == OMX_STE_COLOR_FormatRawData) ){
        grabparams.output_format  = GRBFMT_JPEG;
        DBGT_PTRACE("grab jpeg");
    } else {
        grabparams.output_format  = gconvertFormatForFW(pGrabDesc->grabFormat);
        DBGT_PTRACE("grab YUV");
    }

    grabparams.buffer_size = pGrabDesc->bufferSize;

    grabparams.x_window_size      = pGrabDesc->xwindowSize;
    grabparams.y_window_size      = pGrabDesc->ywindowSize;
    grabparams.x_window_offset    = pGrabDesc->xwindowOffset;
    grabparams.y_window_offset    = pGrabDesc->ywindowOffset;
    grabparams.x_frame_size       = pGrabDesc->xframeSize;
    grabparams.y_frame_size       = pGrabDesc->yframeSize;
    grabparams.dest_buf_addr      = pGrabDesc->destBufferAddr;
    grabparams.buf_id             = 0;
    grabparams.disable_grab_cache = pGrabDesc->disableGrabCache;
    grabparams.cache_buf_addr     = pGrabDesc->cacheBufferAddr;
    grabparams.enable_stab        = 0;

    configureHook(grabparams, pGrabDesc->port_idx);

    DBGT_PTRACE("\t Configure Grabctl");
    DBGT_PTRACE("grabparams.buffer_size        = %d", (int)grabparams.buffer_size);
    DBGT_PTRACE("grabparams.output_format      = 0x%x", grabparams.output_format);
    DBGT_PTRACE("grabparams.x_window_size      = %d", grabparams.x_window_size);
    DBGT_PTRACE("grabparams.y_window_size      = %d", grabparams.y_window_size);
    DBGT_PTRACE("grabparams.x_window_offset    = %d", grabparams.x_window_offset);
    DBGT_PTRACE("grabparams.y_window_offset    = %d", grabparams.y_window_offset);
    DBGT_PTRACE("grabparams.x_frame_size       = %d", grabparams.x_frame_size);
    DBGT_PTRACE("grabparams.y_frame_size       = %d", grabparams.y_frame_size);
    DBGT_PTRACE("grabparams.disable_grab_cache = 0x%x", grabparams.disable_grab_cache);
    DBGT_PTRACE("grabparams.cache_buf_addr     = 0x%x", (int)grabparams.cache_buf_addr);
    DBGT_PTRACE("grabparams.enable_stab        = 0x%x", grabparams.enable_stab);
    DBGT_PTRACE("grabparams.frameSkip          = %d", grabparams.frameSkip);

    callInterface(pGrabDesc, grabparams);
}

/*
 * This function is a helper to convert the OMX index into a "grab understandable" format.
 */
e_grabFormat CGrabControlCommon::gconvertFormatForFW(OMX_COLOR_FORMATTYPE omxformat)
{
    switch ((uint32_t)omxformat )
    {
        case OMX_COLOR_FormatCbYCrY:
            return GRBFMT_YUV422_RASTER_INTERLEAVED;
        case OMX_COLOR_FormatYUV420Planar:
            return GRBFMT_YUV420_RASTER_PLANAR_YV12;
        case OMX_COLOR_FormatYUV420SemiPlanar:
            return GRBFMT_YUV420_RASTER_SEMI_PLANAR;
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
            return GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED;
        default :
            DBC_ASSERT(0); /* Assert in case the format is not supported or it has been corrupted*/
            return (e_grabFormat)-1;
        }
}

OMX_ERRORTYPE CGrabControlCommon::freeGrabCache()
{
    t_cm_error cm_error;

    if (mGrabCacheHandle) {
        cm_error = CM_FreeMpcMemory(mGrabCacheHandle);
        if (cm_error != CM_OK) {
            DBGT_PTRACE("Couldn't free grab cache memory prior to reallocating it! (err %d)", cm_error);
            return OMX_ErrorHardware;
        }
        mGrabCacheHandle = 0;
    }
    return OMX_ErrorNone;
}

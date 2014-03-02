/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_camport.h"
#include "ext_grabctlcommon.h"
#include "IFM_Types.h"
#include "ext_camera.h"
#include "ext_omxcamera.h"
#include "extradata.h"

#undef   DBGT_LAYER
#define  DBGT_LAYER 1
#undef  DBGT_PREFIX
#define DBGT_PREFIX "PORT"
#include "debug_trace.h"


void camport::setDefaultFormatInPortDefinition(OMX_PARAM_PORTDEFINITIONTYPE * defaultDef)
{
    DBGT_PROLOG("");

    DBGT_PTRACE("PortIndex = %d", (int)defaultDef->nPortIndex);

    t_uint16 mWidth      = 0;
    t_uint16 mHeight     = 0;
    t_uint16 mStride     = 0;
    t_sint32 stride      = 0;
    t_uint32 sliceHeight = 0;
    OMX_U32 bufferSize   = 0;

    switch(defaultDef->nPortIndex)
    {
    case CAMERA_PORT_OUT0:
        {
            /* Video port */
            mParamPortDefinition.format.video.cMIMEType             = NULL;
            mParamPortDefinition.format.video.pNativeRender         = 0 ;// not used
            mParamPortDefinition.format.video.nFrameWidth           = defaultDef->format.video.nFrameWidth;
            mParamPortDefinition.format.video.nFrameHeight          = defaultDef->format.video.nFrameHeight;
            mParamPortDefinition.format.video.nBitrate              = 0;
            mParamPortDefinition.format.video.xFramerate            = defaultDef->format.video.xFramerate;
            mParamPortDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
            mParamPortDefinition.format.video.eCompressionFormat    = (OMX_VIDEO_CODINGTYPE)OMX_VIDEO_CodingUnused;
            mParamPortDefinition.format.video.eColorFormat          = defaultDef->format.video.eColorFormat;
            mParamPortDefinition.format.video.pNativeWindow         = 0;

            mInternalFrameWidth  = mParamPortDefinition.format.video.nFrameWidth;
            mInternalFrameHeight = mParamPortDefinition.format.video.nFrameHeight;

            /*  Get the number of byte per pixel */
            mBytesPerPixel = getPixelDepth(mParamPortDefinition.format.video.eColorFormat);

            /* Check hardware constraints */
            getHwConstraints(defaultDef->nPortIndex, defaultDef->format.video.eColorFormat,&mWidth, &mHeight, &mStride);

            /* Align buffer allocation according to the hardware constraints.
             * If width/height set by client is not compliant to HW
             * constraints, handle those internally. Note that port
             * configuration seen by client (getParameter) is based on
             * the (potentially unaligned) values set by the client ! */
            mInternalFrameWidth  = (mInternalFrameWidth  + (mWidth-1))&~(mWidth-1);
            mInternalFrameHeight = (mInternalFrameHeight + (mHeight-1))&~(mHeight-1);

            /* Buffer size taking into account hardware constraint */
            bufferSize = (OMX_U32) ((double) (mInternalFrameWidth * mInternalFrameHeight) * mBytesPerPixel);

#ifdef STAB
            if ((mParamPortDefinition.format.video.eColorFormat != OMX_COLOR_FormatRawBayer8bit)&&(mParamPortDefinition.format.video.eColorFormat !=OMX_COLOR_FormatRawBayer10bit ))
            {
                bufferSize += sizeof(OMX_DIGITALVIDEOSTABINFOTYPE);
                /* allocate 30% extra to accomodate stab overscanning */
                bufferSize += (bufferSize*7)/9;
            }
#endif

            /* Set stride and sliceHeight */
            stride      = mInternalFrameWidth * mStride;
            sliceHeight = mInternalFrameHeight;

            if (bufferSize == 0) {
                DBGT_ASSERT(0);
            }

            /* Updated param */
            mParamPortDefinition.nBufferSize               = bufferSize;
            mParamPortDefinition.format.video.nStride      = stride;
            mParamPortDefinition.format.video.nSliceHeight = sliceHeight;

            DBGT_PTRACE("mParamPortDefinition.format.video.xFramerate         = %d",
                    (int) mParamPortDefinition.format.video.xFramerate);
            DBGT_PTRACE("mParamPortDefinition.format.video.nFrameWidth        = %d",
                    (int)mParamPortDefinition.format.video.nFrameWidth);
            DBGT_PTRACE("mParamPortDefinition.format.video.nFrameHeight       = %d ",
                    (int)mParamPortDefinition.format.video.nFrameHeight);
            DBGT_PTRACE("mParamPortDefinition.format.video.eCompressionFormat = %d",
                    (int)mParamPortDefinition.format.video.eCompressionFormat);
            DBGT_PTRACE("mParamPortDefinition.format.video.eColorFormat       = %d",
                    (int)mParamPortDefinition.format.video.eColorFormat);
            DBGT_PTRACE("mParamPortDefinition.format.video.nStride            = %d",
                    (int)mParamPortDefinition.format.video.nStride);
            DBGT_PTRACE("mParamPortDefinition.format.video.nSliceHeight       = %d",
                    (int)mParamPortDefinition.format.video.nSliceHeight);
            DBGT_PTRACE("mParamPortDefinition.nBufferSize                     = %d",
                    (int)mParamPortDefinition.nBufferSize);

            break;
        }
    case CAMERA_PORT_OUT1:
        {
            /* Image port */
            mParamPortDefinition.format.image.cMIMEType             = NULL;
            mParamPortDefinition.format.image.pNativeRender         = 0 ;// not used
            mParamPortDefinition.format.image.nFrameWidth           = defaultDef->format.image.nFrameWidth;
            mParamPortDefinition.format.image.nFrameHeight          = defaultDef->format.image.nFrameHeight;
            mParamPortDefinition.format.image.bFlagErrorConcealment = OMX_FALSE;
            mParamPortDefinition.format.image.eCompressionFormat    = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingUnused;
            mParamPortDefinition.format.image.eColorFormat          = defaultDef->format.image.eColorFormat;
            mParamPortDefinition.format.image.pNativeWindow         = 0;

            mInternalFrameWidth  = mParamPortDefinition.format.image.nFrameWidth;
            mInternalFrameHeight = mParamPortDefinition.format.image.nFrameHeight;

            /*  Get the number of byte per pixel */
            mBytesPerPixel = getPixelDepth(mParamPortDefinition.format.image.eColorFormat);

            /* Check hardware constraints */
            getHwConstraints(defaultDef->nPortIndex, defaultDef->format.image.eColorFormat,&mWidth, &mHeight, &mStride);

            /* Align buffer allocation according to the hardware constraint*/
            mInternalFrameWidth  = (mInternalFrameWidth  + (mWidth-1))&~(mWidth-1);
            mInternalFrameHeight = (mInternalFrameHeight + (mHeight-1))&~(mHeight-1);

            /* Buffer size taking into account hardware constraint */
            bufferSize = (OMX_U32) ((double) (mInternalFrameWidth * mInternalFrameHeight) * mBytesPerPixel);

            /* Set stride and sliceHeight */
            stride      = mInternalFrameWidth * mStride;
            sliceHeight = mInternalFrameHeight;

            //Increase buffer size for adding extradata
            bufferSize += Extradata::GetExtradataSize(Component_Camera, defaultDef->nPortIndex);

            if (bufferSize == 0) {
                DBGT_ASSERT(0);
            }

            /* Updated param */
            mParamPortDefinition.nBufferSize               = bufferSize;
            mParamPortDefinition.format.image.nStride      = stride;
            mParamPortDefinition.format.image.nSliceHeight = sliceHeight;

            DBGT_PTRACE("mParamPortDefinition.format.image.nFrameWidth        = %d",
                    (int)mParamPortDefinition.format.image.nFrameWidth);
            DBGT_PTRACE("mParamPortDefinition.format.image.nFrameHeight       = %d",
                    (int)mParamPortDefinition.format.image.nFrameHeight);
            DBGT_PTRACE("mParamPortDefinition.format.image.eCompressionFormat = %d",
                    (int)mParamPortDefinition.format.image.eCompressionFormat);
            DBGT_PTRACE("mParamPortDefinition.format.image.eColorFormat       = %d",
                    (int)mParamPortDefinition.format.image.eColorFormat);
            DBGT_PTRACE("mParamPortDefinition.format.image.nStride            = %d",
                    (int)mParamPortDefinition.format.image.nStride);
            DBGT_PTRACE("mParamPortDefinition.format.image.nSliceHeight       = %d",
                    (int)mParamPortDefinition.format.image.nSliceHeight);
            DBGT_PTRACE("mParamPortDefinition.nBufferSize                     = %d",
                    (int)mParamPortDefinition.nBufferSize);
            break;
        }
    default:
        {
            DBGT_ASSERT(0); //this should not be called for other pipeId
            break;
        }
    }

    DBGT_EPILOG("");
}


OMX_ERRORTYPE camport::checkFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef)
{
    /* This is very dangerous as it implies that the operating mode is known at that time.*/
    /* It means that IndexCommonSensorMode should be set before the OMX_IndexParamPortDefinition */
    /* If not, sizes/format are evaluated in Still mode as it is the default one */
    /* Could do : also call this function on IndexCommonSensorMode so that
     * it returns an error, once the operating mode is known */

    if(portdef.nPortIndex==CAMERA_PORT_OUT0)
    {
        /* check Size*/
        if(portdef.format.video.nFrameWidth > getMaxRes(CAMERA_PORT_OUT0)) return OMX_ErrorBadParameter;

        /* Check Format */
        if(isSupportedFmt(CAMERA_PORT_OUT0, portdef.format.video.eColorFormat, (t_uint32)portdef.format.video.eCompressionFormat) != OMX_TRUE) return OMX_ErrorBadParameter;

        return OMX_ErrorNone;
    }
    else if(portdef.nPortIndex==CAMERA_PORT_OUT1)
    {
        /* check Size*/
        if(portdef.format.image.nFrameWidth > getMaxRes(CAMERA_PORT_OUT1)) return OMX_ErrorBadParameter;

        /* Check Format */
        if(isSupportedFmt(CAMERA_PORT_OUT1,portdef.format.image.eColorFormat, (t_uint32)portdef.format.image.eCompressionFormat) != OMX_TRUE) return OMX_ErrorBadParameter;

        return OMX_ErrorNone;
    }
    else
        return OMX_ErrorBadParameter;//portId is not correct.
}


typedef struct CAM_PORT_INDEX_STRUCT {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
} CAM_PORT_INDEX_STRUCT;


OMX_ERRORTYPE camport::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    switch (nParamIndex)
    {
    case OMX_IndexParamCommonSensorMode:
        {
            CAM_PORT_INDEX_STRUCT *portIdxStruct =
                static_cast<CAM_PORT_INDEX_STRUCT *>(pComponentParameterStructure);

            /* check that indeed it is an OMX_ALL else return an error */
            DBGT_PTRACE("camport::setParameter, nPortIndex %ld",portIdxStruct->nPortIndex);

            OMX_PARAM_SENSORMODETYPE *pSensorMode = (OMX_PARAM_SENSORMODETYPE *)pComponentParameterStructure;
            DBGT_PTRACE("New sensor mode: Framerate: %ffps, Framesize: %ux%u",
                    (double)pSensorMode->nFrameRate,
                    (char)pSensorMode->sFrameSize.nWidth,
                    (char)pSensorMode->sFrameSize.nHeight );

            bOneShot=pSensorMode->bOneShot;

            ENS_Component* comp = (ENS_Component*)&getENSComponent();

            OMX_STATETYPE state;
            comp->GetState(comp,&state);

            if( state == OMX_StateLoaded)
            {
                /* update the opmode whatever the portState and whatever OMX_ALL was set or not */
                DBGT_PTRACE("in loaded state");

                COmxCamera* omxcam = (COmxCamera*)&getENSComponent();
                Camera* cam = (Camera*)&omxcam->getProcessingComponent();

                omxcam->mSensorMode=*(OMX_PARAM_SENSORMODETYPE*)pComponentParameterStructure;

                if(pSensorMode->bOneShot== OMX_FALSE) cam->setOperatingMode(OpMode_Cam_VideoPreview);
                else  cam->setOperatingMode(OpMode_Cam_StillPreview);

                DBGT_PTRACE("bOneShot %d", pSensorMode->bOneShot);

                error = OMX_ErrorNone;

            }
            else
            {
                DBGT_PTRACE("not in loaded state");
                // in idle state all ports must be disabled and should have the same value of bOneShot on the last call of this setParameter

                camport *    port0 = (camport *) comp->getPort(CAMERA_PORT_OUT0);
                camport *    port1 = (camport *) comp->getPort(CAMERA_PORT_OUT1);

                if((port0->isEnabled() == OMX_TRUE) || (port1->isEnabled() == OMX_TRUE))
                {
                    DBGT_PTRACE("all port are not disabled");
                    //if one of the port is enabled we dont change the current opemode, it wont be applied.
                    // ENS will report an error
                    // but suppose vpb2 is enabled, we would have process this function for vpb1 and vpb0.
                    // taht s why we do not do anything in that case.
                    error = OMX_ErrorNone;
                }
                else // means all of them are disabled
                {
                    DBGT_PTRACE("all port are disabled");
                    // in that case all bOneShot should be the same but this has to be checked only on the last call of setParamter, means on vpb2
                    if(portIdxStruct->nPortIndex == CAMERA_PORT_OUT1)
                    {
                        if((port0->bOneShot== OMX_TRUE) && (port1->bOneShot== OMX_TRUE))
                        {
                            DBGT_PTRACE("bOneShot true for all ports, change mSensorMode and stillPathEnabled and opmode for camera component");
                            COmxCamera* omxcam = (COmxCamera*)&getENSComponent();
                            Camera* cam = (Camera*)&omxcam->getProcessingComponent();
                            omxcam->mSensorMode=*(OMX_PARAM_SENSORMODETYPE*)pComponentParameterStructure;
                            cam->setOperatingMode(OpMode_Cam_StillPreview);

                            error = OMX_ErrorNone;
                        }
                        else if((port0->bOneShot== OMX_FALSE) && (port1->bOneShot== OMX_FALSE))
                        {
                            DBGT_PTRACE("bOneShot false for all ports, change mSensorMode and stillPathEnabled and opmode for camera component");
                            COmxCamera* omxcam = (COmxCamera*)&getENSComponent();
                            Camera* cam = (Camera*)&omxcam->getProcessingComponent();
                            omxcam->mSensorMode=*(OMX_PARAM_SENSORMODETYPE*)pComponentParameterStructure;
                            cam->setOperatingMode(OpMode_Cam_VideoPreview);

                            error = OMX_ErrorNone;
                        }
                        else
                        {
                            DBGT_PTRACE("bOneShot ot the same for all ports, return error");
                            // all port have not the same value, do not change opmode camera and return error
                            // this means it has come without a OMX_ALL, or without same values for all portIndex
                            error = OMX_ErrorBadPortIndex;
                        }
                    }
                    else
                    {
                        DBGT_PTRACE("not vpb1, dont do anything yet");
                        error = OMX_ErrorNone;
                    }
                }
            }
            break;
        }
    default :
        error = ENS_Port::setParameter(nParamIndex,pComponentParameterStructure);
        break;
    }
    return error;
}

OMX_ERRORTYPE camport::getParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) const
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    switch (nParamIndex)
    {
    case OMX_IndexParamCommonSensorMode:
        {
            CAM_PORT_INDEX_STRUCT *portIdxStruct =
                static_cast<CAM_PORT_INDEX_STRUCT *>(pComponentParameterStructure);
            (void)portIdxStruct->nPortIndex;
            /* check that indeed it is an OMX_ALL else return an error */
            DBGT_PTRACE("camport::getParameter, nPortIndex %ld",portIdxStruct->nPortIndex);

            /* dont do  anything yet */
            error = OMX_ErrorNone;
            break;
        }
    default :
        error = ENS_Port::getParameter(nParamIndex,pComponentParameterStructure);
        break;
    }
    return error;
}

OMX_ERRORTYPE camport::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef)
{
    DBGT_PROLOG("");

    DBGT_PTRACE("PortIndex = %d", (int)portdef.nPortIndex);

    t_uint16 mWidth      = 0;
    t_uint16 mHeight     = 0;
    t_uint16 mStride     = 0;
    t_sint32 stride      = 0;
    t_uint32 sliceHeight = 0;
    OMX_U32 bufferSize   = 0;
    OMX_ERRORTYPE error  = OMX_ErrorNone;

    error = checkFormatInPortDefinition(portdef);
    if(error != OMX_ErrorNone)
    {
        goto end;
    }

    switch(portdef.nPortIndex)
    {
    case CAMERA_PORT_OUT0:
        {
            /* Video port */
            mParamPortDefinition.format.video.cMIMEType             = portdef.format.video.cMIMEType;
            mParamPortDefinition.format.video.xFramerate            = portdef.format.video.xFramerate;
            mParamPortDefinition.format.video.nFrameWidth           = portdef.format.video.nFrameWidth;
            mParamPortDefinition.format.video.nFrameHeight          = portdef.format.video.nFrameHeight;
            mParamPortDefinition.format.video.pNativeRender         = portdef.format.video.pNativeRender;
            mParamPortDefinition.format.video.bFlagErrorConcealment = portdef.format.video.bFlagErrorConcealment;
            mParamPortDefinition.format.video.eCompressionFormat    = portdef.format.video.eCompressionFormat;
            mParamPortDefinition.format.video.eColorFormat          = portdef.format.video.eColorFormat;
            mParamPortDefinition.format.video.pNativeWindow         = portdef.format.video.pNativeWindow;

            mInternalFrameWidth  = mParamPortDefinition.format.video.nFrameWidth;
            mInternalFrameHeight = mParamPortDefinition.format.video.nFrameHeight;

            /*  Get the number of byte per pixel */
            mBytesPerPixel = getPixelDepth(mParamPortDefinition.format.video.eColorFormat);

            /* Check hardware constraints */
            getHwConstraints(portdef.nPortIndex, portdef.format.video.eColorFormat,&mWidth, &mHeight, &mStride);

            /* Align buffer allocation according to the hardware constraints.
             * If width/height set by client is not compliant to HW
             * constraints, handle those internally. Note that port
             * configuration seen by client (getParameter) is based on
             * the (potentially unaligned) values set by the client ! */
            mInternalFrameWidth  = (mInternalFrameWidth  + (mWidth-1))&~(mWidth-1);
            mInternalFrameHeight = (mInternalFrameHeight + (mHeight-1))&~(mHeight-1);

            if((mInternalFrameWidth  != mParamPortDefinition.format.video.nFrameWidth) ||
                    (mInternalFrameHeight != mParamPortDefinition.format.video.nFrameHeight))
            {
                DBGT_WARNING("Frame size requested is %dx%d.",
                        (int)mParamPortDefinition.format.video.nFrameWidth,
                        (int)mParamPortDefinition.format.video.nFrameHeight);
                DBGT_WARNING("Frame size output by the camera need to be %dx%d to respect hardware constraint alignment",
                        (int)mInternalFrameWidth,
                        (int)mInternalFrameHeight);
                DBGT_PTRACE("Allocated buffer size will be %dx%d to respect hardware constraint alignment for current pixel format",
                        (int)mInternalFrameWidth,
                        (int)mInternalFrameHeight);
            } else {
                DBGT_PTRACE("Allocated buffer size will be %dx%d (same as the frame size)",
                        (int)mInternalFrameWidth,
                        (int)mInternalFrameHeight);
            }

            /* Buffer size taking into account hardware constraint */
            bufferSize = (OMX_U32) ((double) (mInternalFrameWidth * mInternalFrameHeight) * mBytesPerPixel);

#ifdef STAB
            if ((mParamPortDefinition.format.video.eColorFormat != OMX_COLOR_FormatRawBayer8bit)&&
                    (mParamPortDefinition.format.video.eColorFormat !=OMX_COLOR_FormatRawBayer10bit )) {
                bufferSize += sizeof(OMX_DIGITALVIDEOSTABINFOTYPE);
                /* allocate 30% extra to accomodate stab overscanning */
                bufferSize += (bufferSize*7)/9;
            }
#endif

            if (mParamPortDefinition.format.video.eColorFormat == OMX_STE_COLOR_FormatRawData){
                // this pixel format is composed of a yuv frame & a jpeg
                // Both of them are interlaced. The buffer size is fix to 10 Mbytes
                bufferSize = 0xA00000;
            }

            /* Set stride and sliceHeight */
            stride      = mInternalFrameWidth * mStride;
            sliceHeight = mInternalFrameHeight;

            if (bufferSize == 0)
            {
                DBGT_ASSERT(0); //frameWidth and height have been overwritten, assert
            }

            /* Updated param */
            mParamPortDefinition.nBufferSize               = bufferSize;
            mParamPortDefinition.format.video.nStride      = stride;
            mParamPortDefinition.format.video.nSliceHeight = sliceHeight; //use nSliceHeight to warn about buffer alignement need to be used to configure SIA DMA
            portSettingsChanged = OMX_TRUE;

            DBGT_PTRACE("mParamPortDefinition.format.video.xFramerate         = %d",
                    (int)mParamPortDefinition.format.video.xFramerate);
            DBGT_PTRACE("mParamPortDefinition.format.video.nFrameWidth        = %d",
                    (int)mParamPortDefinition.format.video.nFrameWidth);
            DBGT_PTRACE("mParamPortDefinition.format.video.nFrameHeight       = %d",
                    (int)mParamPortDefinition.format.video.nFrameHeight);
            DBGT_PTRACE("mParamPortDefinition.format.video.eCompressionFormat = %d",
                    (int)mParamPortDefinition.format.video.eCompressionFormat);
            DBGT_PTRACE("mParamPortDefinition.format.video.eColorFormat       = %d",
                    (int)mParamPortDefinition.format.video.eColorFormat);
            DBGT_PTRACE("mParamPortDefinition.format.video.nStride            = %d",
                    (int)mParamPortDefinition.format.video.nStride);
            DBGT_PTRACE("mParamPortDefinition.format.video.nSliceHeight       = %d",
                    (int)mParamPortDefinition.format.video.nSliceHeight);
            DBGT_PTRACE("mParamPortDefinition.nBufferSize                     = %d",
                    (int)mParamPortDefinition.nBufferSize);

            break;
        }
    case CAMERA_PORT_OUT1:
        {
            /* Video port */
            mParamPortDefinition.format.image.cMIMEType             = portdef.format.image.cMIMEType;
            mParamPortDefinition.format.image.nFrameWidth           = portdef.format.image.nFrameWidth;
            mParamPortDefinition.format.image.nFrameHeight          = portdef.format.image.nFrameHeight;
            mParamPortDefinition.format.image.pNativeRender         = portdef.format.image.pNativeRender;
            mParamPortDefinition.format.image.bFlagErrorConcealment = portdef.format.image.bFlagErrorConcealment;
            mParamPortDefinition.format.image.eCompressionFormat    = portdef.format.image.eCompressionFormat;
            mParamPortDefinition.format.image.eColorFormat          = portdef.format.image.eColorFormat;
            mParamPortDefinition.format.image.pNativeWindow         = portdef.format.image.pNativeWindow;

            mInternalFrameWidth  = portdef.format.image.nFrameWidth;
            mInternalFrameHeight = portdef.format.image.nFrameHeight;

	    DBGT_PTRACE("=========================eCompressionFormat = %d\n", mParamPortDefinition.format.image.eCompressionFormat );

            if(mParamPortDefinition.format.image.eCompressionFormat == OMX_IMAGE_CodingJPEG)
            {
                /*  We are in the external ISP Jpeg case */

                /*  We retrive from the driver .dat file the file size needed
                 *  to store the Jpeg. */
                int bSuccess;
                COmxCamera* omxcam = (COmxCamera*)&getENSComponent();
                Camera* cam = (Camera*)&omxcam->getProcessingComponent();
                HAL_Gr_Camera_SizeConfig_t* SizeConfig_p = new HAL_Gr_Camera_SizeConfig_t;
                t_uint8 Id = 0;
                bool found = 0;

                DBGT_PTRACE("============================setFormatInPortDefinition Before SizeConfig - %d %d", mInternalFrameWidth, mInternalFrameHeight);
                while(!found)
                {
                    bSuccess = cam->cam_Ctrllib->Camera_CtrlLib_GetSizeConfig(cam->cam_h, Id, SizeConfig_p);
                    if(!bSuccess){
                        DBGT_PTRACE("Error from Camera_CtrlLib_GetSizeConfig: Id not found");
                        DBGT_ASSERT(0);
                        break;
                    }


                    DBGT_PTRACE("SizeConfig[%d] - %d %d", Id, SizeConfig_p->Width, SizeConfig_p->Height,SizeConfig_p->Type );
                    if((SizeConfig_p->Height == (int)mInternalFrameHeight) &&
                            (SizeConfig_p->Width == (int)mInternalFrameWidth) &&
                            (SizeConfig_p->Type == 0)) /*For raw capture table contains 0*/
                        found = 1;
                    else
                        Id++;
                }

                bSuccess = cam->cam_Ctrllib->Camera_CtrlLib_GetNeededJPEG_BufferSize(cam->cam_h, Id, (int*) &bufferSize);
                if(!bSuccess)
                {
                    DBGT_PTRACE("Error from Camera_CtrlLib_GetNeededJPEG_BufferSize");
                    DBGT_ASSERT(0);
                }

                DBGT_PTRACE("Allocated buffer size for Jpeg = %d", (int)bufferSize);

                if (bufferSize == 0)
                {
                    /*  Driver .dat shoul define the buffer size needed in
                     *  case of Jpeg picture. */
                    DBGT_ASSERT(0);
                }

                /* Set stride and sliceHeight */
                stride      = 0;
                sliceHeight = 1; //use nSliceHeight to warn about buffer alignement needed to be used to configure SIA DMA
            }
            else
            {
                /*  We are in external ISP yuv case */

                /*  Get the number of byte per pixel */
                mBytesPerPixel = getPixelDepth(mParamPortDefinition.format.image.eColorFormat);

                /* Check hardware constraints */
                getHwConstraints(portdef.nPortIndex, portdef.format.image.eColorFormat,&mWidth, &mHeight, &mStride);

                /* Align buffer allocation according to the hardware constraints.
                 * If width/height set by client is not compliant to HW
                 * constraints, handle those internally. Note that port
                 * configuration seen by client (getParameter) is based on
                 * the (potentially unaligned) values set by the client ! */
                mInternalFrameWidth  = (mInternalFrameWidth  + (mWidth-1))&~(mWidth-1);
                mInternalFrameHeight = (mInternalFrameHeight + (mHeight-1))&~(mHeight-1);

                if((mInternalFrameWidth  != mParamPortDefinition.format.image.nFrameWidth) ||
                        (mInternalFrameHeight != mParamPortDefinition.format.image.nFrameHeight))
                {
                    DBGT_WARNING("Frame size requested is %dx%d.",
                            (int)mParamPortDefinition.format.video.nFrameWidth,
                            (int)mParamPortDefinition.format.video.nFrameHeight);
                    DBGT_WARNING("Frame size output by the camera need to be %dx%d to respect hardware constraint alignment",
                            (int)mInternalFrameWidth,
                            (int)mInternalFrameHeight);
                    DBGT_PTRACE("Allocated buffer size will be %dx%d to respect hardware constraint alignment for current pixel format",
                            (int)mInternalFrameWidth,
                            (int)mInternalFrameHeight);
                }
                else
                {
                    DBGT_PTRACE("Allocated buffer size will be %dx%d (same as the frame size)",
                            (int)mInternalFrameWidth,
                            (int)mInternalFrameHeight);
                }

                /* Buffer size taking into account hardware constraint */
                bufferSize = (OMX_U32) ((double) (mInternalFrameWidth * mInternalFrameHeight) * mBytesPerPixel);

                /* Set stride and sliceHeight */
                stride      = mInternalFrameWidth * mStride;
                sliceHeight = mInternalFrameHeight;
            }

            //Increase buffer size for adding extradata
            bufferSize += Extradata::GetExtradataSize(Component_Camera, portdef.nPortIndex);

            if (bufferSize == 0)
            {
                DBGT_ASSERT(0); //frameWidth and height have been overwritten, assert
            }

            /* Updated param */
            mParamPortDefinition.nBufferSize               = bufferSize;
            mParamPortDefinition.format.image.nStride      = stride;
            mParamPortDefinition.format.image.nSliceHeight = sliceHeight; //use nSliceHeight to warn about buffer alignement need to be used to configure SIA DMA
            portSettingsChanged = OMX_TRUE;

            DBGT_PTRACE("mParamPortDefinition.format.image.nFrameWidth        = %d",
                    (int)mParamPortDefinition.format.image.nFrameWidth);
            DBGT_PTRACE("mParamPortDefinition.format.image.nFrameHeight       = %d",
                    (int)mParamPortDefinition.format.image.nFrameHeight);
            DBGT_PTRACE("mParamPortDefinition.format.image.eCompressionFormat = %d",
                    (int)mParamPortDefinition.format.image.eCompressionFormat);
            DBGT_PTRACE("mParamPortDefinition.format.image.eColorFormat       = %d",
                    (int)mParamPortDefinition.format.image.eColorFormat);
            DBGT_PTRACE("mParamPortDefinition.format.image.nStride            = %d",
                    (int)mParamPortDefinition.format.image.nStride);
            DBGT_PTRACE("mParamPortDefinition.format.image.nSliceHeight       = %d",
                    (int)mParamPortDefinition.format.image.nSliceHeight);
            DBGT_PTRACE("mParamPortDefinition.nBufferSize                     = %d",
                    (int)mParamPortDefinition.nBufferSize);
            break;
        }
    default:
        {
            DBGT_ASSERT(0); //this should not be called for other pipeId
            break;
        }
    }

end:
    DBGT_EPILOG("");
    return error;
}


void camport::getHwConstraints(t_uint16 portId,OMX_COLOR_FORMATTYPE omxformat, t_uint16 * p_multiple_width, t_uint16 * p_multiple_height, t_uint16 * p_multiple_stride)
{
    /* table coming from hardware specification */
    /* return value is multiple of pixels */
    switch(portId) {
    case CAMERA_PORT_OUT0: //CAM Pipe
    case CAMERA_PORT_OUT1: //CAM Pipe
        {
            switch((t_uint32)omxformat)
            {
            case OMX_STE_COLOR_FormatRawData:
                *p_multiple_width  = 1;
                *p_multiple_height = 1;
                *p_multiple_stride = 1;
                break;
            case OMX_COLOR_FormatCbYCrY:
                *p_multiple_width  = 8;
                *p_multiple_height = 1;
                *p_multiple_stride = 2;
                break;
            case OMX_COLOR_FormatYUV420Planar:
            case OMX_COLOR_FormatYUV420SemiPlanar:
                *p_multiple_width  = 8;
                *p_multiple_height = 2;
                *p_multiple_stride = 1;
                break;
            case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
                *p_multiple_width  = 16;
                *p_multiple_height = 16;
                *p_multiple_stride = 1;
                break;
            default:
                DBGT_ASSERT(0); //this should not be called for other omxformat
                break;
            }
            break;
        }
    default:
        DBGT_ASSERT(0); //this should not be called for other pipeId
        break;
    }
}


/*
 * Supported Fmt table
 */
OMX_BOOL camport::isSupportedFmt(t_uint16 portID, OMX_COLOR_FORMATTYPE omxformat, t_uint32 omxcodingformat)
{
    switch(portID)
    {
    case CAMERA_PORT_OUT0:
        {
            OMX_IMAGE_CODINGTYPE omxencodingformat = (OMX_IMAGE_CODINGTYPE)omxcodingformat;

            switch (omxencodingformat) {
            case OMX_IMAGE_CodingUnused:
                break;
            case OMX_IMAGE_CodingJPEG:
                return OMX_TRUE;
            default:
                return OMX_FALSE;
            }

            switch ((t_uint32)omxformat) {
            case OMX_STE_COLOR_FormatRawData:
            case OMX_COLOR_FormatCbYCrY:
            case OMX_COLOR_FormatYUV420Planar:
            case OMX_COLOR_FormatYUV420SemiPlanar:
            case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
                return OMX_TRUE;
            default:
                return OMX_FALSE;
            }
        }
    case CAMERA_PORT_OUT1:
        {
            OMX_IMAGE_CODINGTYPE omxencodingformat = (OMX_IMAGE_CODINGTYPE)omxcodingformat;

            switch (omxencodingformat) {
            case OMX_IMAGE_CodingUnused:
                break;
            case OMX_IMAGE_CodingJPEG:
                return OMX_TRUE;
            default:
                return OMX_FALSE;
            }

            switch ((t_uint32)omxformat) {
            case OMX_STE_COLOR_FormatRawData:
            case OMX_COLOR_FormatCbYCrY:
            case OMX_COLOR_FormatYUV420Planar:
            case OMX_COLOR_FormatYUV420SemiPlanar:
            case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
                return OMX_TRUE;
            default:
                return OMX_FALSE;
            }
        }
    default:
        DBGT_ASSERT(0,"portId has been corrupted");//portId has been corrupted
        return OMX_FALSE;
    }
}


t_uint16 camport::getMaxRes(t_uint16 portId) {
    switch (portId) {
    case CAMERA_PORT_OUT0:
        return 3280;
    case CAMERA_PORT_OUT1:
        return 3280;
    default:
        DBGT_ASSERT(0); // portId has been overriden
        return 0;
    }
}

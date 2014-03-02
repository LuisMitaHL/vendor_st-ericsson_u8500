/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#define DBGT_PREFIX "CAMPORT"


#include "camport.h"
#include "osi_trace.h"
#include "grabctlcommon.h"
#include "IFM_Types.h"
#include "extradata.h"
#include "camera.h"
#ifndef CR_ZSL_YUV
#define CR_ZSL_YUV
#endif

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_camportTraces.h"
#endif

void camport::setDefaultFormatInPortDefinition(OMX_PARAM_PORTDEFINITIONTYPE * defaultDef)
{
	IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry camport::setDefaultFormatInPortDefinition");
    OMX_U32 bufferSize = 0;

	mParamPortDefinition.format.video.cMIMEType =NULL;
	mParamPortDefinition.format.video.pNativeRender = 0 ;// not used
	mParamPortDefinition.format.video.nFrameWidth = defaultDef->format.video.nFrameWidth;
	mParamPortDefinition.format.video.nFrameHeight = defaultDef->format.video.nFrameHeight;
	mParamPortDefinition.format.video.nBitrate = 0;
	mParamPortDefinition.format.video.xFramerate = defaultDef->format.video.xFramerate;
	mParamPortDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
	mParamPortDefinition.format.video.eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_IMAGE_CodingUnused;
	mParamPortDefinition.format.video.eColorFormat = defaultDef->format.video.eColorFormat;
	mParamPortDefinition.format.video.pNativeWindow = 0;
	mParamPortDefinition.bBuffersContiguous = OMX_TRUE;

	mInternalFrameWidth = defaultDef->format.video.nFrameWidth;
	mInternalFrameHeight = defaultDef->format.video.nFrameHeight;

	mBytesPerPixel = getPixelDepth(mParamPortDefinition.format.video.eColorFormat);

    mParamPortDefinition.format.video.nStride = getStride(defaultDef->format.video.eColorFormat, mInternalFrameWidth, NULL);
    mParamPortDefinition.format.video.nSliceHeight = mInternalFrameHeight;

	MSG2("Default mParamPortDefinition.format.video.xFramerate = %ld (%ffps)\n", defaultDef->format.video.xFramerate, ((float)defaultDef->format.video.xFramerate)/ (1<<16) );
	OstTraceFiltInst2(TRACE_DEBUG, "Default mParamPortDefinition.format.video.xFramerate = %ld (%ffps)", defaultDef->format.video.xFramerate, ((float)defaultDef->format.video.xFramerate)/ (1<<16) );
	MSG1("Default mParamPortDefinition.format.video.nFrameWidth = %ld\n",defaultDef->format.video.nFrameWidth);
	OstTraceFiltInst1(TRACE_DEBUG, "Default mParamPortDefinition.format.video.nFrameWidth = %ld",defaultDef->format.video.nFrameWidth);
	MSG1("Default mParamPortDefinition.format.video.nFrameHeight = %ld\n",defaultDef->format.video.nFrameHeight);
	OstTraceFiltInst1(TRACE_DEBUG, "Default mParamPortDefinition.format.video.nFrameHeight = %ld",defaultDef->format.video.nFrameHeight);
	MSG1("Default mParamPortDefinition.format.video.eColorFormat = %d\n",defaultDef->format.video.eColorFormat);
	OstTraceFiltInst1(TRACE_DEBUG, "Default mParamPortDefinition.format.video.eColorFormat = %d",defaultDef->format.video.eColorFormat);

    bufferSize = getBufferSize(mParamPortDefinition.format.video.eColorFormat, mParamPortDefinition.format.video.nStride,
                                           mParamPortDefinition.format.video.nFrameWidth, mParamPortDefinition.format.video.nSliceHeight);
	bufferSize += Extradata::GetExtradataSize(Component_Camera, defaultDef->nPortIndex);
	if (bufferSize == 0) {
		DBC_ASSERT(0);
	}
   	mParamPortDefinition.nBufferSize = (bufferSize+0x1000) & 0xFFFFF000; // round to next 4K

	MSG1("mParamPortDefinition.nBufferSize = %ld\n",mParamPortDefinition.nBufferSize);
	OstTraceFiltInst1(TRACE_DEBUG, "mParamPortDefinition.nBufferSize = %ld",mParamPortDefinition.nBufferSize);

	OUTR(" ",(0));
	OstTraceFiltInst0(TRACE_FLOW, "Exit camport::setDefaultFormatInPortDefinition");

}

OMX_ERRORTYPE camport::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef)
{
    IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry camport::setFormatInPortDefinition");

    t_uint16 mStride=1;
    t_uint16 mWidth = 0;
    t_uint16 mHeight = 0;
    OMX_S32 stride = 0;
    OMX_U32 bufferSize=0;

    COmxCamera* omxcam = (COmxCamera*)&getENSComponent();
    OMX_STATETYPE state;
    omxcam->GetState(omxcam,&state);
    Camera *cam = (Camera *)&omxcam->getProcessingComponent();

    OMX_ERRORTYPE error = checkFormatInPortDefinition(portdef);
    if(error != OMX_ErrorNone) {OUT0("\n"); return error;}

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
    OMX_PARAM_PORTDEFINITIONTYPE oldPortDef = mParamPortDefinition;
#endif
/*This is complex to understand dont touch this*/
if((mParamPortDefinition.format.video.eColorFormat != portdef.format.video.eColorFormat )&&(mParamPortDefinition.nPortIndex==CAMERA_PORT_OUT1))
{
    if( state != OMX_StateLoaded) {
    	MSG0("in loaded state\n");
    	OstTraceFiltInst0(TRACE_DEBUG, "in loaded state");
    	if(portdef.nPortIndex == CAMERA_PORT_OUT1)
    	{
     	/* update the opmode whatever the portState and whatever OMX_ALL was set or not */
		 GET_PROPERTY(DIPLAY_720_PROP, val, 0); \
		 cam->mDisplay720p=0;
		 cam->mDisplay720p=strtoul(val,NULL,16); \
		 if(portdef.format.video.eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)
		 {
		 	cam->setPipes(GRBPID_PIPE_LR,GRBPID_PIPE_HR,GRBPID_PIPE_HR);
		 	MSG1( "camport::Pipes 2: (%d)", GRBPID_PIPE_HR);
			
		 }
		 else
		 {
	 		cam->setPipes(GRBPID_PIPE_LR,GRBPID_PIPE_RAW_OUT,GRBPID_PIPE_HR);
	 		MSG1( "camport::Pipes 2: (%d)", GRBPID_PIPE_RAW_OUT); 
		 }
    	}
	else
	{
		//cam->setPipes(GRBPID_PIPE_LR,GRBPID_PIPE_RAW_OUT,GRBPID_PIPE_HR);
	}
     }
    else
    {

     }
}
    if((portdef.nPortIndex == CAMERA_PORT_OUT0)||(portdef.nPortIndex == CAMERA_PORT_OUT2)||(portdef.nPortIndex == CAMERA_PORT_OUT1))
    {
        /* check hw constraints TODO: check for stab */
        getHwConstraints(portdef.nPortIndex, portdef.format.video.eColorFormat,&mWidth, &mHeight, &mStride);
	if(portdef.format.video.eColorFormat != (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatRawBayer12bit)
	{
        /* the stride and sliceHeight overridden by client must respect HW constraints */
        if(portdef.format.video.nStride%mStride !=0) return OMX_ErrorBadParameter;
        if(portdef.format.video.nSliceHeight%mHeight !=0) return OMX_ErrorBadParameter;

        /* if width/height set by client is not compliant to HW constraints, handle those internally.
        Note that port configuration seen by client (getParameter) is based on the (potentially unaligned) 
        values set by the client ! */  
        if(portdef.format.video.nFrameWidth%mWidth !=0) {
            mInternalFrameWidth = portdef.format.video.nFrameWidth + (mWidth-(portdef.format.video.nFrameWidth%mWidth));
        }
        else {
            mInternalFrameWidth = portdef.format.video.nFrameWidth;
        }

            if (portdef.format.video.nFrameHeight % mHeight != 0) {
                mInternalFrameHeight = portdef.format.video.nFrameHeight + (mHeight - (portdef.format.video.nFrameHeight % mHeight));
            } else {
                mInternalFrameHeight = portdef.format.video.nFrameHeight;
            }
            mParamPortDefinition.format.video.nFrameWidth = portdef.format.video.nFrameWidth;
            mParamPortDefinition.format.video.nFrameHeight = portdef.format.video.nFrameHeight;
        } else {
                mParamPortDefinition.format.video.nFrameWidth = cam->iSensor->getBayerWidth();
                mParamPortDefinition.format.video.nFrameHeight = cam->iSensor->getBayerHeight();
                mParamPortDefinition.format.video.nSliceHeight = mParamPortDefinition.format.video.nFrameHeight;
                mInternalFrameWidth = mParamPortDefinition.format.video.nFrameWidth;
                mInternalFrameHeight = mParamPortDefinition.format.video.nFrameHeight;
        }
    }

    mParamPortDefinition.format.video.cMIMEType = portdef.format.video.cMIMEType;
    mParamPortDefinition.format.video.xFramerate = portdef.format.video.xFramerate;
	mParamPortDefinition.format.video.pNativeRender = portdef.format.video.pNativeRender ;// not used
	mParamPortDefinition.format.video.bFlagErrorConcealment = portdef.format.video.bFlagErrorConcealment;
	mParamPortDefinition.format.video.eCompressionFormat = portdef.format.video.eCompressionFormat;
	mParamPortDefinition.format.video.eColorFormat = portdef.format.video.eColorFormat;
	mParamPortDefinition.format.video.pNativeWindow = portdef.format.video.pNativeWindow;

	mBytesPerPixel = getPixelDepth(mParamPortDefinition.format.video.eColorFormat);
	
    /* Calculate stride */
    stride = getStride(portdef.format.video.eColorFormat, mInternalFrameWidth, NULL);
	/* check the sanity of the client-provided stride value */
    if(portdef.format.video.nStride > stride) {
        mParamPortDefinition.format.video.nStride = portdef.format.video.nStride;
    } 
    else {
        mParamPortDefinition.format.video.nStride = stride;
        /* make sure it complies with HW constraints */
        if((mParamPortDefinition.format.video.nStride % mStride) != 0) {
            mParamPortDefinition.format.video.nStride += (mStride -(mParamPortDefinition.format.video.nStride % mStride));          
        }
	}

	/* check the sanity of the client-provided nSliceHeight value */
 	if (portdef.format.video.nSliceHeight > mInternalFrameHeight) {
        mParamPortDefinition.format.video.nSliceHeight = portdef.format.video.nSliceHeight;
    } else {
        mParamPortDefinition.format.video.nSliceHeight = mInternalFrameHeight;
    } 

    bIsOverScanned= false;
#ifdef STAB
#if defined(ENABLE_FEATURE_BUILD_HATS)
    if((bOneShot==OMX_FALSE)     /* overscan systematically in case of video mode */
#else
/*Always overscan preview buffer - CR438048*/
    if(((portdef.nPortIndex == CAMERA_PORT_OUT0)||(portdef.nPortIndex == CAMERA_PORT_OUT2))
#endif
        && (mParamPortDefinition.format.video.eColorFormat != OMX_COLOR_FormatRawBayer8bit)
        && (mParamPortDefinition.format.video.eColorFormat != OMX_COLOR_FormatRawBayer10bit)
        && (mParamPortDefinition.format.video.eColorFormat !=(OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatRawBayer12bit)) 
    {
        bIsOverScanned = true;
        /* allocate (max_overscan)%^2 extra to accomodate stab overscanning! */
        /* nStride and nSliceHeight +(max_overscan)% (rounded up to Macroblock16 size) */
        /* we don't really need to round to MB size for portLR, but I prefer to follow HR constraints when stab is enabled */

	 /* +16 is Correction for aspect ratio --should not be less than actual X/Y  else ISP gives Streaming Error*/	
        mMaxOverscannedWidth = ((((mInternalFrameWidth*CAM_OVERSCAN_NOM)/CAM_OVERSCAN_DEN)+15)& ~(0xF)) + 16;
        mMaxOverscannedHeight = ((((mInternalFrameHeight*CAM_OVERSCAN_NOM)/CAM_OVERSCAN_DEN)+15)& ~(0xF));

		/*In case overscanned VF resolution > XGA clip it to XGA and maintain the aspect ratio*/
		if(portdef.nPortIndex == CAMERA_PORT_OUT0 && (mMaxOverscannedWidth > XGA_WIDTH))
    	{
			mMaxOverscannedWidth = XGA_WIDTH;
			mMaxOverscannedHeight = (( (mInternalFrameHeight*mMaxOverscannedWidth)/mInternalFrameWidth)&(~0xF));
		}
		
        bool isInterLeavedFormat;
        stride = getStride(portdef.format.video.eColorFormat, mMaxOverscannedWidth, &isInterLeavedFormat);
        mParamPortDefinition.format.video.nStride = stride;
        /* case of client-provided nStride overriding current stride */
        if (portdef.format.video.nStride > stride) {
    			mParamPortDefinition.format.video.nStride = portdef.format.video.nStride;
            if(isInterLeavedFormat) {
    			mMaxOverscannedWidth = portdef.format.video.nStride/mBytesPerPixel;
    		}
            else {
                mMaxOverscannedWidth = portdef.format.video.nStride;
            }
        }

        mParamPortDefinition.format.video.nSliceHeight = mMaxOverscannedHeight;
        /* case of client-provided nSliceHeight overriding current one */
        if (portdef.format.video.nSliceHeight > mMaxOverscannedHeight) {
    			mParamPortDefinition.format.video.nSliceHeight = portdef.format.video.nSliceHeight;
    			mMaxOverscannedHeight = portdef.format.video.nSliceHeight;
    		}  
	}
    mOverScannedWidth = mMaxOverscannedWidth;
    mOverScannedHeight = mMaxOverscannedHeight;
    MSG1(" mOverScannedWidth= %lu\n",mOverScannedWidth);
    MSG1(" mOverScannedHeight= %lu\n",mOverScannedHeight);
#endif

	bufferSize = getBufferSize(mParamPortDefinition.format.video.eColorFormat, mParamPortDefinition.format.video.nStride,
                                           mParamPortDefinition.format.video.nFrameWidth, mParamPortDefinition.format.video.nSliceHeight);
    MSG2("%s mParamPortDefinition.nPortIndex = %ld \n",__FUNCTION__, mParamPortDefinition.nPortIndex);

	MSG2("%s bufferSize: %ld \n",__FUNCTION__,bufferSize);
	bufferSize += Extradata::GetExtradataSize(Component_Camera, portdef.nPortIndex);
    
    MSG("%s nFrameWidth: %ld,  nFrameHeight: %ld, nStride: %ld, nSliceHeight: %ld, bufferSize: %ld\n",__FUNCTION__,
		mParamPortDefinition.format.video.nFrameWidth,
		mParamPortDefinition.format.video.nFrameHeight,
		mParamPortDefinition.format.video.nStride,
        mParamPortDefinition.format.video.nSliceHeight,
        bufferSize);
     
	if (bufferSize == 0) {
		DBC_ASSERT(0); //frameWidth and height have been overwritten, assert
	}
	mParamPortDefinition.nBufferSize = (bufferSize+0x1000) & 0xFFFFF000; // round to next 4K
    MSG2("%s nBufferSize: %ld",__FUNCTION__,mParamPortDefinition.nBufferSize);
	
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
        if (memcmp(&oldPortDef, &mParamPortDefinition, sizeof(OMX_PARAM_PORTDEFINITIONTYPE)) != 0)
        {
          MSG1("Port Setting is changed!! %d \n", portdef.nPortIndex);
          portSettingsChanged = OMX_TRUE;
        }
#else
    portSettingsChanged = OMX_TRUE;
#endif

	OUTR(" ",OMX_ErrorNone);
	OstTraceFiltInst1(TRACE_FLOW, "Exit camport::setDefaultFormatInPortDefinition (%d)",OMX_ErrorNone);
	return OMX_ErrorNone;

}



OMX_ERRORTYPE camport::checkFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef)
{
	/* This is very dangerous as it implies that the operating mode is known at that time.*/
	/* It means that IndexCommonSensorMode should be set before the OMX_IndexParamPortDefinition */
	/* If not, sizes/format are evaluated in Still mode as it is the default one */
	/* Could do : also call this function on IndexCommonSensorMode so that
	 * it returns an error, once the operating mode is known */

	if(portdef.nPortIndex==CAMERA_PORT_OUT0) {

		/* check Size*/
		if(portdef.format.video.nFrameWidth > getMaxRes(CAMERA_PORT_OUT0)) return OMX_ErrorBadParameter;

		/* Check Format */
		if(isSupportedFmt(CAMERA_PORT_OUT0, portdef.format.video.eColorFormat) != OMX_TRUE) return OMX_ErrorBadParameter;

		return OMX_ErrorNone;
	}
	else if(portdef.nPortIndex==CAMERA_PORT_OUT1) {

		/* check Size */
		/* not possible in loaded state since we do not know sensor modes */
		/* size will be checked when transiting from loaded to idle */
		OMX_STATETYPE State = OMX_StateInvalid;
		COmxCamera* omxcam = (COmxCamera*)&getENSComponent();
		omxcam->GetState(&getENSComponent(), &State);
	
		if ((OMX_StateLoaded != State) && (portdef.format.video.eColorFormat != (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar))
		{
			OstTraceFiltInst1(TRACE_FLOW, "Exit camport::checkFormatInPortDefinition (%d)",State);
			CAM_SM* CamSM = ((Camera*)&omxcam->getProcessingComponent())->p_cam_sm;

			Sensor_Output_Mode_ts* pSensorModes = (Sensor_Output_Mode_ts*)CamSM->pSensorModesShmChunk->armLogicalAddress;
			t_uint16 SensorModesNber = CamSM->pSensor->getSensorModesNber();
			t_uint8 i = 0;
        
			for (i=0 ; i<SensorModesNber ; i++)
			{
				/* u32_usage_restriction_bitmask should also be checked (not available in FW right now) */
				if ((pSensorModes[i].u32_output_res_width == mParamPortDefinition.format.video.nFrameWidth)
					&& ((pSensorModes[i].u32_output_res_height + CamSM->pSensor->getNberOfNonActiveLines()) == mParamPortDefinition.format.video.nFrameHeight)
					&& (pSensorModes[i].u32_woi_res_width == mParamPortDefinition.format.video.nFrameWidth)
					&& ((pSensorModes[i].u32_woi_res_height + CamSM->pSensor->getNberOfNonActiveLines()) == mParamPortDefinition.format.video.nFrameHeight))
				{
					CamSM->currentStillSensorMode = pSensorModes[i];
					break;
				}
			}

			/* no sensor mode corresponding to port resolution */
			if (SensorModesNber == i)
			{
				return OMX_ErrorBadParameter;
			}
		}
        
		/* Check Format */
		if(isSupportedFmt(CAMERA_PORT_OUT1,portdef.format.video.eColorFormat) != OMX_TRUE) return OMX_ErrorBadParameter;

		return OMX_ErrorNone;
	}
	else if(portdef.nPortIndex==CAMERA_PORT_OUT2) {

		/* check Size*/
		if(portdef.format.video.nFrameWidth > getMaxRes(CAMERA_PORT_OUT2)) return OMX_ErrorBadParameter;

		/* Check Format */
		if(isSupportedFmt(CAMERA_PORT_OUT2,portdef.format.video.eColorFormat)!= OMX_TRUE) return OMX_ErrorBadParameter;

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



/* suit le code le plus pourri qui existe sur terre, .... */
OMX_ERRORTYPE camport::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	switch ((t_uint32)nParamIndex)
	{
		case OMX_IndexParamCommonSensorMode:
		    /* JDU: If I understood correctly, OMX_IndexParamCommonSensorMode is a component specific index, and as the ENS doesn't manage them well, we
		     * trick by declaring it port specific on OMX_ALL port, so the ENS calls this function on an OMX setParameter.
		     * Any other explanation would be welcome here...
		     */
		{
			CAM_PORT_INDEX_STRUCT *portIdxStruct =
					static_cast<CAM_PORT_INDEX_STRUCT *>(pComponentParameterStructure);

			/* check that indeed it is an OMX_ALL else return an error */
			MSG1("camport::setParameter, nPortIndex %ld\n",portIdxStruct->nPortIndex);
			OstTraceFiltInst1(TRACE_DEBUG, "camport::setParameter, nPortIndex %ld",portIdxStruct->nPortIndex);

			OMX_PARAM_SENSORMODETYPE *pSensorMode = (OMX_PARAM_SENSORMODETYPE *)pComponentParameterStructure;
			MSG4("New sensor mode: Framerate: 0x%08lx (%ffps), Framesize: %lux%lu\n", pSensorMode->nFrameRate , ((float)pSensorMode->nFrameRate) / (1<<16) , pSensorMode->sFrameSize.nWidth, pSensorMode->sFrameSize.nHeight);
			OstTraceFiltInst4(TRACE_DEBUG, "New sensor mode: Framerate: 0x%08lx (%ffps), Framesize: %lux%lu", pSensorMode->nFrameRate , ((float)pSensorMode->nFrameRate) / (1<<16) , pSensorMode->sFrameSize.nWidth, pSensorMode->sFrameSize.nHeight);

			bOneShot = pSensorMode->bOneShot;
			COmxCamera* omxcam = (COmxCamera*)&getENSComponent();
			omxcam->mCameraUserSettings.bOneShot = bOneShot;

			OMX_STATETYPE state;
			omxcam->GetState(omxcam,&state);

			if( state == OMX_StateLoaded) {
				/* update the opmode whatever the portState and whatever OMX_ALL was set or not */
				MSG0("in loaded state\n");
				OstTraceFiltInst0(TRACE_DEBUG, "in loaded state");

				Camera* cam = (Camera*)&omxcam->getProcessingComponent();

				omxcam->mSensorMode=*(OMX_PARAM_SENSORMODETYPE*)pComponentParameterStructure;
				omxcam->mSensorMode.nPortIndex = OMX_ALL;

				if(pSensorMode->bOneShot== OMX_FALSE) cam->setOperatingMode(OpMode_Cam_VideoPreview);
				else  cam->setOperatingMode(OpMode_Cam_StillPreview);

				MSG1("bOneShot %d\n", pSensorMode->bOneShot);
				OstTraceFiltInst1(TRACE_DEBUG, "bOneShot %d", pSensorMode->bOneShot);

				error = OMX_ErrorNone;

			}
			else {
				MSG0("not in loaded state\n");
				OstTraceFiltInst0(TRACE_DEBUG, "not in loaded state");
				// in idle state all ports must be disabled and should have the same value of bOneShot on the last call of this setParameter

				camport * port0 = (camport *) omxcam->getPort(CAMERA_PORT_OUT0);
				camport * port1 = (camport *) omxcam->getPort(CAMERA_PORT_OUT1);
				camport * port2 = (camport *) omxcam->getPort(CAMERA_PORT_OUT2);


				if((port0->isEnabled() == OMX_TRUE)||(port1->isEnabled() == OMX_TRUE)||(port2->isEnabled() == OMX_TRUE))
				{
					MSG0("all port are not disabled\n");
					OstTraceFiltInst0(TRACE_DEBUG, "all port are not disabled");
					// if one of the port is enabled we dont change the current opmode, it wont be applied.
					// ENS will report an error
					// but suppose vpb2 is enabled, we would have process this function for vpb1 and vpb0.
					// that's why we do nothing in that case.
					error = OMX_ErrorNone;
				}
				else // means all of them are disabled
				{
					MSG0("all port are disabled\n");
					OstTraceFiltInst0(TRACE_DEBUG, "all port are disabled");

					// in that case all bOneShot should be the same but this has to be checked only on the last call of setParamter, means on vpb2
					if(portIdxStruct->nPortIndex == CAMERA_PORT_OUT2) {
						if((port0->bOneShot== OMX_TRUE)&&(port1->bOneShot== OMX_TRUE)&&(port2->bOneShot== OMX_TRUE)) {

							MSG0("bOneShot true for all ports, change mSensorMode and stillPathEnabled and opmode for camera component\n");
							OstTraceFiltInst0(TRACE_DEBUG, "bOneShot true for all ports, change mSensorMode and stillPathEnabled and opmode for camera component");
							Camera* cam = (Camera*)&omxcam->getProcessingComponent();
							omxcam->mSensorMode=*(OMX_PARAM_SENSORMODETYPE*)pComponentParameterStructure;
							omxcam->mSensorMode.nPortIndex = OMX_ALL;
							cam->setOperatingMode(OpMode_Cam_StillPreview);

							error = OMX_ErrorNone;

						}
						else if ((port0->bOneShot== OMX_FALSE)&&(port1->bOneShot== OMX_FALSE)&&(port2->bOneShot== OMX_FALSE)){

							MSG0("bOneShot false for all ports, change mSensorMode and stillPathEnabled and opmode for camera component\n");
							OstTraceFiltInst0(TRACE_DEBUG, "bOneShot false for all ports, change mSensorMode and stillPathEnabled and opmode for camera component");
							Camera* cam = (Camera*)&omxcam->getProcessingComponent();
							omxcam->mSensorMode=*(OMX_PARAM_SENSORMODETYPE*)pComponentParameterStructure;
							omxcam->mSensorMode.nPortIndex = OMX_ALL;
							cam->setOperatingMode(OpMode_Cam_VideoPreview);

							error = OMX_ErrorNone;
						}
						else {
							DBGT_ERROR("bOneShot ot the same for all ports, return error\n");
							OstTraceFiltInst0(TRACE_ERROR, "bOneShot ot the same for all ports, return error");
							// all port have not the same value, do not change opmode camera and return error
							// this means it has come without a OMX_ALL, or without same values for all portIndex
							error = OMX_ErrorBadPortIndex;
						}
					}
					else {
						MSG0("not vpb2, dont do anything yet\n");
						OstTraceFiltInst0(TRACE_DEBUG, "not vpb2, dont do anything yet");
						error = OMX_ErrorNone;
					}
				}
			}
			break;
		}
			
		//ER 406209	
		case OMX_STE_IndexParamFrameRateRange:
		{	

			COmxCamera* omxcam = (COmxCamera*)&getENSComponent();
			OMX_STE_PARAM_FRAMERATERANGE *portFrameRateRangeStruct = static_cast<OMX_STE_PARAM_FRAMERATERANGE *>(pComponentParameterStructure);	
			OMX_STATETYPE state;
			omxcam->GetState(omxcam,&state);
			if( state == OMX_StateLoaded) 
			{
				/* update the opmode whatever the portState and whatever OMX_ALL was set or not */
				MSG0("in loaded state\n");
				OstTraceFiltInst0(TRACE_DEBUG, "in loaded state");

				omxcam->mFrameRateRange=*(OMX_STE_PARAM_FRAMERATERANGE*)portFrameRateRangeStruct;
				omxcam->mFrameRateRange.nPortIndex = OMX_ALL;
		
				error = OMX_ErrorNone;

			}
			else 
			{
				camport * port0 = (camport *) omxcam->getPort(CAMERA_PORT_OUT0);
				camport * port1 = (camport *) omxcam->getPort(CAMERA_PORT_OUT1);
				camport * port2 = (camport *) omxcam->getPort(CAMERA_PORT_OUT2);


				if((port0->isEnabled() == OMX_TRUE)||(port1->isEnabled() == OMX_TRUE)||(port2->isEnabled() == OMX_TRUE))
				{
					MSG0("all ports are not disabled\n");
					OstTraceFiltInst0(TRACE_DEBUG, "all ports are not disabled");
					// ENS will report an error if any of the ports is enabled
					error = OMX_ErrorNone;
				}
				else
				{

					if(((((OMX_STE_PARAM_FRAMERATERANGE*)portFrameRateRangeStruct)->xFrameRateMin) < (CAM_RATE_MIN*Q16))
					|| ((((OMX_STE_PARAM_FRAMERATERANGE*)portFrameRateRangeStruct)->xFrameRateMin) >  (((OMX_STE_PARAM_FRAMERATERANGE*)portFrameRateRangeStruct)->xFrameRateMax)))
					{
						error = OMX_ErrorBadParameter;
					}
					else
					{
						omxcam->mFrameRateRange=*(OMX_STE_PARAM_FRAMERATERANGE*)portFrameRateRangeStruct;
						omxcam->mFrameRateRange.nPortIndex = OMX_ALL;
						
						MSG0("Port Settings is changed!!\n");
						port0->portSettingsChanged = OMX_TRUE;
						port1->portSettingsChanged = OMX_TRUE;
						port2->portSettingsChanged = OMX_TRUE;

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
	switch ((t_uint32)nParamIndex)
	{
		case OMX_IndexParamCommonSensorMode:
		{
			CAM_PORT_INDEX_STRUCT *portIdxStruct =
			static_cast<CAM_PORT_INDEX_STRUCT *>(pComponentParameterStructure);
			(void)portIdxStruct->nPortIndex;

			/* check that indeed it is an OMX_ALL else return an error */
			MSG1("camport::getParameter, nPortIndex %ld\n",portIdxStruct->nPortIndex);
			OstTraceFiltInst1(TRACE_DEBUG, "camport::getParameter, nPortIndex %ld",portIdxStruct->nPortIndex);

			OMX_PARAM_SENSORMODETYPE *pSensorMode = (OMX_PARAM_SENSORMODETYPE *)pComponentParameterStructure;
			COmxCamera* omxcam = (COmxCamera*)&getENSComponent();

			*pSensorMode = omxcam->mSensorMode;

			error = OMX_ErrorNone;
			break;
		}
		//ER 406209	
		case OMX_STE_IndexParamFrameRateRange:
		{
			COmxCamera* omxcam = (COmxCamera*)&getENSComponent();
			OMX_STE_PARAM_FRAMERATERANGE *portFrameRateRangeStruct =
					static_cast<OMX_STE_PARAM_FRAMERATERANGE *>(pComponentParameterStructure);
			memcpy(portFrameRateRangeStruct, &omxcam->mFrameRateRange, sizeof(OMX_STE_PARAM_FRAMERATERANGE));
			error = OMX_ErrorNone;
			break;
		}
		default :
			error = ENS_Port::getParameter(nParamIndex,pComponentParameterStructure);
			break;

	}
	return error;
}







void camport::getHwConstraints(t_uint16 portId,OMX_COLOR_FORMATTYPE omxformat, t_uint16 * p_multiple_width, t_uint16 * p_multiple_height, t_uint16 * p_multiple_stride)
{

	/* table coming from hardware specification */
	/* return value is multiple of pixels */
	switch(portId) {
		case CAMERA_PORT_OUT0: //LR Pipe
		{

			switch((t_uint32)omxformat) {
				case OMX_COLOR_FormatCbYCrY:
					*p_multiple_width = 8; 
					*p_multiple_height = 1;
					*p_multiple_stride = 2;
					break;
				case OMX_COLOR_Format16bitRGB565:
					*p_multiple_width = 8;
					*p_multiple_height = 1;
					*p_multiple_stride = 8;
					break;
				case OMX_COLOR_Format16bitARGB4444:
					*p_multiple_width = 8;
					*p_multiple_height = 1;
					*p_multiple_stride = 8;
					break;
				case OMX_COLOR_Format16bitARGB1555:
					*p_multiple_width = 8;
					*p_multiple_height = 1;
					*p_multiple_stride = 8;
					break;
				case OMX_COLOR_Format24bitRGB888:
					*p_multiple_width = 8;
					*p_multiple_height = 1;
					*p_multiple_stride = 8;
					break;
				case OMX_COLOR_Format32bitARGB8888:
					*p_multiple_width = 8;
					*p_multiple_height = 1;
					*p_multiple_stride = 8;
					break;
				default:
					DBC_ASSERT(0); //this should not be called for other omxformat
					break;
			}

	    if(((COmxCamera *)&getENSComponent())->getConfigFrameStabType().bStab)
					*p_multiple_stride = 16; 

			break;
		}
		case CAMERA_PORT_OUT1: 
		{
			switch((t_uint32)omxformat) {
				case OMX_COLOR_FormatYUV420PackedPlanar:
					*p_multiple_width = 16;
					*p_multiple_height = 2;
					*p_multiple_stride = 16;
					break;
				case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
					*p_multiple_width = 8;
					*p_multiple_height = 16;				 
					*p_multiple_stride = 16;
					break;
				default:
					*p_multiple_width = 1;
					*p_multiple_height = 1;
					*p_multiple_stride = 1;
			break;
				}
			break;
			
		}
		case CAMERA_PORT_OUT2: //HR pipe
		{

			switch((t_uint32)omxformat) {
				case OMX_COLOR_FormatYUV420PackedPlanar:
					*p_multiple_width = 16;
					*p_multiple_height = 2;
					*p_multiple_stride = 16;
					break;
				case OMX_COLOR_FormatYUV420Planar: // for backward compatibility
				case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
				case OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar:
                case OMX_STE_COLOR_FormatYUV420PackedSemiPlanar_NV21:
                case OMX_STE_COLOR_FormatYUV420PackedSemiPlanar_NV12:
					*p_multiple_width = 8;
					*p_multiple_height = 16;				 
					*p_multiple_stride = 16;
					break;
				case OMX_COLOR_FormatCbYCrY:
					*p_multiple_width = 4;
					*p_multiple_height = 1;
					*p_multiple_stride = 8;
					break;
				case OMX_SYMBIAN_COLOR_FormatRawBayer12bit:
					*p_multiple_width = 1;
					*p_multiple_height = 1;
					*p_multiple_stride = 1;
				break;
				default:
					DBC_ASSERT(0); //this should not be called for other omxformat
					break;
			}


			break;
		}
		default:
		{
			DBC_ASSERT(0); //this should not be called for other pipeId
			break;
		}

	}
	
// 	*p_multiple_stride = (*p_multiple_stride) *  getPixelDepth(omxformat);
        bool isInterLeavedFormat;
        t_uint16 stride = getStride(omxformat, (OMX_U32)1, &isInterLeavedFormat);  //calling to get value for isInterLeavedFormat
        if(isInterLeavedFormat) {
	    *p_multiple_stride = (*p_multiple_stride) *  getPixelDepth(omxformat);
         }

}



/*
 * Supported Fmt table
 */
OMX_BOOL camport::isSupportedFmt(t_uint16 portID, OMX_COLOR_FORMATTYPE omxformat)
{

	switch(portID)
	{
	case CAMERA_PORT_OUT0:
	{
		switch ((t_uint32)omxformat) {
			case OMX_COLOR_Format16bitRGB565:
			case OMX_COLOR_Format16bitARGB4444:
			case OMX_COLOR_Format16bitARGB1555:
			case OMX_COLOR_Format24bitRGB888:
			case OMX_COLOR_Format32bitARGB8888:
			case OMX_COLOR_FormatCbYCrY:
				return OMX_TRUE;
			default:
				return OMX_FALSE;
		}

	}
	case CAMERA_PORT_OUT2:
	{
		switch ((t_uint32)omxformat) {
			case OMX_COLOR_FormatCbYCrY:
			case OMX_COLOR_FormatYUV420Planar:// for backward compatibility
			case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar://CR_ZSL_YUV
			case OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar:
			case OMX_COLOR_FormatYUV420PackedPlanar:
            case OMX_STE_COLOR_FormatYUV420PackedSemiPlanar_NV21:
            case OMX_STE_COLOR_FormatYUV420PackedSemiPlanar_NV12:
				return OMX_TRUE;
			default:
				return OMX_FALSE;
		}


	}
	case CAMERA_PORT_OUT1:
	{
		switch ((t_uint32)omxformat) {
			case OMX_COLOR_FormatRawBayer8bit:
			case OMX_COLOR_FormatRawBayer8bitcompressed:
			case OMX_COLOR_FormatRawBayer10bit:
			case OMX_COLOR_FormatYUV420PackedPlanar:
			case OMX_SYMBIAN_COLOR_FormatRawBayer12bit:
			case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar://CR_ZSL_YUV
				return OMX_TRUE;
			default:
				return OMX_FALSE;
		}

	}
	default:
		DBC_ASSERT(0);//portId has been corrupted
		return OMX_FALSE;
	}
}


t_uint16 camport::getMaxRes(t_uint16 portId) {
	switch (portId) {
		case CAMERA_PORT_OUT0:
			return 1024;
		case CAMERA_PORT_OUT2:
			return 3280;
		default:
			DBC_ASSERT(0); // portId has been overriden
			return 0;
	}
}


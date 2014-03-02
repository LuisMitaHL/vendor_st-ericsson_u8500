/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "hsmcam.h"
#include "host/grabctl_types.idt.h"
#include "host/grab_types.idt.h"
#include "ImgConfig.h"
#include "camera.h"

OMX_ERRORTYPE CAM_SM::configureResolution(void)
{
    int xSize, ySize;
    camport *port;
    OMX_BOOL test = OMX_FALSE;
	Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
	
    /* configure LR pipe */
    if(((OpMode_Cam_VideoPreview == pOpModeMgr->CurrentOperatingMode)
            || (OpMode_Cam_StillPreview == pOpModeMgr->CurrentOperatingMode)
            || (OpMode_Cam_VideoRecord == pOpModeMgr->CurrentOperatingMode)
            || (OpMode_Cam_StillFaceTracking == pOpModeMgr->CurrentOperatingMode))
        && (OMX_FALSE == pSelfTest->pTesting->bEnabled))
    {
        /* get port settings */
        port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT0);

        /* check that color format is valid & configure it */
        test = port->isSupportedFmt(CAMERA_PORT_OUT0,port->getParamPortDefinition().format.video.eColorFormat);
        if (OMX_FALSE == test) {
            return OMX_ErrorBadParameter;
        }
        mIspPrivateData.Pipe1_OutputFormat = (t_uint32)convertFormatForISP(port->getParamPortDefinition().format.video.eColorFormat);
        pIspctlCom->queuePE(Pipe_1_e_OutputFormat_Pipe_Byte0,mIspPrivateData.Pipe1_OutputFormat);
        
        /* configure pipe resolutions */
        pGrabControl->getOverscanResolution(port->mInternalFrameWidth, port->mInternalFrameHeight,
                                            pGrabControl->gconvertFormatForFW(port->getParamPortDefinition().format.video.eColorFormat, false),
                                            1.0, xSize, ySize, CAMERA_PORT_OUT0);
		/*Handling LR Pipe Limitation*/
		if(xSize > XGA_WIDTH && (pGrabControl->IsStabEnabled()))
		{
			pGrabControl->rectifyOverscanResolution(port->mInternalFrameWidth,port->mInternalFrameHeight, xSize, ySize);
		}
        mIspPrivateData.Pipe1_XSize = xSize;
        mIspPrivateData.Pipe1_YSize = ySize;
        pIspctlCom->queuePE(Pipe_1_u16_X_size_Byte0, xSize);
        pIspctlCom->queuePE(Pipe_1_u16_Y_size_Byte0, ySize);
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
        mIspPrivateData.Pipe0_XSize = xSize;
        mIspPrivateData.Pipe0_YSize = ySize;
        pIspctlCom->queuePE(Pipe_0_u16_X_size_Byte0, xSize);
        pIspctlCom->queuePE(Pipe_0_u16_Y_size_Byte0, ySize);
#endif
	if(OMX_TRUE == Cam->iOpModeMgr.IsTimeNudgeEnabled())
	{
       port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
	if(port->getParamPortDefinition().format.video.eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)
	{
        /* check that color format is valid & configure it */
        test = port->isSupportedFmt(CAMERA_PORT_OUT1,port->getParamPortDefinition().format.video.eColorFormat);
        if (OMX_FALSE == test) {
            return OMX_ErrorBadParameter;
        }
        mIspPrivateData.Pipe0_OutputFormat = (t_uint32)convertFormatForISP(port->getParamPortDefinition().format.video.eColorFormat);
        pIspctlCom->queuePE(Pipe_0_e_OutputFormat_Pipe_Byte0,mIspPrivateData.Pipe0_OutputFormat);
		pGrabControl->getOverscanResolution(port->mInternalFrameWidth, port->mInternalFrameHeight,
                                                pGrabControl->gconvertFormatForFW(port->getParamPortDefinition().format.video.eColorFormat, false),
                                                1.0, xSize, ySize,CAMERA_PORT_OUT1);
		if (((COmxCamera*)&mENSComponent)->nRotation ==0){
            mIspPrivateData.Pipe0_XSize = xSize;
            mIspPrivateData.Pipe0_YSize = ySize;
            pIspctlCom->queuePE(Pipe_0_u16_X_size_Byte0, xSize);
            pIspctlCom->queuePE(Pipe_0_u16_Y_size_Byte0, ySize);
        }
        /*In case of rotation the x,y port parameters are swaped, we need to swap again for the pipe */
        else {
            mIspPrivateData.Pipe0_XSize = ySize;
            mIspPrivateData.Pipe0_YSize = xSize;
            pIspctlCom->queuePE(Pipe_0_u16_X_size_Byte0, ySize);
            pIspctlCom->queuePE(Pipe_0_u16_Y_size_Byte0, xSize);
        }
	}
	else
	{

		//Do nothing
	}
	}
    }

    /* configure HR pipe */
    if(((OpMode_Cam_VideoRecord == pOpModeMgr->CurrentOperatingMode)
            || (OpMode_Cam_StillFaceTracking == pOpModeMgr->CurrentOperatingMode)
            || (OMX_TRUE == pGrabControl->IsStabEnabled()))
        && (OMX_FALSE == pSelfTest->pTesting->bEnabled))
    {
        /* get port settings */
        port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT2);

        /* check that color format is valid & configure it */
        test = port->isSupportedFmt(CAMERA_PORT_OUT2,port->getParamPortDefinition().format.video.eColorFormat);
        if (OMX_FALSE == test) {
            return OMX_ErrorBadParameter;
        }
        mIspPrivateData.Pipe0_OutputFormat = (t_uint32)convertFormatForISP(port->getParamPortDefinition().format.video.eColorFormat);
        pIspctlCom->queuePE(Pipe_0_e_OutputFormat_Pipe_Byte0,mIspPrivateData.Pipe0_OutputFormat);

        /* configure pipe resolutions */
        /* get LR pipe settings to configure HR pipe resolution for stab in preview only */
        if ((OMX_TRUE == pGrabControl->IsStabEnabled()) &&
            (OpMode_Cam_VideoPreview == pOpModeMgr->CurrentOperatingMode))
        {
            camport * portVF = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT0);
            /* portRecord color formats have stronger resolution constraints. Don't use portVF settings for gconvertFormatForFW */
            pGrabControl->getOverscanResolution(portVF->mInternalFrameWidth, portVF->mInternalFrameHeight,
                                                pGrabControl->gconvertFormatForFW(port->getParamPortDefinition().format.video.eColorFormat, false),
                                                1.0, xSize, ySize,CAMERA_PORT_OUT0);
			if(xSize > XGA_WIDTH && (pGrabControl->IsStabEnabled()))
			{
				pGrabControl->rectifyOverscanResolution(portVF->mInternalFrameWidth,portVF->mInternalFrameHeight, xSize, ySize);
			}	
			
        }
        /* no stab */
        else {
            pGrabControl->getOverscanResolution(port->mInternalFrameWidth, port->mInternalFrameHeight,
                                                pGrabControl->gconvertFormatForFW(port->getParamPortDefinition().format.video.eColorFormat, false),
                                                1.0, xSize, ySize,CAMERA_PORT_OUT2);
        }
        if (((COmxCamera*)&mENSComponent)->nRotation ==0){
            mIspPrivateData.Pipe0_XSize = xSize;
            mIspPrivateData.Pipe0_YSize = ySize;
            pIspctlCom->queuePE(Pipe_0_u16_X_size_Byte0, xSize);
            pIspctlCom->queuePE(Pipe_0_u16_Y_size_Byte0, ySize);
        }
        /*In case of rotation the x,y port parameters are swaped, we need to swap again for the pipe */
        else {
            mIspPrivateData.Pipe0_XSize = ySize;
            mIspPrivateData.Pipe0_YSize = xSize;
            pIspctlCom->queuePE(Pipe_0_u16_X_size_Byte0, ySize);
            pIspctlCom->queuePE(Pipe_0_u16_Y_size_Byte0, xSize);
        }
    }

    /* configure BMS pipe */
    /* in time nudge size is chosen by FW (since LR is streaming) => nothing to do */
    if ((OpMode_Cam_StillCaptureSingle == pOpModeMgr->CurrentOperatingMode)
        || (OpMode_Cam_StillCaptureBurstLimited == pOpModeMgr->CurrentOperatingMode)
        || (OpMode_Cam_StillCaptureBurstInfinite == pOpModeMgr->CurrentOperatingMode)
        || (OpMode_Cam_StillCaptureSingleInVideo == pOpModeMgr->CurrentOperatingMode)
        || (OpMode_Cam_StillCaptureBurstLimitedInVideo == pOpModeMgr->CurrentOperatingMode)
        || (OpMode_Cam_StillCaptureBurstInfiniteInVideo == pOpModeMgr->CurrentOperatingMode)
        || (OMX_TRUE == pSelfTest->pTesting->bEnabled))
    {
        /* get port settings */
        port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);

        /* check that color format is valid */
        test = port->isSupportedFmt(CAMERA_PORT_OUT1,port->getParamPortDefinition().format.video.eColorFormat);
        if (OMX_FALSE == test) {
            return OMX_ErrorBadParameter;
        }

        /* configure pipe resolutions */
        pGrabControl->getOverscanResolution(port->mInternalFrameWidth, port->mInternalFrameHeight,
                                            pGrabControl->gconvertFormatForFW(port->getParamPortDefinition().format.video.eColorFormat, false),
                                            1.0, xSize, ySize,CAMERA_PORT_OUT1);

	if(port->getParamPortDefinition().format.video.eColorFormat != (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)
	{
        mIspPrivateData.Pipe0_XSize = port->mInternalFrameWidth;
        mIspPrivateData.Pipe0_YSize = port->mInternalFrameHeight;
       	 pIspctlCom->queuePE(Pipe_RAW_u16_output_res_X_size_Byte0, xSize);
	        pIspctlCom->queuePE(Pipe_RAW_u16_output_res_Y_size_Byte0, ySize - pSensor->getNberOfNonActiveLines());
       	 pIspctlCom->queuePE(Pipe_RAW_u16_woi_res_X_Byte0, xSize);
	        pIspctlCom->queuePE(Pipe_RAW_u16_woi_res_Y_Byte0, ySize - pSensor->getNberOfNonActiveLines());
	}
	else
	{
		if (((COmxCamera*)&mENSComponent)->nRotation ==0){
                mIspPrivateData.Pipe0_XSize = xSize;
                mIspPrivateData.Pipe0_YSize = ySize;
            	pIspctlCom->queuePE(Pipe_0_u16_X_size_Byte0, xSize);
            	pIspctlCom->queuePE(Pipe_0_u16_Y_size_Byte0, ySize);
        	}
        /*In case of rotation the x,y port parameters are swaped, we need to swap again for the pipe */
        	else {
                mIspPrivateData.Pipe0_XSize = ySize;
                mIspPrivateData.Pipe0_YSize = xSize;
            	pIspctlCom->queuePE(Pipe_0_u16_X_size_Byte0, ySize);
            	pIspctlCom->queuePE(Pipe_0_u16_Y_size_Byte0, xSize);
        	}
	}
		
    }

    port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT0);
    port->portSettingsChanged = OMX_FALSE;
    port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT2);
    port->portSettingsChanged = OMX_FALSE;
    port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
    port->portSettingsChanged = OMX_FALSE;

    return (OMX_ErrorNone);
}


OMX_ERRORTYPE CAM_SM::configurePipes(void)
{
	Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
	camport *port;
    /* select source */
    if (pSensor->GetCameraSlot() == ePrimaryCamera) pIspctlCom->queuePE(SystemSetup_e_InputImageSource_Byte0, (t_uint32)InputImageSource_e_Sensor0);
    if (pSensor->GetCameraSlot() == eSecondaryCamera) pIspctlCom->queuePE(SystemSetup_e_InputImageSource_Byte0, (t_uint32)InputImageSource_e_Sensor1);

    pIspctlCom->queuePE(HostFrameConstraints_e_FrameDimensionProgMode_Byte0, (t_uint32)FrameDimensionProgMode_e_Auto);

    /* BMS0 only */
    if ((OMX_TRUE == pSelfTest->pTesting->bEnabled) && (OMX_SYMBIAN_CameraSelfTestSensor == pSelfTest->currentSelftest))
    {
        /* disable LR pipe */
        mIspPrivateData.Pipe1_Enable = (t_uint32)Flag_e_FALSE;
        pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe1Enable_Byte0, (t_uint32)Flag_e_FALSE);

        /* disable HR pipe */
        mIspPrivateData.Pipe0_Enable = (t_uint32)Flag_e_FALSE;
        pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe0Enable_Byte0, (t_uint32)Flag_e_FALSE);

        /* enable BMS pipe : BMS0 */
        pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, (t_uint32)Flag_e_TRUE);
        pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, (t_uint32)Flag_e_FALSE);
        pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, (t_uint32)Flag_e_FALSE);

        /* The fast capture case is dedicated to selt tests */
        /* => no need to program Image Quality related ISP blocks */
    }

    /* BMS2 only *//*Case for Still Capture for both YUV and RAW*/
    else if ((OpMode_Cam_StillCaptureSingle == pOpModeMgr->CurrentOperatingMode)
        || (OpMode_Cam_StillCaptureBurstLimited == pOpModeMgr->CurrentOperatingMode)
        || (OpMode_Cam_StillCaptureBurstInfinite == pOpModeMgr->CurrentOperatingMode)
        || (OpMode_Cam_StillCaptureSingleInVideo == pOpModeMgr->CurrentOperatingMode)
        || (OpMode_Cam_StillCaptureBurstLimitedInVideo == pOpModeMgr->CurrentOperatingMode)
        || (OpMode_Cam_StillCaptureBurstInfiniteInVideo == pOpModeMgr->CurrentOperatingMode))
    {
         port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
		 /*1. Still +YUV
		     2.Still + Raw*/
         if(port->getParamPortDefinition().format.video.eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)
         {
         	 /*Enable HR for YUV+Still*/
            	pIspctlCom->queuePE(Pipe_0_e_Flag_TogglePixValid_Byte0, (t_uint32)Flag_e_FALSE);
                mIspPrivateData.Pipe0_Enable = (t_uint32)Flag_e_TRUE;
            	pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe0Enable_Byte0, (t_uint32)Flag_e_TRUE);
		/*Disable BMS*/			
		pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, (t_uint32)Flag_e_FALSE);
            	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, (t_uint32)Flag_e_FALSE);
            	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, (t_uint32)Flag_e_FALSE);

		/* program image quality blocks */
	       /* RSO = channel offset */
       	pIspctlCom->queuePE(RSO_Control_e_Flag_EnableRSO_Byte0, (t_uint32)Flag_e_TRUE);
	       pIspctlCom->queuePE(RSO_Control_e_RSO_Mode_Control_Byte0, (t_uint32)RSO_Mode_e_Manual);
       	pIspDampers->RsoControl(ISP_DAMPERS_ENABLE);
	       /* linearization = SDL */
       	pIspctlCom->queuePE(SDL_Control_e_SDLMode_Control_Byte0, (t_uint32)SDL_Custom);
	       /* LSC = Lens Shading Correction = gridiron */
       	pIspctlCom->queuePE(GridironControl_e_Flag_Enable_Byte0, (t_uint32)Flag_e_TRUE);
	       /* digital gain = channel gains */
       	pIspctlCom->queuePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0, (t_uint32)Flag_e_TRUE);
	       /* green imbalance = scorpio */
       	/*Scorpio enabled*/
	       pIspctlCom->queuePE(Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0, (t_uint32)Flag_e_TRUE);
       	pIspctlCom->queuePE(Scorpio_Ctrl_e_ScorpioMode_Byte0, (t_uint32)ScorpioMode_e_Manual);
	       pIspDampers->ScorpioControl(ISP_DAMPERS_ENABLE);
       	/* noise correction = arctic = duster (noise filtering + defect correction) */
	       pIspctlCom->queuePE(DusterControl_e_Flag_DusterEnable_Byte0, (t_uint32)Flag_e_TRUE);
	        pIspctlCom->queuePE(DusterControl_u16_Duster_ByPass_Ctrl_Byte0, 0x140);
	        /* duster_rc_enablegrad | bypass_scythe */
       	 pIspDampers->DefCorControl(ISP_DAMPERS_ENABLE);
	        pIspDampers->NoiseFilterControl(ISP_DAMPERS_ENABLE);
       	 pIspDampers->NoiseModelControl(ISP_DAMPERS_ENABLE);
	        pIspDampers->ScytheControl(ISP_DAMPERS_ENABLE);
       	 /* binning repair */
       	 pIspctlCom->queuePE(BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0, (t_uint32)Flag_e_FALSE);
	        /* Sharpening / Peaking = adsoc */
       	 pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_TRUE);
	        pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Byte0, (t_uint32)Flag_e_FALSE);
	        pIspDampers->SharpeningHrControl(ISP_DAMPERS_ENABLE);
	        pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_TRUE);
	        pIspDampers->SharpeningLrControl(ISP_DAMPERS_ENABLE);
         }
	 else
 	{
        /* disable LR pipe */
        mIspPrivateData.Pipe1_Enable = (t_uint32)Flag_e_FALSE;
        pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe1Enable_Byte0, (t_uint32)Flag_e_FALSE);

        /* disable HR pipe */
        mIspPrivateData.Pipe0_Enable = (t_uint32)Flag_e_FALSE;
         pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe0Enable_Byte0, (t_uint32)Flag_e_FALSE);
		/*Raw+Still
          enable BMS pipe : RAW Image Processed == Ideal Raw ==  BMS2 + some blocks enabled 
		*/
        pIspctlCom->queuePE(DataPathControl_e_BayerStore2Source_Byte0, (t_uint32)BayerStore2Source_e_BayerCrop);
        pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, (t_uint32)Flag_e_FALSE);
        pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, (t_uint32)Flag_e_FALSE);
        pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, (t_uint32)Flag_e_TRUE);

        /* program image quality blocks */
        if (ePreset==OMX_SYMBIAN_RawImageUnprocessed){
            /* RSO = channel offset */
            pIspctlCom->queuePE(RSO_Control_e_Flag_EnableRSO_Byte0, (t_uint32)Flag_e_FALSE);
            pIspDampers->RsoControl(ISP_DAMPERS_DISABLE);
            /* linearization = SDL */
            pIspctlCom->queuePE(SDL_Control_e_SDLMode_Control_Byte0, (t_uint32)SDL_Disable);
            /* LSC = Lens Shading Correction = gridiron */
            pIspctlCom->queuePE(GridironControl_e_Flag_Enable_Byte0, (t_uint32)Flag_e_FALSE);
            /* digital gain = channel gains */
            pIspctlCom->queuePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0, (t_uint32)Flag_e_FALSE);
            /* green imbalance = scorpio */
            pIspctlCom->queuePE(Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0, (t_uint32)Flag_e_FALSE);
            pIspDampers->ScorpioControl(ISP_DAMPERS_DISABLE);
            /* noise correction = arctic = duster (noise filtering + defect correction) */
            pIspctlCom->queuePE(DusterControl_e_Flag_DusterEnable_Byte0, (t_uint32)Flag_e_FALSE);
            pIspctlCom->queuePE(DusterControl_u16_Duster_ByPass_Ctrl_Byte0, 0x258);
            pIspDampers->DefCorControl(ISP_DAMPERS_DISABLE);
            pIspDampers->NoiseFilterControl(ISP_DAMPERS_DISABLE);
            pIspDampers->NoiseModelControl(ISP_DAMPERS_DISABLE);
            pIspDampers->ScytheControl(ISP_DAMPERS_DISABLE);
            /* binning repair */
            pIspctlCom->queuePE(BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0, (t_uint32)Flag_e_FALSE);
            /* Sharpening / Peaking */
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_FALSE);
            pIspDampers->SharpeningHrControl(ISP_DAMPERS_DISABLE);
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_FALSE);
            pIspDampers->SharpeningLrControl(ISP_DAMPERS_DISABLE);
        }
        else if (ePreset==OMX_SYMBIAN_RawImageProcessed){
            /* RSO = channel offset */
            pIspctlCom->queuePE(RSO_Control_e_Flag_EnableRSO_Byte0, (t_uint32)Flag_e_TRUE);
            pIspctlCom->queuePE(RSO_Control_e_RSO_Mode_Control_Byte0, (t_uint32)RSO_Mode_e_Manual);
            pIspDampers->RsoControl(ISP_DAMPERS_ENABLE);
            /* linearization = SDL */
            pIspctlCom->queuePE(SDL_Control_e_SDLMode_Control_Byte0, (t_uint32)SDL_Custom);
            /* LSC = Lens Shading Correction = gridiron */
            /* Gridiron enabled*/
            pIspctlCom->queuePE(GridironControl_e_Flag_Enable_Byte0, (t_uint32)Flag_e_TRUE);
            /* digital gain = channel gains */
            pIspctlCom->queuePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0, (t_uint32)Flag_e_FALSE);
            /* green imbalance = scorpio */
            pIspctlCom->queuePE(Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0, (t_uint32)Flag_e_FALSE);
            pIspDampers->ScorpioControl(ISP_DAMPERS_DISABLE);
            /* noise correction = arctic = duster (noise filtering + defect correction) */
            pIspctlCom->queuePE(DusterControl_e_Flag_DusterEnable_Byte0, (t_uint32)Flag_e_TRUE);
            pIspctlCom->queuePE(DusterControl_u16_Duster_ByPass_Ctrl_Byte0, 0x50);
            /* duster_gaussian_bypass | duster_rc_enablegrad | bypass_scythe */
            pIspDampers->NoiseModelControl(ISP_DAMPERS_ENABLE);
            pIspDampers->DefCorControl(ISP_DAMPERS_ENABLE);
            pIspDampers->NoiseFilterControl(ISP_DAMPERS_DISABLE);
            pIspDampers->ScytheControl(ISP_DAMPERS_ENABLE);
            /* binning repair */
            pIspctlCom->queuePE(BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0, (t_uint32)Flag_e_FALSE);
            /* Sharpening / Peaking = adsoc */
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_FALSE);
            pIspDampers->SharpeningHrControl(ISP_DAMPERS_DISABLE);
            pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_FALSE);
            pIspDampers->SharpeningLrControl(ISP_DAMPERS_DISABLE);
		}
	 	}
    }

    /* LR, possibly HR, possibly BMS2 (processed or unprocessed) *//*basically ZSL both Raw and YUV*/
    else
    {
	
		OMX_PARAM_PORTDEFINITIONTYPE paramPortDefinition;
		paramPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
		getOmxIlSpecVersion(&paramPortDefinition.nVersion);
		mENSComponent.getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&paramPortDefinition);
        /* enable LR pipe */
        pIspctlCom->queuePE(Pipe_1_e_Flag_TogglePixValid_Byte0, (t_uint32)Flag_e_FALSE);
        mIspPrivateData.Pipe1_Enable = (t_uint32)Flag_e_TRUE;
        pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe1Enable_Byte0, (t_uint32)Flag_e_TRUE);

        /* enable HR pipe */
        if((pOpModeMgr->CurrentOperatingMode == OpMode_Cam_VideoRecord)
            ||(pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillFaceTracking)
            || (OMX_TRUE == pGrabControl->IsStabEnabled())) {
            pIspctlCom->queuePE(Pipe_0_e_Flag_TogglePixValid_Byte0, (t_uint32)Flag_e_FALSE);
            mIspPrivateData.Pipe0_Enable = (t_uint32)Flag_e_TRUE;
            pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe0Enable_Byte0, (t_uint32)Flag_e_TRUE);
        }
        else {
            mIspPrivateData.Pipe0_Enable = (t_uint32)Flag_e_FALSE;
            pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe0Enable_Byte0, (t_uint32)Flag_e_FALSE);
        }

        /* enable BMS pipe (only in time nudge)&& RAW for Time Nudge and YUV disable BMS and Enable HR */
        if (OMX_TRUE == Cam->iOpModeMgr.IsTimeNudgeEnabled() && !(Cam->mDisplay720p&0x01))
        {
        	port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
        	if(port->getParamPortDefinition().format.video.eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)
        	{
        	/*Enable HR for YUV+ZSL*/
            		pIspctlCom->queuePE(Pipe_0_e_Flag_TogglePixValid_Byte0, (t_uint32)Flag_e_FALSE);
                    mIspPrivateData.Pipe0_Enable = (t_uint32)Flag_e_TRUE;
            		pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe0Enable_Byte0, (t_uint32)Flag_e_TRUE);
		/*Disable BMS*/			
			pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, (t_uint32)Flag_e_FALSE);
            		pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, (t_uint32)Flag_e_FALSE);
            		pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, (t_uint32)Flag_e_FALSE);
        	}
		else
		{
            /* BMS2 */
            /* no need to configure size : we will have one of current sensor mode */
		/*Enable BMS for ZSL-RAW, HR may or may not be started*/
            	pIspctlCom->queuePE(DataPathControl_e_BayerStore2Source_Byte0, (t_uint32)BayerStore2Source_e_BayerCrop);
            	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, (t_uint32)Flag_e_FALSE);
            	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, (t_uint32)Flag_e_FALSE);
            	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, (t_uint32)Flag_e_TRUE);
		/*Disable HR pipe for Sure*/
        mIspPrivateData.Pipe0_Enable = (t_uint32)Flag_e_FALSE;
		pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe0Enable_Byte0, (t_uint32)Flag_e_FALSE);
		}
        }
		else if(((pOpModeMgr->CurrentOperatingMode == OpMode_Cam_VideoRecord||pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillFaceTracking )&&(paramPortDefinition.bEnabled==OMX_TRUE))|| (Cam->mDisplay720p&0x01) )
		{
			/* BMS2 */
            /* no need to configure size : we will have one of current sensor mode */
			/*Enable BMS for Still During Video */						        			
            	pIspctlCom->queuePE(DataPathControl_e_BayerStore2Source_Byte0, (t_uint32) BayerStore2Source_e_BayerCrop);
            	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, (t_uint32)Flag_e_FALSE);
            	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, (t_uint32)Flag_e_FALSE);
            	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, (t_uint32)Flag_e_TRUE);
		}
        else
        {
        /*Case of Only Stab/Video Record/Still Face Tracking*/
            pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, (t_uint32)Flag_e_FALSE);
            pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, (t_uint32)Flag_e_FALSE);
            pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, (t_uint32)Flag_e_FALSE);
        }

        /* program image quality blocks */
        /* RSO = channel offset */
        pIspctlCom->queuePE(RSO_Control_e_Flag_EnableRSO_Byte0, (t_uint32)Flag_e_TRUE);
        pIspctlCom->queuePE(RSO_Control_e_RSO_Mode_Control_Byte0, (t_uint32)RSO_Mode_e_Manual);
        pIspDampers->RsoControl(ISP_DAMPERS_ENABLE);
        /* linearization = SDL */
        pIspctlCom->queuePE(SDL_Control_e_SDLMode_Control_Byte0, (t_uint32)SDL_Custom);
        /* LSC = Lens Shading Correction = gridiron */
        pIspctlCom->queuePE(GridironControl_e_Flag_Enable_Byte0, (t_uint32)Flag_e_TRUE);
        /* digital gain = channel gains */
        pIspctlCom->queuePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0, (t_uint32)Flag_e_TRUE);
        /* green imbalance = scorpio */
        /*Scorpio enabled*/
        pIspctlCom->queuePE(Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0, (t_uint32)Flag_e_TRUE);
        pIspctlCom->queuePE(Scorpio_Ctrl_e_ScorpioMode_Byte0, (t_uint32)ScorpioMode_e_Manual);
        pIspDampers->ScorpioControl(ISP_DAMPERS_ENABLE);
        /* noise correction = arctic = duster (noise filtering + defect correction) */
        pIspctlCom->queuePE(DusterControl_e_Flag_DusterEnable_Byte0, (t_uint32)Flag_e_TRUE);
        pIspctlCom->queuePE(DusterControl_u16_Duster_ByPass_Ctrl_Byte0, 0x140);
        /* duster_rc_enablegrad | bypass_scythe */
        pIspDampers->DefCorControl(ISP_DAMPERS_ENABLE);
        pIspDampers->NoiseFilterControl(ISP_DAMPERS_ENABLE);
        pIspDampers->NoiseModelControl(ISP_DAMPERS_ENABLE);
        pIspDampers->ScytheControl(ISP_DAMPERS_ENABLE);
        /* binning repair */
        pIspctlCom->queuePE(BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0, (t_uint32)Flag_e_FALSE);
        /* Sharpening / Peaking = adsoc */
        pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_TRUE);
        pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Byte0, (t_uint32)Flag_e_FALSE);
        pIspDampers->SharpeningHrControl(ISP_DAMPERS_ENABLE);
        pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_TRUE);
        pIspDampers->SharpeningLrControl(ISP_DAMPERS_ENABLE);

#if 0 // FIXME Enable this when FW API finalized
        /* Apply YUV range settings to LR pipe */
        COmxCamera* pOmxCamera = (COmxCamera*)&mENSComponent;
        Transform_te eTransformType = colorPrimary2TransformType(pOmxCamera->mColorPrimary_VPB0.eColorPrimary);
        pIspctlCom->writePE(CE_YUVCoderControls_1_e_Transform_Type_Byte0, eTransformType);
#endif
    }
    return (OMX_ErrorNone);
}


// obsolete
OMX_ERRORTYPE CAM_SM::configureIspResolution(t_uint8 portId) {
    return (OMX_ErrorNone);
}


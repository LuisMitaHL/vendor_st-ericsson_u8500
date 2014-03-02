/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "hsmispproc.h"
#include "ispprocport.h"
#include "VhcElementDefs.h"
#include "tuning_params.h"
#include "ispproc.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "ispproc_src_isp_configuring_pipeTraces.h"
#endif

OMX_ERRORTYPE ISPPROC_SM::configureResolution(void)
{
    return (OMX_ErrorNone);
}

OMX_ERRORTYPE ISPPROC_SM::configurePipes(void)
{
	/* Can be modified and remain as is because it's set after in the Camera */
	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, (t_uint32)Flag_e_FALSE);
	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, (t_uint32)Flag_e_FALSE);
	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, (t_uint32)Flag_e_FALSE);
	pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe0Enable_Byte0, (t_uint32)Flag_e_TRUE);
	pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe1Enable_Byte0, (t_uint32)Flag_e_TRUE);
	pIspctlCom->queuePE(HostFrameConstraints_e_FrameDimensionProgMode_Byte0, (t_uint32)FrameDimensionProgMode_e_Manual);
	pIspctlCom->queuePE(SystemSetup_e_InputImageSource_Byte0, (t_uint32)InputImageSource_e_BayerLoad1);

	if (ConfiguringISPPipe_TimeNudgeBML == ConfiguringISPPipe_ConfigType)
	{
		MSG0("ISPPROC_SM::configurePipes : time nudge BML\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "ISPPROC_SM::configurePipes : time nudge BML", (&mENSComponent));
		/* RSO = channel offset */
		pIspctlCom->queuePE(RSO_Control_e_Flag_EnableRSO_Byte0, (t_uint32)Flag_e_FALSE);
		pIspDampers->RsoControl(ISP_DAMPERS_DISABLE);
		/* linearization = SDL */
		pIspctlCom->queuePE(SDL_Control_e_SDLMode_Control_Byte0, (t_uint32)SDL_Disable);
		/* LSC = Lens Shading Correction = gridiron */
		pIspctlCom->queuePE(GridironControl_e_Flag_Enable_Byte0, (t_uint32)Flag_e_FALSE);
		/* digital gain = channel gains */
		pIspctlCom->queuePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0,(t_uint32) Flag_e_FALSE);
		/* green imbalance = scorpio */
		pIspctlCom->queuePE(Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0, (t_uint32)Flag_e_FALSE);
		pIspDampers->ScorpioControl(ISP_DAMPERS_DISABLE);
		/* noise correction = arctic = duster (noise filtering + defect correction) */
		pIspctlCom->queuePE(DusterControl_e_Flag_DusterEnable_Byte0, (t_uint32)Flag_e_FALSE);
		pIspDampers->NoiseModelControl(ISP_DAMPERS_DISABLE);
		pIspDampers->DefCorControl(ISP_DAMPERS_DISABLE);
		pIspDampers->NoiseFilterControl(ISP_DAMPERS_DISABLE);
		pIspDampers->ScytheControl(ISP_DAMPERS_DISABLE);
		/* binning repair */
		pIspctlCom->queuePE(BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0, (t_uint32)Flag_e_FALSE);
		/* Sharpening / Peaking = adsoc */
		pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_TRUE);
		pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_TRUE);
		pIspDampers->SharpeningLrControl(ISP_DAMPERS_ENABLE);
		pIspDampers->SharpeningHrControl(ISP_DAMPERS_ENABLE);
	}
	else
	{
		MSG0("ISPPROC_SM::configurePipes : standard BML\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "ISPPROC_SM::configurePipes : standard BML", (&mENSComponent));
		MSG1("pPreset==%d\n",pPreset);
		OstTraceFiltStatic1 (TRACE_DEBUG, "pPreset==%d", (&mENSComponent),pPreset);
		/* Configure IQ related blocks */
		if (pPreset==OMX_SYMBIAN_RawImageProcessed)
		{
			MSG0("In pPreset == OMX_SYMBIAN_RawImageProcessed\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "In pPreset == OMX_SYMBIAN_RawImageProcessed", (&mENSComponent));
			/* RSO = channel offset */
			pIspctlCom->queuePE(RSO_Control_e_Flag_EnableRSO_Byte0, (t_uint32)Flag_e_FALSE);
			pIspDampers->RsoControl(ISP_DAMPERS_DISABLE);
			/* linearization = SDL */
			pIspctlCom->queuePE(SDL_Control_e_SDLMode_Control_Byte0, (t_uint32)SDL_Disable);
			/* LSC = Lens Shading Correction = gridiron */
			pIspctlCom->queuePE(GridironControl_e_Flag_Enable_Byte0, (t_uint32)Flag_e_FALSE);
			/* digital gain = channel gains */
			pIspctlCom->queuePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0, (t_uint32)Flag_e_TRUE);
			/* green imbalance = scorpio */
			pIspctlCom->queuePE(Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0, (t_uint32)Flag_e_TRUE);
			pIspDampers->ScorpioControl(ISP_DAMPERS_ENABLE);
			/* noise correction = arctic = duster (noise filtering + defect correction) */
			/* noise correction = arctic = duster: enable gaussian only */
			pIspctlCom->queuePE(DusterControl_e_Flag_DusterEnable_Byte0, (t_uint32)Flag_e_TRUE);
			pIspctlCom->queuePE(DusterControl_u16_Duster_ByPass_Ctrl_Byte0, 0x348);
			/* duster_defcor_bypass | bypass_scythe */
			pIspDampers->NoiseModelControl(ISP_DAMPERS_DISABLE);
			pIspDampers->DefCorControl(ISP_DAMPERS_DISABLE);
			pIspDampers->NoiseFilterControl(ISP_DAMPERS_ENABLE);
			pIspDampers->ScytheControl(ISP_DAMPERS_DISABLE);
			/* binning repair */
			pIspctlCom->queuePE(BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0, (t_uint32)Flag_e_FALSE);
			/* Sharpening / Peaking = adsoc */
			pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_TRUE);
			pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_TRUE);
			pIspDampers->SharpeningLrControl(ISP_DAMPERS_ENABLE);
			pIspDampers->SharpeningHrControl(ISP_DAMPERS_ENABLE);
		}
		else if (pPreset == OMX_SYMBIAN_RawImageUnprocessed)
		{
			MSG0("In pPreset == OMX_SYMBIAN_RawImageUnprocessed\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "In pPreset == OMX_SYMBIAN_RawImageUnprocessed", (&mENSComponent));
			/* RSO = channel offset */
			pIspctlCom->queuePE(RSO_Control_e_Flag_EnableRSO_Byte0, (t_uint32)Flag_e_TRUE);
			pIspctlCom->queuePE(RSO_Control_e_RSO_Mode_Control_Byte0, (t_uint32)RSO_Mode_e_Manual);
			pIspDampers->RsoControl(ISP_DAMPERS_ENABLE);
			/* linearization = SDL */
			pIspctlCom->queuePE(SDL_Control_e_SDLMode_Control_Byte0,(t_uint32) SDL_Custom);
			/* LSC = Lens Shading Correction = gridiron */
			/*Gridiron enabled*/
			pIspctlCom->queuePE(GridironControl_e_Flag_Enable_Byte0, (t_uint32)Flag_e_TRUE);
			/* digital gain = channel gains */
			pIspctlCom->queuePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0, (t_uint32)Flag_e_TRUE);
			/* green imbalance = scorpio */
			pIspctlCom->queuePE(Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0, (t_uint32)Flag_e_TRUE);
			pIspDampers->ScorpioControl(ISP_DAMPERS_ENABLE);
			/* noise correction = arctic = duster (noise filtering + defect correction) */
			pIspctlCom->queuePE(DusterControl_e_Flag_DusterEnable_Byte0, (t_uint32)Flag_e_TRUE);
			pIspctlCom->queuePE(DusterControl_u16_Duster_ByPass_Ctrl_Byte0, 0x140);
			/* duster_rc_enablegrad | bypass_scythe */
			pIspDampers->DefCorControl(ISP_DAMPERS_ENABLE);
			pIspDampers->NoiseModelControl(ISP_DAMPERS_ENABLE);
			pIspDampers->NoiseFilterControl(ISP_DAMPERS_DISABLE);
			pIspDampers->ScytheControl(ISP_DAMPERS_DISABLE);
			/* binning repair */
			pIspctlCom->queuePE(BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0, (t_uint32)Flag_e_FALSE);
			/* Sharpening / Peaking = adsoc */
			pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_TRUE);
			pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_TRUE);
			pIspDampers->SharpeningLrControl(ISP_DAMPERS_ENABLE);
			pIspDampers->SharpeningHrControl(ISP_DAMPERS_ENABLE);
		}
	}

#if 0 // FIXME Enable this when FW API finalized
	/* Apply YUV range default settings to LR and HR pipes */
	Transform_te eTransformType = colorPrimary2TransformType(OMX_SYMBIAN_ColorPrimaryBT601FullRange);
	pIspctlCom->writePE(CE_YUVCoderControls_0_e_Transform_Type_Byte0, eTransformType);
	pIspctlCom->writePE(CE_YUVCoderControls_1_e_Transform_Type_Byte0, eTransformType);
#endif

    /* Configure resolutions */
    OMX_ERRORTYPE error = OMX_ErrorUndefined;
    error = configureIspResolution(ISPPROC_PORT_OUT0);
    if (OMX_ErrorNone != error) {
        return error;
    }
    error = configureIspResolution(ISPPROC_PORT_OUT1);
    if (OMX_ErrorNone != error) {
        return error;
    }

    return (OMX_ErrorNone);
}



OMX_ERRORTYPE ISPPROC_SM::configureIspResolution(t_uint8 portId)
{
    OMX_BOOL test = OMX_FALSE;
    ispprocport* port = (ispprocport*)mENSComponent.getPort(portId);

    if(portId==ISPPROC_PORT_OUT0){
        test = port->isSupportedFmt(ISPPROC_PORT_OUT0,port->getParamPortDefinition().format.video.eColorFormat);
        if (OMX_FALSE == test) {
            return OMX_ErrorBadParameter;
        }
        pIspctlCom->queuePE(Pipe_1_u16_X_size_Byte0, port->mInternalFrameWidth);
        pIspctlCom->queuePE(Pipe_1_u16_Y_size_Byte0, port->mInternalFrameHeight);
        pIspctlCom->queuePE(Pipe_1_e_OutputFormat_Pipe_Byte0,(t_uint32)convertFormatForISP(port->getParamPortDefinition().format.video.eColorFormat));
        pIspctlCom->queuePE(Pipe_1_e_Flag_TogglePixValid_Byte0,0);
    }
    else if (portId==ISPPROC_PORT_OUT1)
    {
		if(((*pRotation == OMX_SYMBIAN_OrientationRowRightColumnTop) || (*pRotation == OMX_SYMBIAN_OrientationRowLeftColumnBottom)) && (*pAutoRotation == OMX_TRUE))
        {
            test = port->isSupportedFmt(ISPPROC_PORT_OUT1,port->getParamPortDefinition().format.video.eColorFormat);
            if (OMX_FALSE == test) {
                return OMX_ErrorBadParameter;
            }
            pIspctlCom->queuePE(Pipe_0_u16_X_size_Byte0, port->mInternalFrameHeight);
            pIspctlCom->queuePE(Pipe_0_u16_Y_size_Byte0, port->mInternalFrameWidth);
            pIspctlCom->queuePE(Pipe_0_e_OutputFormat_Pipe_Byte0,(t_uint32)convertFormatForISP(port->getParamPortDefinition().format.video.eColorFormat));
            pIspctlCom->queuePE(Pipe_0_e_Flag_TogglePixValid_Byte0,0);
        }
        else
        {
            test = port->isSupportedFmt(ISPPROC_PORT_OUT1,port->getParamPortDefinition().format.video.eColorFormat);
            if (OMX_FALSE == test) {
                return OMX_ErrorBadParameter;
            }
            pIspctlCom->queuePE(Pipe_0_u16_X_size_Byte0, port->mInternalFrameWidth);
            pIspctlCom->queuePE(Pipe_0_u16_Y_size_Byte0, port->mInternalFrameHeight);
            pIspctlCom->queuePE(Pipe_0_e_OutputFormat_Pipe_Byte0,(t_uint32)convertFormatForISP(port->getParamPortDefinition().format.video.eColorFormat));
            pIspctlCom->queuePE(Pipe_0_e_Flag_TogglePixValid_Byte0,0);
        }
    }
    else {
        return (OMX_ErrorUndefined);
    }
    return (OMX_ErrorNone);
}

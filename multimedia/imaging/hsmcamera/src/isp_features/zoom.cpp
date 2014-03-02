/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


//#undef OMXCOMPONENT
//#define OMXCOMPONENT "ZOOM"
#define DBGT_PREFIX "ZOOM"


#include "zoom.h"

CZoom::CZoom(COmxCamera* pOMXCamera, OMX_PARAM_SENSORMODETYPE* pSensorMde)
{
	pOMXCam = pOMXCamera;
	pSensorMode = pSensorMde;
	
	/* init zoom struct pointers to OMXCam ones */
	pZoomFactor = &pOMXCamera->mZoomFactorCmdType;
	pCenterFOV = &pOMXCamera->mCenterFieldOfViewCmdType;
	pZoomFactorStatus = &pOMXCamera->mZoomFactorStatusType;
	pCenterFOVStatus = &pOMXCamera->mCenterFieldOfViewStatusType;
		
	/* setup all default values */
	ZoomCmd = ZoomCommand_e_None; 
	pZoomFactor->nPortIndex = OMX_ALL;
	pZoomFactor->xZoomFactor.nValue = 0x10000;
	pZoomFactor->xZoomFactor.nMin= 0x10000;
	pZoomFactor->xZoomFactor.nMax= 0x10000;

	ZoomFactor.nPortIndex = OMX_ALL;
    ZoomFactor.nSize = sizeof(ZoomFactor);
	ZoomFactor.xZoomFactor.nValue = 0x10000;
	ZoomFactor.xZoomFactor.nMin= 0x10000;
	ZoomFactor.xZoomFactor.nMax= 0x10000;

	pCenterFOV->nPortIndex = OMX_ALL;
	pCenterFOV->sPoint.nX = 0;
	pCenterFOV->sPoint.nY = 0;
	CenterFOV = * pCenterFOV;

	CenterFOV.nPortIndex = OMX_ALL;
	CenterFOV.sPoint.nX = 0;
	CenterFOV.sPoint.nY = 0;
	        
	internalZoomConfig = internalZoomConfig_none;
	maxFOVXAtCurrentCenter = 0;
	maxFOVXAtZeroCenter = 0;
	maxFOVYAtZeroCenter = 0;
	minXFieldOfView = 0;
    minYFieldOfView = 0;
	changeoverIsNeeded = OMX_FALSE;
	requestDenied = OMX_FALSE;
	outOfRange = OMX_FALSE;
	
	pZoomFactorStatus->nPortIndex = OMX_ALL;
	pZoomFactorStatus->xZoomFactor.nValue = 0x10000;
	pZoomFactorStatus->xZoomFactor.nMin = 0x10000;
	pZoomFactorStatus->xZoomFactor.nMax = 0x10000;
	pCenterFOVStatus->nPortIndex = OMX_ALL;
	pCenterFOVStatus->sPoint.nX = 0;
	pCenterFOVStatus->sPoint.nY = 0;

	SetCenterRequestdone = OMX_FALSE;
	//minYFieldOfView =
      
}

/* TODO : take into account scene orientation */
float CZoom::getMaxZoomForUsecase(CSensor* pSensor)
{
	float maxZoom = 0;
    
	/* still usecase */
	if (OMX_TRUE == pSensorMode->bOneShot)
	{
    	t_uint32 sensorWidth = 0;
        camport* previewPort = (camport *) pOMXCam->getPort(0);
    	sensorWidth = pSensor->getBayerWidth();
        
        /* we do not want upscale > 6.0 */
        maxZoom = sensorWidth * 6.0 / previewPort->mInternalFrameWidth;
	}
	/* video usecase */
	else
	{
		maxZoom = 30.0;
    }
    
	return (maxZoom);    
}


float CZoom::calculateFOV(OMX_SYMBIAN_CONFIG_POINTTYPE* pPoint)
{
	float XFieldOfView = 0;
	float YFieldOfView = 0;
	float res = 0.0;
	float fovY = 0.0;

	t_uint32 XFOVFloor = 0;

	//For Q16 correction, F/W expects values in integer, so, need to convert Q16 percentage values to integer
	camport * portVF = (camport *)pOMXCam->getPort(0);
//	OMX_PARAM_PORTDEFINITIONTYPE port_params; // Unused variable warning

//	port_params = portVF->getParamPortDefinition();
	OMX_S32 resX = portVF->mInternalFrameWidth;
	OMX_S32 resY = portVF->mInternalFrameHeight;

	res=((float)resX/(float)resY);
	fovY= (1/res)*(maxFOVXAtCurrentCenter);

	OMX_S32 temp = pPoint->sPoint.nX;
	temp = (temp*maxFOVXAtCurrentCenter)/(2*65536);
	pPoint->sPoint.nX = temp;

	temp = pPoint->sPoint.nY;
	temp = (temp*fovY)/(2*65536);
	pPoint->sPoint.nY = temp;

	//Q16 Correction End

	if ((pCenterFOVStatus->sPoint.nX == pPoint->sPoint.nX ) && 
		(pCenterFOVStatus->sPoint.nY == pPoint->sPoint.nY  ))
	{
		/*No Center change, but consider zoom factor */
		/* Rq : xZoomFactor is in Q16 format */
		XFieldOfView = maxFOVXAtCurrentCenter  * (0x10000/(float)pZoomFactor->xZoomFactor.nValue);
	}
	else
	{
		/*Do not consider zoom factor as center is changed, current zoom will be max field of view at current center
		   so zoom factor will be 1 */
		
		if (0 < pPoint->sPoint.nX ) 
		{
			XFieldOfView = 2 * ((OMX_S32)(maxFOVXAtZeroCenter/2) - pPoint->sPoint.nX);
		}
		else
		{
			XFieldOfView = 2 * ((OMX_S32)(maxFOVXAtZeroCenter/2) + pPoint->sPoint.nX);
		}

		/* check if field of view is not less than the minimum XFieldOfView allowed */
		if( XFieldOfView < minXFieldOfView)
		{
			XFieldOfView =	minXFieldOfView;
		}

		if (0 < pPoint->sPoint.nY ) 
		{
			YFieldOfView = 2 * ((OMX_S32)(maxFOVYAtZeroCenter/2) - pPoint->sPoint.nY);
		}
		else
		{
			YFieldOfView = 2 * ((OMX_S32)(maxFOVYAtZeroCenter/2) + pPoint->sPoint.nY);
		}
		
		camport* previewPort = (camport *) pOMXCam->getPort(CAMERA_PORT_OUT0);
		OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition = previewPort->getParamPortDefinition();
		
		float UserAspectRatio = (float)mParamPortDefinition.format.video.nFrameWidth/mParamPortDefinition.format.video.nFrameHeight;
		float CalculatedAspectRatio = XFieldOfView/YFieldOfView;

		/* calculating minimum YField of view */
		minYFieldOfView=(1/UserAspectRatio)*minXFieldOfView;

		/* check if field of view is not less than the minimum YFieldOfView allowed */
		if( YFieldOfView < minYFieldOfView)
		{
			YFieldOfView =	minYFieldOfView;
		}

		if (UserAspectRatio < CalculatedAspectRatio)
		{
			/*We need to cut field of view in X direction*/
			XFieldOfView = UserAspectRatio * YFieldOfView;
			
		}

		/* Here, the field of view is set to nearest lower integer as the calculation in floating point (by ARM) for YFOV
		 *  results in faulty calculation of XFOV which results in test failure when | nY | = 1
		 *  The faulty calculation comes from the fact that ARM calculates a wrong value of XFOV due to some loss
		 *  of precision while calculating the floating point no.
		 *  */

		XFOVFloor = XFieldOfView;
		XFieldOfView = XFOVFloor;
	}
	return XFieldOfView;
}


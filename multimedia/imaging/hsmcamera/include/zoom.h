/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ZOOM_H_
#define _ZOOM_H_

#include "OMX_Image.h"
#include "VhcElementDefs.h"
#include "sensor.h"
#include "omxcamera.h"
#include "IFM_Types.h"

typedef enum {internalZoomConfig_none, internalZoomConfig_resetFOV, internalZoomConfig_resetFOVandCenter,
		internalZoomConfig_setCurrentFOVandCenter} t_internalZoomConfig;
							
#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CZoom);
#endif
class CZoom
{

	public :
		CZoom(COmxCamera* pOMXCamera, OMX_PARAM_SENSORMODETYPE* pSensorMde);
			
		float getMaxZoomForUsecase(CSensor* iSensor);
		void getNextZoomCOmmand(void);
		float calculateFOV(OMX_SYMBIAN_CONFIG_POINTTYPE* pPoint);
		OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE* pZoomFactor;
		OMX_SYMBIAN_CONFIG_POINTTYPE* pCenterFOV;
		OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE* pZoomFactorStatus;
		OMX_SYMBIAN_CONFIG_POINTTYPE* pCenterFOVStatus;
				
		ZoomCommand_te ZoomCmd; /* zoom command type */
		OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE ZoomFactor;
		OMX_SYMBIAN_CONFIG_POINTTYPE CenterFOV;        
		t_internalZoomConfig internalZoomConfig;
		
		float maxFOVXAtCurrentCenter; /* maximum X Field of View available at current center */
		float maxFOVXAtZeroCenter; /* maximum X Field of View available at zero center */
		float maxFOVYAtZeroCenter; /* maximum Y Field of View available at zero center */
		float minXFieldOfView; /*minimum value of field of view allowed in X*/
		float minYFieldOfView; /*minimum value of field of view allowed in Y*/
		
		OMX_BOOL changeoverIsNeeded; /* sensor changeover is needed to perform zoom command */
		OMX_BOOL requestDenied; /* TRUE if zoom command is denied */
		OMX_BOOL outOfRange; /* TRUE if zoom command is moderated */
        OMX_BOOL SetCenterRequestdone;
		
	
	protected :
		COmxCamera* pOMXCam;
		OMX_PARAM_SENSORMODETYPE* pSensorMode;
};
#endif /**/

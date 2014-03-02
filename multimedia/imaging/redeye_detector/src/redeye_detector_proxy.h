/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _REDEYE_DETECTOR_PROXY_H_
#define _REDEYE_DETECTOR_PROXY_H_

#include <ENS_Component.h>
#include <ENS_Port.h>
#include <ENS_Nmf.h>
#include <ENS_List.h>
#include <OMX_Index.h>
#include <OMX_Core.h>
#include "RED_OMX_extensions.h"
#include <cm/inc/cm.hpp>
#include <cm/inc/cm_type.h>

class ENS_Component;

//Factory method for components RED REC and Norcos
OMX_ERRORTYPE Factory_RedEyeDetection     (ENS_Component ** ppENSComponent);


class RedEyeDetector: public OpenMax_Component
//*************************************************************************************************************
{
public:
	RedEyeDetector();
	virtual ~RedEyeDetector();
	virtual int Construct(); //Make the real constuction
	virtual void *        getConfigAddr(OMX_INDEXTYPE nConfigIndex, size_t *StructSize=NULL, int *pOffset=NULL) const;
	virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const;

	int GetThresholdBitClassRed() const {return(config_FILTER.ThresholdBitClassRed); }
	int GetThresholdBitClassGolden() const {return(config_FILTER.ThresholdBitClassGolden); }
			
protected:

	IFM_CONFIG_RED_THRESHOLD             config_FILTER;
};

#ifdef __EPOC32__
NONSHARABLE_CLASS(RedEyeDetector);
#endif

#endif // _REDEYE_DETECTOR_PROXY_H_

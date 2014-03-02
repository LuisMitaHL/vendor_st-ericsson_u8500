/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _FACE_DETECTOR_PROXY_H_
#define _FACE_DETECTOR_PROXY_H_

#include <ENS_Component.h>
#include <ENS_Port.h>
#include <ENS_Nmf.h>
#include <ENS_List.h>
#include <OMX_Index.h>
#include <OMX_Core.h>
#include <cm/inc/cm.hpp>
#include <cm/inc/cm_type.h>
#include "../OMX_extensions/face_detector_extensions.h"

#include <OMX_IndexExt.h>
#include <OMX_CoreExt.h>
#ifdef __SYMBIAN32__
	#include <OMX_Symbian_CameraExt.h>      // for OMX_SYMBIAN_CONFIG_ROITYPE
	#include <OMX_Symbian_CameraIndexExt.h> // for OMX_Symbian_IndexConfigROI
#else
	#include <OMX_Symbian_CameraExt_Ste.h>      // for OMX_SYMBIAN_CONFIG_ROITYPE
	#include <OMX_Symbian_CameraIndexExt_Ste.h> // for OMX_Symbian_IndexConfigROI
#endif

class ENS_Component;

class Face_Detector_Component: public OpenMax_Component
//*************************************************************************************************************
{
public:
	Face_Detector_Component();
	virtual ~Face_Detector_Component();
	virtual int                    Construct        (); //Make the real constuction	
	virtual OMX_ERRORTYPE          setConfig        (OMX_INDEXTYPE nIndex, OMX_PTR pStructure);
	virtual OMX_ERRORTYPE          getConfig        (OMX_INDEXTYPE nConfigIndex, OMX_PTR pConfigStructure) const;
	virtual int                    NMFConfigure     (); /// Called during the initialisation of the NMF part of the component 
	virtual void *                 getConfigAddr    (OMX_INDEXTYPE nConfigIndex, size_t *StructSize=NULL, int *pOffset=NULL) const;
	virtual OMX_ERRORTYPE          getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const;

protected:
	OMX_SYMBIAN_CONFIG_ROITYPE     ROIs;
	OMX_CONFIG_CALLBACKREQUESTTYPE CallbackRequest;
	OMX_INDEXTYPE                  IndexConfigROI;
	OMX_INDEXTYPE                  IndexConfigCallbackRequest;

};

OMX_ERRORTYPE Factory_face_detector(ENS_Component_p * ppENSComponent);


#endif // _FACE_DETECTOR_PROXY_H_

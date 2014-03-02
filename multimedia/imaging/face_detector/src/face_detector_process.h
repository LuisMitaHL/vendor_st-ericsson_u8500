/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _FACE_DETECTOR_PROCESS_H_
#define _FACE_DETECTOR_PROCESS_H_
#include <stdio.h>
#include <OMX_Types.h>
#include <OMX_Component.h>
#include <Component.h> // because we NMF-extend fsm.component.component
#include <IFM_Types.h>

#ifdef __SYMBIAN32__
	#include <OMX_Symbian_CameraExt.h>      // for OMX_SYMBIAN_CONFIG_ROITYPE
	#include <OMX_Symbian_CameraIndexExt.h> // for OMX_Symbian_IndexConfigROI
#else
	#include <OMX_Symbian_CameraExt_Ste.h>      // for OMX_SYMBIAN_CONFIG_ROITYPE
	#include <OMX_Symbian_CameraIndexExt_Ste.h> // for OMX_Symbian_IndexConfigROI
#endif

class face_detector_proc: public openmax_processor
//*************************************************************************************************************
{
public:
	face_detector_proc();
	virtual ~face_detector_proc();
	virtual void process();
	const OMX_SYMBIAN_CONFIG_ROITYPE &GetROI() const { return(ROIs); }
	enum
	{
		eInputPort_Img       = 0,
		eOutputPort_Img      = -1, /* no output port   */
		eOutputPort_Metadata = -1, /* no metadata port */
	};
protected:
	virtual int GetConvertionConfig (_tConvertionConfig &Config, bool bDequeue);
	OMX_INDEXTYPE      IndexConfigROI;
	OMX_SYMBIAN_CONFIG_ROITYPE ROIs;
};


#endif //_FACE_DETECTOR_PROCESS_H_

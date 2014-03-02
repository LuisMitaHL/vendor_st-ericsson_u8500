/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _NORCOS_PROXY_H_
#define _NORCOS_PROXY_H_

#include <ENS_Component.h>
#include <ENS_Port.h>
#include <ENS_Nmf.h>
#include <ENS_List.h>
#include <OMX_Index.h>
#include <OMX_Core.h>
#include <cm/inc/cm.hpp>
#include <cm/inc/cm_type.h>
#include "../OMX_extensions/norcos_OMX_extensions.h"

// Uncomment for disabling buffer sharing
//#define NO_OMX_BUFFER_SHARING  
// Set to 0 or comment if you want to disable the internal buffer sharing into the algo lib

class ENS_Component;

class Norcos: public OpenMax_Component
//*************************************************************************************************************
{
public:
	Norcos();
	virtual ~Norcos();
	virtual int Construct(); //Make the real constuction
	virtual void *        getConfigAddr(OMX_INDEXTYPE nConfigIndex, size_t *StructSize=NULL, int *pOffset=NULL) const;
	virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const;
	virtual int NMFConfigure(); /// Make the initialisation for the NMF part of the component 

	enum
	{
		eError_New0=OpenMax_Component::eError_LastEnum,
		eError_LastEnum,
	};

protected:
	int iEnableWriting;
	IFM_CONFIG_NORCOS_CONTROLMODE config_ControlMode;
	// IFM_CONFIG_FILTER             config_FILTER;
};

#endif // _NORCOS_PROXY_H_

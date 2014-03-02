/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _ARMIVPROC_PROXY_H_
#define _ARMIVPROC_PROXY_H_

#include <ENS_Component.h>
#include <ENS_Port.h>
#include <ENS_Nmf.h>
#include <ENS_List.h>
#include <OMX_Index.h>
#include <OMX_Core.h>
#include <cm/inc/cm.hpp>
#include <cm/inc/cm_type.h>
#include "../OMX_extensions/armivproc_extensions.h"

class ENS_Component;

class ArmIVProc: public OpenMax_Component
//*************************************************************************************************************
{
public:
	ArmIVProc();
	virtual ~ArmIVProc();
	virtual int Construct(); //Make the real constuction
	virtual int NMFConfigure(); /// Make the initialisation for the NMF part of the component 
	virtual int PortFormatChanged(ENS_Port & port );

	
	virtual void *        getConfigAddr(OMX_INDEXTYPE nConfigIndex, size_t *StructSize=NULL, int *pOffset=NULL) const;
	virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const;

	bool GetAutoRotateEnable() const {return(m_bAutoRotateEnable); }

	OMX_CONFIG_ROTATIONTYPE m_Rotation;
	OMX_CONFIG_MIRRORTYPE   m_Mirror;
protected:

	bool m_bAutoRotateEnable;
};

OMX_ERRORTYPE Factory_ArmIVProc(ENS_Component_p * ppENSComponent);


#endif // _ARMIVPROC_PROXY_H_

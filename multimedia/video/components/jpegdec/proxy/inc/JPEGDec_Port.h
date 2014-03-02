/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __JPEGDEC_PORT_H
#define __JPEGDEC_PORT_H

#include "VFM_Port.h"

class JPEGDec_Port:public VFM_Port
{
public:

	JPEGDec_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);
	virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentParameterStructure);
	OMX_ERRORTYPE setIndexParamImagePortFormat(OMX_IMAGE_PARAM_PORTFORMATTYPE *portDef);
	OMX_ERRORTYPE getIndexParamImagePortFormat(OMX_IMAGE_PARAM_PORTFORMATTYPE *portDef);
	virtual void getSlavedSizeFromGivenMasterSize(OMX_U32 nWidthMaster, OMX_U32 nHeightMaster, OMX_U32 *nWidthSlave, OMX_U32 *nHeightSlave) const;

protected:
	virtual OMX_U32 getBufferSize() const;
	virtual OMX_ERRORTYPE checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef);
	void jpegdec_port_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal);
};

#endif  //__JPEGDEC_PORT_H

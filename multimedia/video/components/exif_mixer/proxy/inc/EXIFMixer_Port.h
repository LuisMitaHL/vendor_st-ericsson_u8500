/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "EXIFMixer_Proxy.h"
#include "EXIFMixer_ParamAndConfig.h"
#include "VFM_Port.h"

class EXIFMixer_Port:public VFM_Port
{
public:
	EXIFMixer_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);
    virtual ~EXIFMixer_Port();
    void exif_mixer_port_assert(OMX_U32 condition, OMX_U32 errorType, OMX_U32 line, OMX_BOOL isFatal);

protected:
	virtual OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& portDef);
	virtual OMX_U32 getBufferSize() const;
	virtual OMX_ERRORTYPE checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef);

};


/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _CAM_PORT_STATS_H_
#define _CAM_PORT_STATS_H_

#include <ENS_Port.h>
#include "OMX_3A_OtherExt.h"

class camportstats: public ENS_Port {
public:
  camportstats(OMX_U32 nPortIndex, OMX_DIRTYPE eDir, OMX_BUFFERSUPPLIERTYPE eSupplierPref, OMX_SEMC_OTHER_FORMATTYPE eFormat, ENS_Component &enscomp);

  OMX_PARAM_PORTDEFINITIONTYPE getParamPortDefinition(void) const {
    return mParamPortDefinition;
  }

  virtual OMX_ERRORTYPE setParameter(
    OMX_INDEXTYPE nParamIndex,
    OMX_PTR pComponentParameterStructure);

  virtual OMX_ERRORTYPE getParameter(
    OMX_INDEXTYPE nParamIndex,
    OMX_PTR pComponentParameterStructure) const;
protected:

  virtual OMX_ERRORTYPE setFormatInPortDefinition(
    const OMX_PARAM_PORTDEFINITIONTYPE& portDef);

  virtual OMX_ERRORTYPE checkCompatibility(
    OMX_HANDLETYPE hTunneledComponent,
    OMX_U32 nTunneledPort) const;

};
#endif // _CAM_PORT_STATS_H_

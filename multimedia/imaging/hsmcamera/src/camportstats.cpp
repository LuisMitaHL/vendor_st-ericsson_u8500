/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "camportstats.h"
//extern "C" {
//#include <cm/inc/cm_macros.h>
#include <cm/inc/cm.hpp>
//}

#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x11
#define OMXCOMPONENT "CAMORT"
#include "osi_trace.h"

/* */
/* camportstats */
/**/
camportstats::camportstats(OMX_U32 nPortIndex, OMX_DIRTYPE eDir, OMX_BUFFERSUPPLIERTYPE eSupplierPref, OMX_SEMC_OTHER_FORMATTYPE eFormat, ENS_Component &enscomp)
  : ENS_Port(
    EnsCommonPortData(
      nPortIndex,
      eDir,
      1,
      0, // TO determined in camportstats
      OMX_PortDomainOther,
      eSupplierPref),
    enscomp)
{
  IN0("");
  // TODO: change
  mParamPortDefinition.format.other.eFormat = (OMX_OTHER_FORMATTYPE)eFormat;
  switch(eFormat)
  {
    case OMX_SEMC_OTHER_FormatStatsAewb:
      mParamPortDefinition.nBufferSize = sizeof(OMX_SEMC_OTHER_STATSAEWBDATATYPE);
      break;
    case OMX_SEMC_OTHER_FormatStatsAf:
      mParamPortDefinition.nBufferSize = sizeof(OMX_SEMC_OTHER_STATSAFDATATYPE);
      break;
    default:
      break;
  }
  OUTR("",OMX_ErrorNone);
}

/* */
/* camportstats::setFormatInPortDefinition */
/**/
OMX_ERRORTYPE camportstats::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& portDef)
{
  IN0("");
  OMX_SEMC_OTHER_FORMATTYPE eFormat = (OMX_SEMC_OTHER_FORMATTYPE)portDef.format.other.eFormat;
  if (eFormat != OMX_SEMC_OTHER_FormatStatsAf &&
      eFormat != OMX_SEMC_OTHER_FormatStatsAewb)
  {
    OUTR("",OMX_ErrorBadParameter);
    return OMX_ErrorBadParameter;
  }
  OUTR("",OMX_ErrorNone);
  return OMX_ErrorNone;
}


/* */
/* camportstats::checkCompatibility */
/**/
OMX_ERRORTYPE camportstats::checkCompatibility(OMX_HANDLETYPE hTunneledComponent,OMX_U32 nTunneledPort) const {
  IN0("");
  OUTR("",OMX_ErrorNone);
  return OMX_ErrorNone;
}

typedef struct CAM_PORT_INDEX_STRUCT {
  OMX_U32 nSize;
  OMX_VERSIONTYPE nVersion;
  OMX_U32 nPortIndex;
} CAM_PORT_INDEX_STRUCT;

OMX_ERRORTYPE camportstats::setParameter(
  OMX_INDEXTYPE nParamIndex,
  OMX_PTR pComponentParameterStructure)
{
  OMX_ERRORTYPE error = OMX_ErrorNone;
  switch (nParamIndex)
  {
    case OMX_IndexParamCommonSensorMode:
    {
      CAM_PORT_INDEX_STRUCT *portIdxStruct =
        static_cast<CAM_PORT_INDEX_STRUCT *>(pComponentParameterStructure);
      (void)portIdxStruct->nPortIndex;
      /* check that indeed it is an OMX_ALL else return an error */
      MSG1("camportstats::setParameter, nPortIndex %d\n",portIdxStruct->nPortIndex);
      /* dont do anything yet */
      break;
    }
    default :
      error = ENS_Port::setParameter(nParamIndex,pComponentParameterStructure);
      break;
  }
  return error;
}



OMX_ERRORTYPE camportstats::getParameter(
  OMX_INDEXTYPE nParamIndex,
  OMX_PTR pComponentParameterStructure) const
{
  OMX_ERRORTYPE error = OMX_ErrorNone;
  switch (nParamIndex)
  {
    case OMX_IndexParamCommonSensorMode:
    {
      CAM_PORT_INDEX_STRUCT *portIdxStruct =
        static_cast<CAM_PORT_INDEX_STRUCT *>(pComponentParameterStructure);
      (void)portIdxStruct->nPortIndex;
      /* check that indeed it is an OMX_ALL else return an error */
      MSG1("camportstats::getParameter, nPortIndex %d\n",portIdxStruct->nPortIndex);
      /* dont do anything yet */
      break;
    }
    default :
      error = ENS_Port::getParameter(nParamIndex,pComponentParameterStructure);
      break;
  }
  return error;
}

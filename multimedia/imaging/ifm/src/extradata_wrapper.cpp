/*
 * copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "extradata.h"
#include "extradata_wrapper.h"

static OMX_ERRORTYPE SetExifParams(OMX_BUFFERHEADERTYPE* pOmxBufHdr,
                                   IFM_EXIF_SETTINGS* pExtradataData,
                                   OMX_VERSIONTYPE version)
{
  return Extradata::SetExtradataField(pOmxBufHdr, pExtradataData, version);
}

static Extradata_WrapperInterface_t gExtradataWrapperInterface =
{
  SetExifParams
};

Extradata_WrapperInterface_t* GetExtradataWrapper(void)
{
  return &gExtradataWrapperInterface;
}

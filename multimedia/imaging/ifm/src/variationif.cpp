/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "variationif.h"

VariationIf::VariationIf() {
}

VariationIf::~VariationIf() {
}

OMX_ERRORTYPE VariationIf::listOfResolutions(t_sensorDesciptor* sensor_id, t_sensorResolutions* resolutions,
					     int *numberOfNonActiveLines)
{
  *numberOfNonActiveLines = -1;
  return OMX_ErrorNotImplemented;
}

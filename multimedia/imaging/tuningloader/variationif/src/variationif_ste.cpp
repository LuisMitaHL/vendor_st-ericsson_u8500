/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "variationif_ste.h"

VariationIf_Ste::VariationIf_Ste()
{
}

VariationIf_Ste::~VariationIf_Ste()
{
}

VariationIf* VariationIf::getInstance()
{
  return new VariationIf_Ste();
}

void VariationIf::releaseInstance(VariationIf * instance)
{
  VariationIf_Ste *i = static_cast<VariationIf_Ste*>(instance);
  delete i;
}


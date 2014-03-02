/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _STE_ENS_IMAGING_COMPONENTS_H_
#define _STE_ENS_IMAGING_COMPONENTS_H_

#include <ENS_Component.h>

OMX_ERRORTYPE OMXExtCameraFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE OMXExtCameraSecondaryFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));

#endif /* _STE_ENS_IMAGING_COMPONENTS_H_ */


/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste_ens_imaging_weakcomponents.cpp
 * \brief   Define imaging component weak factory methods
 */

#include "ext_ste_ens_imaging_components.h"

OMX_ERRORTYPE OMXCameraFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE OMXExtCameraSecondaryFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}

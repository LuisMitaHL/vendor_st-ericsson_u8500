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

#include "ste_ens_imaging_components.h"

OMX_ERRORTYPE CameraFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE pointerFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE OMXCameraFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}

/*
OMX_ERRORTYPE OMXExtCameraFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}
*/

OMX_ERRORTYPE imagingfilterFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}

/*
OMX_ERRORTYPE displaysinkFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE swispprocFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE fakesinkFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE clockSourceFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}
*/

OMX_ERRORTYPE OMXCameraSecondaryFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}

/*
OMX_ERRORTYPE OMXExtCameraSecondaryFactoryMethod(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}
*/

OMX_ERRORTYPE OMXISPProcFactoryMethod(ENS_Component_p *ppENSComponent) {
	    return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE OMXISPProcSecondaryFactoryMethod(ENS_Component_p *ppENSComponent) {
	    return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE imageSplitterFactoryMethod (ENS_Component_p *ppENSComponent) {
	    return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE fakesourceFactoryMethod(ENS_Component_p *ppENSComponent)  {
	    return OMX_ErrorComponentNotFound;
}
OMX_ERRORTYPE Factory_RedEyeDetection(ENS_Component_p *ppENSComponent) {
    return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE Factory_RedEyeCorrection(ENS_Component_p *ppENSComponent) {
   return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE Factory_Norcos(ENS_Component_p *ppENSComponent) {
   return OMX_ErrorComponentNotFound;
}

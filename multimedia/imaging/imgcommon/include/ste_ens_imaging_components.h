/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _STE_ENS_IMAGING_COMPONENTS_H_
#define _STE_ENS_IMAGING_COMPONENTS_H_

#include <ENS_Component.h>

OMX_ERRORTYPE CameraFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE pointerFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE OMXCameraFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
//OMX_ERRORTYPE OMXExtCameraFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE imagingfilterFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
//OMX_ERRORTYPE displaysinkFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
//OMX_ERRORTYPE swispprocFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
//OMX_ERRORTYPE fakesinkFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
//OMX_ERRORTYPE clockSourceFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE OMXCameraSecondaryFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
//OMX_ERRORTYPE OMXExtCameraSecondaryFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE OMXISPProcFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE OMXISPProcSecondaryFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE imageSplitterFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE fakesourceFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE Factory_RedEyeDetection(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE Factory_RedEyeCorrection(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE Factory_Norcos(ENS_Component_p *ppENSComponent) __attribute__((weak));

#endif /* _STE_ENS_IMAGING_COMPONENTS_H_ */


/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* 
 * \file    ste_ens_video_components.h
 * \brief   Define the ENS components prototype of ENS components factory for video
 */
#ifndef _STE_ENS_VIDEO_COMPONENTS_H_
#define _STE_ENS_VIDEO_COMPONENTS_H_

#include <ENS_Component.h>

OMX_ERRORTYPE MPEG2DecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE MPEG4DecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE MPEG4DecFactoryMethodHOST(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE MPEG4EncFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE MPEG4EncFactoryMethodHOST(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE MPEG4EncFactoryMethodMPC(ENS_Component_p *ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE H264DecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE H264EncFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE VC1DecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE VP6DecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE JPEGDecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE JPEGEncFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE ValSourceFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE ValSinkFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE JPEGDecFactoryMethodHOST(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE JPEGDecFactoryMethodMPC(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE JPEGEncFactoryMethodHOST(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE JPEGEncFactoryMethodMPC(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE JPEGEncFactoryMethodHVA(ENS_Component_p *ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE RotateResizeFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE EXIFMixerFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));

#endif /* _STE_ENS_VIDEO_COMPONENTS_H_ */



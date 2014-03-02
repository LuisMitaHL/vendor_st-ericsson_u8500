/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*
* Author:     : Laurent Regnier <laurent.regnier@stericsson.com>
* Filename:   : splitter_shared.h
* Description : shared header for all cpp files
*****************************************************************************/
#ifndef _SPLITTER_SHARED_H_
#define _SPLITTER_SHARED_H_

#ifndef IMAGESPLITTER_BROADCAST_NAME
	#define IMAGESPLITTER_BROADCAST_NAME   "OMX.ST.ImageSplitter"
#endif

#ifndef IMAGESPLITTER_SEQUENTIAL_NAME
	#define IMAGESPLITTER_SEQUENTIAL_NAME   "OMX.ST.ImageSplitter.SEQUENTIAL"
#endif

#ifndef LANGUAGE_SC
// LANGUAGE_SC is defined by Resource compiler
OMX_ERRORTYPE imageSplitterFactoryMethod             (ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE imageSplitter_Sequential_FactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
#else

#endif

#endif // _SPLITTER_SHARED_H_

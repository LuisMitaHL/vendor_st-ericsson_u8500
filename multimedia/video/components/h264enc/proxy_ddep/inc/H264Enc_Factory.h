/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __H264ENC_FACTORY_HPP
#define __H264ENC_FACTORY_HPP

#if ((!defined __SYMBIAN32__) && (!defined IMPORT_C))
    #define IMPORT_C
#endif
#if ((!defined __SYMBIAN32__) && (!defined EXPORT_C))
    #define EXPORT_C
#endif

#include "ENS_Component.h"
IMPORT_C OMX_ERRORTYPE H264EncFactoryMethod(ENS_Component_p * ppENSComponent);
//+ code for CR 332873
IMPORT_C OMX_ERRORTYPE H264EncFactoryMethodCTCall(ENS_Component_p * ppENSComponent);
//- code for CR 332873

#endif

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef  _AUDIOLIBS_TYPES_H_
#define  _AUDIOLIBS_TYPES_H_

/****************************************************************/ 
/* Definition of 16 bits type      ******************************/
/****************************************************************/ 
#ifndef _NMF_MPC_

typedef unsigned short t_uint16;
typedef unsigned long  t_uint32;

#endif // _NMF_MPC_

#include "api_base_audiolibs_types.h"


#ifdef ARM
#ifdef STM_ARM_MODELS
#include "stm-arm.h"
#endif
#ifdef NEON
#ifdef STM_ARM_NEON_MODELS
#include "stm-arm-neon.h"
#else
#include "arm_neon.h"
#endif /* STM_ARM_NEON_MODELS */
#endif /* NEON */

#endif /* ARM */

#endif   // _AUDIOLIBS_TYPES_H_

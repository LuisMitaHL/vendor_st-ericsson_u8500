/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __INC_ITE_TESTENV_COMPLIANCE_H
#define __INC_ITE_TESTENV_COMPLIANCE_H

#ifdef __cplusplus
extern "C"
{
#endif


/* Cancel FALSE and TRUE definitions done 
   in MMTC_TOOLS\nmf\delivery\api\inc\type.h
   to be able to define type t_bool as "typedef enum {FALSE, TRUE}" 
   in MPU_OSI\nomadik\hcl\include\hcl_defs.h */
#undef FALSE
#undef TRUE

/* Cancel some "MASK_xxx" and "SHIFT_xxx" definitions done 
   in MMTC_TOOLS\nmf\delivery\api\inc\type.h, which are not compliant
   with those done in MPU_OSI\nomadik\hcl\include\hcl_defs.h */
#undef MASK_NULL8
#undef MASK_NULL16
#undef MASK_NULL32
#undef MASK_ALL8
#undef MASK_ALL16
#undef MASK_ALL32
#undef SHIFT_BYTE0
#undef SHIFT_BYTE1
#undef SHIFT_BYTE2
#undef SHIFT_BYTE3
#undef SHIFT_HALFWORD0
#undef SHIFT_HALFWORD1

#ifdef __cplusplus
}
#endif
#endif /* __INC_ITE_TESTENV_COMPLIANCE_H */

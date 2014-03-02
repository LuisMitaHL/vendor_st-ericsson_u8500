/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \if INCLUDE_IN_HTML_ONLY
 \file      GenericFunctions.h
 \brief     Contains generic utility functions that can be utilised
            throughout the project.
 \ingroup   Miscellaneous
 \endif
*/
#ifndef GENERICFUNCTIONS_H_
#   define GENERICFUNCTIONS_H_

#   include "Platform.h"

/// Implements a clip function
#   define GenericFunctions_Clip(f_Max, f_Min, f_Val)  (f_Val > f_Max) ? f_Max : (f_Val < f_Min) ? f_Min : f_Val

#   define ConvertFloatToUnsigned_4_DOT_8_format(x)    (((uint32_t) (x * 256)) & 0x0FFF)
#   define ConvertFloatToUnsigned_8_DOT_8_format(x)    (((uint16_t) (x * 256)) & 0xFFFF)

#define HEX_BASE          16

#define HexChartoInt(c) ((c>='0' && c<='9') ? c-'0' : ((c>='A' && c<='F') ? \
                            c-'A'+10 : ((c>='a' && c<='f') ? c-'a'+10 : 0)))

extern uint32_t GenericFunctions_Ceiling (float_t f_Value);
extern uint32_t GenericFunctions_CeilingOfMByN (uint32_t u32_Numerator, uint32_t u32_Denominator);
extern uint32_t GenericFunctions_Log(uint32_t u32_Operand, uint32_t u32_Base) TO_EXT_DDR_PRGM_MEM;
extern uint32_t GenericFunctions_PowerOfInt(uint32_t u32_Base, uint32_t u32_Exponent) TO_EXT_DDR_PRGM_MEM;
extern int16_t                      GetRounded (float_t f_Input);
#endif /*GENERICFUNCTIONS_H_*/


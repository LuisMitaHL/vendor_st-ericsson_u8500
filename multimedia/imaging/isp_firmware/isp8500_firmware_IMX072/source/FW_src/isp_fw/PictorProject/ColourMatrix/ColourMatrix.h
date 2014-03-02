/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file ColourMatrix.h
 \brief  This file is a part of the release code. It contains the data
        structures, macros, enums and function declarations used by the module.
 \ingroup ColourMatrix
*/
#ifndef _COLOUR_MATRIX_H_
#   define _COLOUR_MATRIX_H_

#   include "ColourMatrix_ip_interface.h"
#   include "ColourMatrix_op_interface.h"

/**
 \enum SwapColour_te
 \brief Colour swapping of individual colour channels
 \ingroup ColourMatrix
*/
typedef enum
{
    /// Replace the channel with red
    SwapColour_e_RedSwap,

    /// Replace the channel with green
    SwapColour_e_GreenSwap,

    /// Replace the channel with blue
    SwapColour_e_BlueSwap
} SwapColour_te;

#   define COLOURMATRIX_CreateDampedMatrixElement(f_ElementValue, f_DamperValue) \
        ((int16_t) ((f_ElementValue * f_DamperValue) * 1024.00))
#   define COLOURMATRIX_ConvertSMIA_8_8_TO_SWORD(x)    (x / 256)
#endif // #ifdef _COLOUR_MATRIX_H_


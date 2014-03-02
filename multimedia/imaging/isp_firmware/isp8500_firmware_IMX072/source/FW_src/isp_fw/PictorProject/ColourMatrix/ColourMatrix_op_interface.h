/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup ColourMatrix Colour Matrix Module
            In order to generate common image data from different SMIA sensors,
            a colour space conversion is required to transform data from the colour space
            of a particular SMIA sensor to some standard colour space.
            As per SMIA specifications, a SMIA sensor must expose a static register map,
            a conversion matrix to transform pixel data from native sensor RGB colour space to
            colour space of sRGB.The individual colour matrix elements are damped before
            they are actually applied onto the hardware.
 \brief ColourMatrix module read Colour matrix from the sensor and apply it to ISP pixel pipe.
*/

/**
 \file ColourMatrix_op_interface.h
 \brief  This file is a part of the colour matrix module release code and provide an
         interface to the module. All functionalities offered by the module are
         available through this file. The file also declare the page elements
         to be used in virtual register list.

 \ingroup ColourMatrix
*/
#ifndef _COLOUR_MATRIX_OP_INTERFACE_H_
#   define _COLOUR_MATRIX_OP_INTERFACE_H_

#   include "Platform.h"

/**
 \struct ColourMatrixFloat_ts
 \brief  Control pages for colour matrix. The values are read from the sensor at BOOT time.
         In every frame these values are programmed to the PIPE.
         User is given flexibility to program the values.
 \ingroup ColourMatrix
*/
typedef struct
{
    /// ColourMatrix[0][0]: matrix_element_RedInRed.\n
    /// range (-8.0,+8.0)
    float_t f_RedInRed;

    /// ColourMatrix[0][1]: matrix_element_GreenInRed.\n
    /// range (-8.0,+8.0)
    float_t f_GreenInRed;

    /// ColourMatrix[0][2]: matrix_element_BlueInRed.\n
    /// range (-8.0,+8.0)
    float_t f_BlueInRed;

    /// ColourMatrix[1][0]: matrix_element_RedInGreen.\n
    /// range (-8.0,+8.0)
    float_t f_RedInGreen;

    /// ColourMatrix[1][1]: matrix_element_GreenInGreen.\n
    /// range (-8.0,+8.0)
    float_t f_GreenInGreen;

    /// ColourMatrix[1][2]: matrix_element_BlueInGreen.\n
    /// range (-8.0,+8.0)
    float_t f_BlueInGreen;

    /// ColourMatrix[2][0]: matrix_element_RedInBlue.\n
    /// range (-8.0,+8.0)
    float_t f_RedInBlue;

    /// ColourMatrix[2][1]: matrix_element_GreenInBlue.\n
    /// range (-8.0,+8.0)
    float_t f_GreenInBlue;

    /// ColourMatrix[2][2]: matrix_element_BlueInBlue.\n
    /// range (-8.0,+8.0)
    float_t f_BlueInBlue;

    /// Swap Red channel with some other colour \n
    /// [DEFAULT]:e_RedSwap(0), No effect on image
    uint8_t e_SwapColour_Red;

    /// Swap Green channel with some other colour \n
    /// [DEFAULT]:e_GreenSwap(1), No effect on image
    uint8_t e_SwapColour_Green;

    /// Swap Blue channel with some other colour \n
    /// [DEFAULT]:e_BlueSwap(2), No effect on image
    uint8_t e_SwapColour_Blue;
} ColourMatrixFloat_ts;

/**
 \struct ColourMatrixCtrl_ts
 \brief  Control pages for colour matrix.
 \details The page elements are offset vector, colour matrix mode and a control coin for manual mode.
         User is given flexibility to program the values.
 \ingroup ColourMatrix
*/
typedef struct
{
    /// ColourMatrixOffset_R.\n
    /// range [-1024,+1023]
    int16_t s16_Offset_R;

    /// ColourMatrixOffset_G.\n
    /// range [-1024,+1023]
    int16_t s16_Offset_G;

    /// ColourMatrixOffset_B.\n
    /// range [-1024,+1023]
    int16_t s16_Offset_B;
} ColourMatrixCtrl_ts;

/**
 \struct ColourMatrix_ts
 \brief  Status pages for current colour matrix applied on the Pipe. The values are in 4.10 format.
 \ingroup ColourMatrix
*/
typedef struct  // Element offset
{
    /// ColourMatrix[0][0]: matrix_element_RedInRed.\n
    /// 14 bit signed values[4.10] fixed point coded
    int16_t s16_RedInRed;

    /// ColourMatrix[0][1]: matrix_element_GreenInRed.\n
    /// 14 bit signed values[4.10] fixed point coded
    int16_t s16_GreenInRed;

    /// ColourMatrix[0][2]: matrix_element_BlueInRed.\n
    /// 14 bit signed values[4.10] fixed point coded
    int16_t s16_BlueInRed;

    /// ColourMatrix[1][0]: matrix_element_RedInGreen.\n
    /// 14 bit signed values[4.10] fixed point coded
    int16_t s16_RedInGreen;

    /// ColourMatrix[1][1]: matrix_element_GreenInGreen.\n
    /// 14 bit signed values[4.10] fixed point coded
    int16_t s16_GreenInGreen;

    /// ColourMatrix[1][2]: matrix_element_BlueInGreen.\n
    /// 14 bit signed values[4.10] fixed point coded
    int16_t s16_BlueInGreen;

    /// ColourMatrix[2][0]: matrix_element_RedInBlue.\n
    /// 14 bit signed values[4.10] fixed point coded
    int16_t s16_RedInBlue;

    /// ColourMatrix[2][1]: matrix_element_GreenInBlue.\n
    /// 14 bit signed values[4.10] fixed point coded
    int16_t s16_GreenInBlue;

    /// ColourMatrix[2][2]: matrix_element_BlueInBlue.\n
    /// 14 bit signed values[4.10] fixed point coded
    int16_t s16_BlueInBlue;

    /// ColourMatrixOffset_R
    int16_t s16_Offset_R;

    /// ColourMatrixOffset_G
    int16_t s16_Offset_G;

    /// ColourMatrixOffset_B
    int16_t s16_Offset_B;
} ColourMatrixFixed_ts;

/************************ Exported Page elements *********************/

/// Control Page element for colour matrix (Both PIPE0 and PIPE1)
/// g_CE_ColourMatrixFloat[0] store the values from Sensor and used for PIPE0
/// g_CE_ColourMatrixFloat[1] store the values from Sensor and used for PIPE1
/// After BOOT, both Pipe0 and Pipe1 have same values and can be modified after BOOT.
extern ColourMatrixFloat_ts g_CE_ColourMatrixFloat[];

/// Control Page element for colour matrix (Both PIPE0 and PIPE1)
/// g_CE_ColourMatrixCtrl[0] for programming the offset vector for PIPE0
/// g_CE_ColourMatrixCtrl[1] for programming the offset vector for PIPE0
extern ColourMatrixCtrl_ts  g_CE_ColourMatrixCtrl[];

/// Status page element for Colour matrix in fixed format
/// g_CE_ColourMatrixDamped[0]: Damped matrix for PIPE0
/// g_CE_ColourMatrixDamped[1]: Damper matrix for PIPE1
extern ColourMatrixFixed_ts g_CE_ColourMatrixDamped[];

/********************    Exported API's  ************************************/
extern void                 ColourMatrix_DamperUpdate (uint8_t u8_PipeNo)TO_EXT_DDR_PRGM_MEM;
extern void                 ColourMatrix_Commit (uint8_t u8_PipeNo)TO_EXT_DDR_PRGM_MEM;
#endif //_COLOUR_MATRIX_OP_INTERFACE_H_


/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file ColourMatrix_ip_interface.h
 \brief This file is NOT a part of the module release code.
            All inputs needed by the Colour matrix module that can only
            be resolved at the project level (at integration time)
            are met through this file.
            It is the responsibility of the integrator to generate
            this file at integration time and meet all the input
            dependencies.


 \note     The following sections define the dependencies that must be
           met by the system integrator. The way these dependencies
           have been resolved here are just for example. These dependencies
           must be appropriately resolved based on the platform being used.
 \ingroup ColourMatrix
*/
#ifndef _COLOUR_MATRIX_IP_INTERFACE_H_
#   define _COLOUR_MATRIX_IP_INTERFACE_H_

#   include "PictorhwReg.h"
#   include "Platform.h"
#   include "SystemConfig.h"
#   include "MasterI2C_op_interface.h"
#   include "ColourMatrix_PlatformSpecific.h"
#   include "Stream.h"

/// No of Pipes in ISP
#   define COLOURMATRIX_NO_OF_HARDWARE_PIPE_IN_ISP NO_OF_HARDWARE_PIPE_IN_ISP

/************************************************************************************************************************/

///                                             PIPE 0 defines

/************************************************************************************************************************/

/// PIPE0 MATRIX_RCOF00
#   define SET_PIPE0_COLOUR_MATRIX_RCOF00_RedInRed(x)  Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_RCOF00_rcof00(x)

/// PIPE0 MATRIX_RCOF01
#   define SET_PIPE0_COLOUR_MATRIX_RCOF01_GreenInRed(x)    Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_RCOF01_rcof01(x)

/// PIPE0 MATRIX_RCOF02
#   define SET_PIPE0_COLOUR_MATRIX_RCOF02_BlueInRed(x) Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_RCOF02_rcof02(x)

/// PIPE0 MATRIX_RCOF10
#   define SET_PIPE0_COLOUR_MATRIX_RCOF10_RedInGreen(x)    Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_RCOF10_rcof10(x)

/// PIPE0 MATRIX_RCOF11
#   define SET_PIPE0_COLOUR_MATRIX_RCOF11_GreenInGreen(x)  Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_RCOF11_rcof11(x)

/// PIPE0 MATRIX_RCOF12
#   define SET_PIPE0_COLOUR_MATRIX_RCOF12_BlueInGreen(x)   Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_RCOF12_rcof12(x)

/// PIPE0 MATRIX_RCOF20
#   define SET_PIPE0_COLOUR_MATRIX_RCOF20_RedInBlue(x) Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_RCOF20_rcof20(x)

/// PIPE0 MATRIX_RCOF21
#   define SET_PIPE0_COLOUR_MATRIX_RCOF21_GreenInBlue(x)   Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_RCOF21_rcof21(x)

/// PIPE0 MATRIX_RCOF22
#   define SET_PIPE0_COLOUR_MATRIX_RCOF22_BlueInBlue(x)    Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_RCOF22_rcof22(x)

/// PIPE0 OFFSET_R
#   define SET_PIPE0_COLOUR_MATRIX_R_OFFSET(x) Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_R_OFFSET(x)

/// PIPE0 OFFSET_G
#   define SET_PIPE0_COLOUR_MATRIX_G_OFFSET(x) Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_G_OFFSET(x)

/// PIPE0 OFFSET_B
#   define SET_PIPE0_COLOUR_MATRIX_B_OFFSET(x) Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_B_OFFSET(x)

/************************************************************************************************************************/

///                                             PIPE 1 defines

/************************************************************************************************************************/

/// PIPE1 MATRIX_RCOF00
#   define SET_PIPE1_COLOUR_MATRIX_RCOF00_RedInRed(x)  Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_RCOF00_rcof00(x)

/// PIPE1 MATRIX_RCOF01
#   define SET_PIPE1_COLOUR_MATRIX_RCOF01_GreenInRed(x)    Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_RCOF01_rcof01(x)

/// PIPE1 MATRIX_RCOF02
#   define SET_PIPE1_COLOUR_MATRIX_RCOF02_BlueInRed(x) Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_RCOF02_rcof02(x)

/// PIPE1 MATRIX_RCOF10
#   define SET_PIPE1_COLOUR_MATRIX_RCOF10_RedInGreen(x)    Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_RCOF10_rcof10(x)

/// PIPE1 MATRIX_RCOF11
#   define SET_PIPE1_COLOUR_MATRIX_RCOF11_GreenInGreen(x)  Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_RCOF11_rcof11(x)

/// PIPE1 MATRIX_RCOF12
#   define SET_PIPE1_COLOUR_MATRIX_RCOF12_BlueInGreen(x)   Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_RCOF12_rcof12(x)

/// PIPE1 MATRIX_RCOF20
#   define SET_PIPE1_COLOUR_MATRIX_RCOF20_RedInBlue(x) Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_RCOF20_rcof20(x)

/// PIPE1 MATRIX_RCOF21
#   define SET_PIPE1_COLOUR_MATRIX_RCOF21_GreenInBlue(x)   Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_RCOF21_rcof21(x)

/// PIPE1 MATRIX_RCOF22
#   define SET_PIPE1_COLOUR_MATRIX_RCOF22_BlueInBlue(x)    Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_RCOF22_rcof22(x)

/// PIPE0 OFFSET_R
#   define SET_PIPE1_COLOUR_MATRIX_R_OFFSET(x) Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_R_OFFSET(x)

/// PIPE0 OFFSET_G
#   define SET_PIPE1_COLOUR_MATRIX_G_OFFSET(x) Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_G_OFFSET(x)

/// PIPE0 OFFSET_B
#   define SET_PIPE1_COLOUR_MATRIX_B_OFFSET(x) Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_B_OFFSET(x)
#endif //_COLOUR_MATRIX_IP_INTERFACE_H_


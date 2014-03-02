/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
 * Gamma_op_interface.h
 *
 *  Created on: Feb 2, 2010
 *      Author: mamtac
 */
#ifndef GAMMA_OP_INTERFACE_H_
#   define GAMMA_OP_INTERFACE_H_

/**
 \defgroup Gamma Module

 \brief Gamma module takes input from Host and apply it to ISP pixel pipe.
*/

/**
 \file Gamma_op_interface.h
 \brief  This file is a part of the Gamma module release code and provide an
         interface to the module. All functionalities offered by the module are
         available through this file. The file also declare the page elements
         to be used in virtual register list.

 \ingroup Gamma
*/
#   include "Platform.h"
#   include "PictorhwReg.h"

typedef enum
{
    //Gamma Curve
    //0 - Gamma Curve Standard
    GammaCurve_Standard,

    //1 - Gamma Curve Custom
    GammaCurve_Custom,

    //2 - Gamma Disable
    GammaCurve_Disable
} GammaCurve_te;


/**
 \struct GammaControl_ts
 \brief  Control pages for Gamma. The values are read from the HOST.
         Host is also given flexibility to program the values in LUT Memories.
 \ingroup Gamma
*/
typedef struct
{
    /// GammaCurve ,either Standard or Custom.
    uint8_t e_GammaCurve;

    /// PixelInShift for Sharp Gamma
    uint8_t u8_GammaPixelInShift_Sharp;

    /// PixelInShift for UnSharp Gamma
    uint8_t u8_GammaPixelInShift_UnSharp;

    /// Control coin available to HOST for controlling Gamma IP
    uint8_t e_Coin_Ctrl;
} GammaControl_ts;




/**
 \struct GammaStatusl_ts
 \brief  Status pages for Gamma. The values are read from the HOST.
         Host is also given flexibility to program the values in LUT Memories.
 \ingroup Gamma
*/
typedef struct
{
    /// GammaCurve ,either Standard or Custom.
    uint8_t e_GammaCurve;

    /// PixelInShift for Sharp Gamma
    uint8_t u8_GammaPixelInShift_Sharp;

    /// PixelInShift for UnSharp Gamma
    uint8_t u8_GammaPixelInShift_UnSharp;

    /// Status coin for fw to know, when to apply the parameters programmed by HOST.
    uint8_t e_Coin_Status;
} GammaStatus_ts;


// This is the structure which will store the shared memory address of the GAMMA Custom values.
// These addresses will be written by ARM. Firmware will use this address to copy the values from shared memory
// and will write it to the gamma LUT tables.
typedef struct
{

    /// This is the base address of the DDR memory provided by ARM where custom values for Sharp Green Channel is given
    uint32_t        u32_SharpGreenLutAddress;

    /// This is the base address of the DDR memory provided by ARM where custom values for Sharp Red Channel is given
    uint32_t        u32_SharpRedLutAddress;

    /// This is the base address of the DDR memory provided by ARM where custom values for Sharp Blue Channel is given
    uint32_t        u32_SharpBlueLutAddress;

    /// This is the base address of the DDR memory provided by ARM where custom values for UnSharp Green Channel is given
    uint32_t        u32_UnSharpGreenLutAddress;

    /// This is the base address of the DDR memory provided by ARM where custom values for UnSharp Red Channel is given
    uint32_t        u32_UnSharpRedLutAddress;

    /// This is the base address of the DDR memory provided by ARM where custom values for UnSharp Blue Channel is given
    uint32_t        u32_UnSharpBlueLutAddress;

} GammaMemLUTAddress_ts;


/**
 \struct GammaLastPixelValueControl_ts
 \brief  Control pages for Gamma Last Pixel Values. The values are read from the HOST.
         Host is also given flexibility to program these values.
 \ingroup Gamma
*/
typedef struct
{
    /// Sharp Last GreenGIR Pixel
    uint16_t    u16_Sharp_Lst_GreenGIR;

    /// Sharp Last Red Pixel
    uint16_t    u16_Sharp_Lst_Red;

    /// Sharp Last Blue Pixel
    uint16_t    u16_Sharp_Lst_Blue;

    /// Sharp Last GreenGIB Pixel
    uint16_t    u16_Sharp_Lst_GreenGIB;

    /// UnSharp Last GreenGIR Pixel
    uint16_t    u16_UnSharp_Lst_GreenGIR;

    /// UnSharp Last Red Pixel
    uint16_t    u16_UnSharp_Lst_Red;

    /// UnSharp Last Blue Pixel
    uint16_t    u16_UnSharp_Lst_Blue;

    /// UnSharp Last GreenGIB Pixel
    uint16_t    u16_UnSharp_Lst_GreenGIB;
} GammaLastPixelValueControl_ts;

/************************ Exported Page elements *********************/

/// Control Page element for Gamma (Both PIPE0 and PIPE1)
/// g_CE_Gamma[0] used for PIPE0 Gamma
/// g_CE_Gamma[1] used for PIPE1 Gamma
/// After BOOT, both Pipe0 and Pipe1 have same values and can be modified after BOOT.
extern GammaControl_ts                               g_CE_GammaControl[];
extern GammaStatus_ts                                g_CE_GammaStatus[];
extern GammaMemLUTAddress_ts                         g_CE_GammaMemLutAddress[];
extern GammaLastPixelValueControl_ts                 g_CE_GammaLastPixelValueControl[];

void                                                 Gamma_Commit (uint8_t u8_PipeNo) TO_EXT_DDR_PRGM_MEM;
extern void                                          Update_Gamma_Pipe(uint8_t pipe_no)TO_EXT_DDR_PRGM_MEM;

#define Gamma_isUpdateGammaRequestPending(pipe)     (g_CE_GammaControl[pipe].e_Coin_Ctrl != g_CE_GammaStatus[pipe].e_Coin_Status)

#endif /* GAMMA_OP_INTERFACE_H_ */


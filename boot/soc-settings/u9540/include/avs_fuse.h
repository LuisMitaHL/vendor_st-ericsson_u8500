/*===========================================================================
| ST-Ericsson
| ===========================================================================
|
|  $RCSfile: /sources/avs_fuse.h$
|  $Revision: 1.3$
|  $Date: Fri Sep 14 08:01:58 2012 GMT$
|
|  $Source: /sources/avs_fuse.h$
|
|  Copyright Statement:
|  -------------------
| The confidential and proprietary information contained in this file may
| only be used by a person authorized under and to the extent permitted
| by a subsisting licensing agreement from ST-Ericsson S.A.
|
| Copyright (C) ST-Ericsson S.A. 2011. All rights reserved.
|
| This entire notice must be reproduced on all copies of this file
| and copies of this file may only be made by a person if such person is
| permitted to do so under the terms of a subsisting license agreement
| from ST-Ericsson S.A..
|
|
| Project :  AP9540
| -------
|
| Description:
| ------------
| Definition of bit field and mask of FUSE registers
|
|===========================================================================
|                    Revision History
|===========================================================================
|
|  $Aliases: $
|
|  $Log: /sources/avs_fuse.h$
|  
|   Revision: 1.3 Fri Sep 14 08:01:58 2012 GMT nxp11764
|   Config pack Soc settings v3.0 - AVS V1 implementation
|  
|   Revision: 1.2 Fri Mar 30 12:30:28 2012 GMT frq06447
|   Update mask for Vbbx in FRV21
|  
|   Revision: 1.1 Tue Mar 13 15:07:19 2012 GMT frq06447
|   AVS fuse field and mask registers definition original
|
|
|  $KeysEnd $
|
|
|===========================================================================*/
/*! @file avs_fuse.h
    @author laetitia Dijoux
    @version 1.1
    This header file contains all Fuse constants.
*/
#ifndef __avs_fuse_h
#define __avs_fuse_h

#ifdef __cplusplus
extern "C" {
#endif
#include "avs_calc.h"

/*! @param tab_fuse_avs_rev
    @brief This  parameter is a table of structure : one table for each AVS revision
 */
/* Parameters are in mV */
const tab_avs_fuse_t tab_fuse_avs_rev[]={
/*-----------*/
/* AVS Rev 0 */
/*-----------*/
{
        /* FRV21 [31:0] register */
        0xFFFFFFFFUL,         //AVS_FRV21_FIELD
        0x0000001FUL,         //THERMAL_SENSOR_OFFSET_FIELD
        0x0,                //THERMAL_SENSOR_OFFSET_SHIFT
        0x5,                //THERMAL_SENSOR_OFFSET_LENGTH
        0x000001E0UL,       //VOLTAGE_SENSOR_OFFSET_FIELD
        0x5,                //VOLTAGE_SENSOR_OFFSET_SHIFT
        0x4,                //VOLTAGE_SENSOR_OFFSET_LENGTH
        0x00007E00UL,       //VOLTAGE_SENSOR_TRIM_FIELD
        0x9,                //VOLTAGE_SENSOR_TRIM_SHIFT
        0x6,                //VOLTAGE_SENSOR_TRIM_LENGTH
        0x00038000UL,       //ARM_VBBN_FIELD
        0xF,                //ARM_VBBN_SHIFT
        0x3,                //ARM_VBBN_LENGTH
        0x3,                //ARM_VBBN_2COMP_MASK
        0x001C0000UL,       //ARM_VBBP_FIELD
        0x12,               //ARM_VBBP_SHIFT
        0x3,                //ARM_VBBP_LENGTH
        0x3,                //ARM_VBBP_2COMP_MASK
        0x03E00000UL,       //ARM_OPP0_OFFSET_FIELD
        0x15,               //ARM_OPP0_OFFSET_SHIFT
        0x5,                //ARM_OPP0_OFFSET_LENGTH
        0xF,                //ARM_OPP0_OFFSET_2COMP_MASK
        0x3C000000UL,       //ARM_OPP1_OFFSET_FIELD
        0x1A,               //ARM_OPP1_OFFSET_SHIFT
        0x4,                //ARM_OPP1_OFFSET_LENGTH
        0x7,                //ARM_OPP1_OFFSET_2COMP_MASK
        0xC0000000UL,       //ARM_OPP2_OFFSET_LSB_FIELD
        0x1E,               //ARM_OPP2_OFFSET_LSB_SHIFT
        /* FRV22 [63 : 32] register but cast on 32 bits [31:0]*/
        0xFFFFFFFFUL,       //AVS_FRV22_FIELD
        0x2,                //ARM_OPP2_OFFSET_SHIFT
        0x00000001UL,       //ARM_OPP2_OFFSET_MSB_FIELD
        0x0,                //ARM_OPP2_OFFSET_MSB_SHIFT
        0x3,                //ARM_OPP2_OFFSET_LENGTH
        0x3,                //ARM_OPP2_OFFSET_2COMP_MASK
        0x0000000EUL,       //ARM_OPP3_OFFSET_FIELD
        0x1,                //ARM_OPP3_OFFSET_SHIFT
        0x3,                //ARM_OPP3_OFFSET_LENGTH
        0x3,                //ARM_OPP3_OFFSET_2COMP_MASK
        0x000000F0UL,       //ARM_OPP4_OFFSET_FIELD
        0x4,                //ARM_OPP4_OFFSET_SHIFT
        0x4,                //ARM_OPP4_OFFSET_LENGTH
        0x7,                //ARM_OPP4_OFFSET_2COMP_MASK
        0x00000700UL,       //ARM_OPP5_OFFSET_FIELD
        0x8,                //ARM_OPP5_OFFSET_SHIFT
        0x3,                //ARM_OPP5_OFFSET_LENGTH
        0x3,                //ARM_OPP5_OFFSET_2COMP_MASK
        0x00700000UL,       //SAFE_OPP2_OFFSET_FIELD
        0x14,               //SAFE_OPP2_OFFSET_SHIFT
        0x3,                //SAFE_OPP2_OFFSET_LENGTH
        0x3,                //SAFE_OPP2_OFFSET_2COMP_MASK
        0x03800000UL,       //SAFE_OPP1_OFFSET_FIELD
        0x17,               //SAFE_OPP1_OFFSET_SHIFT
        0x3,                //SAFE_OPP1_OFFSET_LENGTH
        0x3,                //SAFE_OPP1_OFFSET_2COMP_MASK
        0x1C000000UL,       //APE_OPP2_OFFSET_FIELD
        0x1A,               //APE_OPP2_OFFSET_SHIFT
        0x3,                //APE_OPP2_OFFSET_LENGTH
        0x3,                //APE_OPP2_OFFSET_2COMP_MASK
        0xE0000000UL,       //APE_OPP1_OFFSET_FIELD
        0x1D,               //APE_OPP1_OFFSET_SHIFT
        0x3,                //APE_OPP1_OFFSET_LENGTH
        0x3,                 //APE_OPP1_OFFSET_2COMP_MASK
        0x0003E000UL,       //AVS_FRV6_FIELD
        0x0000E000UL,       //AVS_REV_FIELD
        0xD,                //AVS_REV_SHIFT
        0x3,                //AVS_REV_LENGTH
        0x00010000UL,       //OPP0_MASK_FIELD
        0x10,               //OPP0_MASK_SHIFT
        0x1,                //OPP0_MASK_LENGTH
        0x00020000UL,       //OPP0_ENABLE_FIELD
        0x11,               //OPP0_ENABLE_SHIFT
        0x1,                //OPP0_ENABLE_LENGTH
        0x001E0000UL,       //AVS_FRV7_FIELD
        0x00060000UL,       //CHIP_CONFIG_FIELD
        0x11,               //CHIP_CONFIG_SHIFT
        0x2                 //CHIP_CONFIG_LENGTH
}, \
/*-----------*/
/* AVS Rev 1 */
/*-----------*/
{
    /* FRV21 [31:0] register */
    0xFFFFFFFFUL,       //AVS_FRV21_FIELD
    0x0000001FUL,       //THERMAL_SENSOR_OFFSET_FIELD
    0x0,                //THERMAL_SENSOR_OFFSET_SHIFT
    0x5,                //THERMAL_SENSOR_OFFSET_LENGTH
    0x000001E0UL,       //VOLTAGE_SENSOR_OFFSET_FIELD
    0x5,                //VOLTAGE_SENSOR_OFFSET_SHIFT
    0x4,                //VOLTAGE_SENSOR_OFFSET_LENGTH
    0x00007E00UL,       //VOLTAGE_SENSOR_TRIM_FIELD
    0x9,                //VOLTAGE_SENSOR_TRIM_SHIFT
    0x6,                //VOLTAGE_SENSOR_TRIM_LENGTH
    0x00038000UL,       //ARM_VBBN_FIELD
    0xF,                //ARM_VBBN_SHIFT
    0x3,                //ARM_VBBN_LENGTH
    0x3,                //ARM_VBBN_2COMP_MASK
    0x001C0000UL,       //ARM_VBBP_FIELD
    0x12,               //ARM_VBBP_SHIFT
    0x3,                //ARM_VBBP_LENGTH
    0x3,                //ARM_VBBP_2COMP_MASK
    0x01E00000UL,       //ARM_OPP0_OFFSET_FIELD
    0x15,               //ARM_OPP0_OFFSET_SHIFT
    0x4,                //ARM_OPP0_OFFSET_LENGTH
    0x7,                //ARM_OPP0_OFFSET_2COMP_MASK
    0x1E000000UL,       //ARM_OPP1_OFFSET_FIELD
    0x19,               //ARM_OPP1_OFFSET_SHIFT
    0x4,                //ARM_OPP1_OFFSET_LENGTH
    0x7,                //ARM_OPP1_OFFSET_2COMP_MASK
    0xE0000000UL,       //ARM_OPP2_OFFSET_LSB_FIELD
    0x1D,               //ARM_OPP2_OFFSET_LSB_SHIFT
    /* FRV22 [63 : 32] register but cast on 32 bits [31:0]*/
    0xFFFFFFFFUL,       //AVS_FRV22_FIELD
    0x3,                //ARM_OPP2_OFFSET_SHIFT
    0x00000001UL,       //ARM_OPP2_OFFSET_MSB_FIELD
    0x0,                //ARM_OPP2_OFFSET_MSB_SHIFT
    0x4,                //ARM_OPP2_OFFSET_LENGTH
    0x7,                //ARM_OPP2_OFFSET_2COMP_MASK
    0x0000000EUL,       //ARM_OPP3_OFFSET_FIELD
    0x1,                //ARM_OPP3_OFFSET_SHIFT
    0x3,                //ARM_OPP3_OFFSET_LENGTH
    0x3,                //ARM_OPP3_OFFSET_2COMP_MASK
    0x00000070UL,       //ARM_OPP4_OFFSET_FIELD
    0x4,                //ARM_OPP4_OFFSET_SHIFT
    0x3,                //ARM_OPP4_OFFSET_LENGTH
    0x3,                //ARM_OPP4_OFFSET_2COMP_MASK
    0x00000380UL,       //ARM_OPP5_OFFSET_FIELD
    0x7,                //ARM_OPP5_OFFSET_SHIFT
    0x3,                //ARM_OPP5_OFFSET_LENGTH
    0x3,                //ARM_OPP5_OFFSET_2COMP_MASK
    0x00700000UL,       //SAFE_OPP2_OFFSET_FIELD
    0x14,               //SAFE_OPP2_OFFSET_SHIFT
    0x3,                //SAFE_OPP2_OFFSET_LENGTH
    0x3,                //SAFE_OPP2_OFFSET_2COMP_MASK
    0x03800000UL,       //SAFE_OPP1_OFFSET_FIELD
    0x17,               //SAFE_OPP1_OFFSET_SHIFT
    0x3,                //SAFE_OPP1_OFFSET_LENGTH
    0x3,                //SAFE_OPP1_OFFSET_2COMP_MASK
    0x1C000000UL,       //APE_OPP2_OFFSET_FIELD
    0x1A,               //APE_OPP2_OFFSET_SHIFT
    0x3,                //APE_OPP2_OFFSET_LENGTH
    0x3,                //APE_OPP2_OFFSET_2COMP_MASK
    0xE0000000UL,       //APE_OPP1_OFFSET_FIELD
    0x1D,               //APE_OPP1_OFFSET_SHIFT
    0x3,                //APE_OPP1_OFFSET_LENGTH
    0x3,                //APE_OPP1_OFFSET_2COMP_MASK
    0x0003E000UL,       //AVS_FRV6_FIELD
    0x0000E000UL,       //AVS_REV_FIELD
    0xD,                //AVS_REV_SHIFT
    0x3,                //AVS_REV_LENGTH
    0x00010000UL,       //OPP0_MASK_FIELD
    0x10,               //OPP0_MASK_SHIFT
    0x1,                //OPP0_MASK_LENGTH
    0x00020000UL,       //OPP0_ENABLE_FIELD
    0x11,               //OPP0_ENABLE_SHIFT
    0x1,                //OPP0_ENABLE_LENGTH
    0x001E0000UL,       //AVS_FRV7_FIELD
    0x00060000UL,       //CHIP_CONFIG_FIELD
    0x11,               //CHIP_CONFIG_SHIFT
    0x2                 //CHIP_CONFIG_LENGTH
}
};

#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */

#endif

// --============================ End of file ==============================--
